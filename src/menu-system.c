/*
 * GNUjump
 * =======
 *
 * Copyright (C) 2005-2008, Juan Pedro Bolivar Puente
 *
 * GNUjump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * GNUjump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>

#include "gnujump.h"
#include "menu.h"
#include "game.h"
#include "setup.h"
#include "tools.h"

extern SDL_Surface *screen;
extern L_gblOptions gblOps;

void
initMouse (data_t * gfx, mouse_t * mouse)
{
  int i;

  for (i = 0; i < M_STATES; i++)
    {
      initializeSpriteCtl (&(mouse->sprite[i]), gfx->mouse[i]);
    }
  mouse->id = M_IDLE;
  mouse->clicked = FALSE;
}

void
printMouse (data_t * gfx, mouse_t * mouse)
{
  SDL_Rect dest;
  dest.x = mouse->x - gfx->mouseX;
  dest.y = mouse->y - gfx->mouseY;
  printSprite (&(mouse->sprite[mouse->id]), NULL, &dest, 0);
}

void
unprintMouse (data_t * gfx, mouse_t * mouse)
{
  SDL_Rect dest;
  SDL_Rect src;
  src.x = dest.x = mouse->x - gfx->mouseX;
  src.y = dest.y = mouse->y - gfx->mouseY;
  src.w = mouse->sprite[mouse->id].sdata->pic[0]->w;
  src.h = mouse->sprite[mouse->id].sdata->pic[0]->h;
  JPB_PrintSurface (gfx->menuBg, &src, &dest);
}

void
setMouseState (mouse_t * mouse, int id)
{
  if (mouse->id != id)
    {
      mouse->sprite[id].elpTime = mouse->sprite[id].frame = 0;
      mouse->id = id;
    }
}

void
checkMouse (data_t * gfx, mouse_t * mouse, int *sel, int nops, int off)
{
  int rx = 0, ry = 0;
  int no = MIN (gfx->mMaxOps, nops);

  mouse->clicked = SDL_GetMouseState (&(mouse->x), &(mouse->y));

  if (mouse->x >= gfx->menuX
      && mouse->x < gfx->menuX + gfx->menuW
      && mouse->y >= gfx->menuY
      && mouse->y < gfx->menuY + SFont_TextHeight (gfx->menufont) * no)
    {

      if (((mouse->clicked = SDL_GetRelativeMouseState (&rx, &ry)) || rx != 0
	   || ry != 0))
	*sel =
	  (mouse->y - gfx->menuY) / SFont_TextHeight (gfx->menufont) + off;

      if (!mouse->clicked)
	setMouseState (mouse, M_OVER);
      else
	setMouseState (mouse, M_DOWN);
    }
  else
    {
      if (!mouse->clicked)
	setMouseState (mouse, M_IDLE);
      else
	setMouseState (mouse, M_DOWN);
    }
}

/*
 * NEW MENU SYSTEM =============================================================
 */

void
initMenuT (menu_t * menu)
{
  menu->nops = 0;
  menu->opt = NULL;
}

/*
	Arguments are:
	- menu: pointer to the menu where the option should be included.
	- caption/tip: caption and tip of the option (these won't be freed)
	- flags: some flags defining what kind of option this is
	- data: depends on our option flags.
		+ (*int) for MB_CHOOSE
		+ (**char) for MB_INPUT
		+ (*SDLKey) for MB_KEYDEF
	- nops: NONE if no flags are chosen; 0, if flags != MB_CHOOSE, etc..
	- ...: char* to the name of the different options (if nops>0 :-))
*/
void
addMenuTOption (menu_t * menu, char *caption, char *tip, int flags,
		void *data, int nops, ...)
{
  va_list p;
  opt_t *opt = NULL;
  int i = 0;

  menu->nops++;
  menu->opt = realloc (menu->opt, sizeof (opt_t) * menu->nops);

  opt = &menu->opt[menu->nops - 1];
  opt->caption = opt->tip = NULL;
  opt->caption = caption;
  opt->tip = tip;

  opt->flags = flags;
  opt->nops = nops;
  opt->data = data;
  opt->opcap = NULL;

  if (nops > 0)
    {
      va_start (p, nops);
      opt->opcap = malloc (sizeof (char *) * nops);
      for (i = 0; i < nops; i++)
	{
	  opt->opcap[i] = va_arg (p, char *);
	}
      va_end (p);
    }
  else
    {
      opt->flags |= MB_RETURN;
    }

}

void
freeMenuTOption (opt_t * opt)
{
  free (opt->opcap);
}

void
freeMenuT (menu_t * menu)
{
  int i;
  for (i = 0; i < menu->nops; i++)
    {
      freeMenuTOption (&menu->opt[i]);
    }
  free (menu->opt);
}

int
playMenuT (data_t * gfx, menu_t * menu)
{
  int done = FALSE;
  int prevselect = 0, select = 0;
  fader_t *mfaders;
  fader_t afaders[ARROWS];
  char *tmpstr = NULL;
  SDL_Event event;
  L_timer timer;
  mouse_t mouse;
  int offset = 0, maxops = MIN (menu->nops, gfx->mMaxOps), maxoffset =
    menu->nops - gfx->mMaxOps;
  int i;

  if (menu->nops == 0)
    {
      if (gfx->mback)
	Mix_PlayChannel (-1, gfx->mback, 0);
      return NONE;
    }

  initMouse (gfx, &mouse);
  initTimer (&timer, getFps ());
  mfaders = malloc (sizeof (fader_t) * menu->nops);
  setFader (&mfaders[select], 0, gfx->hlalpha, 1, FALSE);
  for (i = select + 1; i < menu->nops; i++)
    setFader (&mfaders[i], 0, 0, 1, FALSE);
  for (i = 0; i < ARROWS; i++)
    setFader (&afaders[i], 0, 0, 1, FALSE);
  drawMenuT (gfx, menu, offset);
  if (menu->opt[select].tip != NULL)
    drawTip (gfx, menu->opt[select].tip);
  FlipScreen ();

  while (!done)
    {
      updateTimer (&timer);
      for (i = 0; i < menu->nops; i++)
	{
	  updateFader (&mfaders[i], timer.ms);
	}
      for (i = 0; i < ARROWS; i++)
	{
	  updateFader (&afaders[i], timer.ms);
	}
      unprintMouse (gfx, &mouse);
      undrawTip (gfx);
      checkMouse (gfx, &mouse, &select, menu->nops, offset);
      switch (checkMenuKeys (&mouse))
	{
	case KMENTER:
	  if (mouse.id != M_OVER)
	    break;
	case KENTER:
	  if (menu->opt[select].nops > 0)
	    {
	      (*((int *) menu->opt[select].data))++;
	      if (*((int *) menu->opt[select].data) >= menu->opt[select].nops)
		{
		  *((int *) menu->opt[select].data) = 0;
		}
	    }
	  if ((menu->opt[select].flags & MB_INPUT) == MB_INPUT)
	    {
	      tmpstr =
		inputMenu (gfx, menu->opt[select].tip,
			   *((char **) menu->opt[select].data), MAX_CHAR - 1);
	      if (tmpstr != NULL)
		{
		  free (*((char **) menu->opt[select].data));
		  *((char **) menu->opt[select].data) = tmpstr;
		}
	    }
	  if ((menu->opt[select].flags & MB_KEYDEF) == MB_KEYDEF)
	    {
	      for (;;)
		{
		  SDL_WaitEvent (NULL);
		  if (SDL_PollEvent (&event) && event.type == SDL_KEYDOWN)
		    {
		      *((SDLKey *) menu->opt[select].data) =
			event.key.keysym.sym;
		      break;
		    }
		}
	    }
	  if ((menu->opt[select].flags & MB_VOLSET) == MB_VOLSET)
	    resetVolumes ();
	  if ((menu->opt[select].flags & MB_RETURN) == MB_RETURN)
	    done = TRUE;

	  if (gfx->mclick)
	    Mix_PlayChannel (-1, gfx->mclick, 0);

	  break;

	case KUP:
	  select--;
	  if (select < 0)
	    {
	      select = menu->nops - 1;
	    }
	  break;

	case KDOWN:
	  select++;
	  if (select >= menu->nops)
	    select = 0;
	  break;

	case KBACK:
	  select = NONE;
	  done = TRUE;

	  if (gfx->mback)
	    Mix_PlayChannel (-1, gfx->mback, 0);
	  continue;

	case KLEFT:
	  if ((menu->opt[select].flags & MB_CHOOSE) == MB_CHOOSE
	      && menu->opt[select].nops > 0)
	    {
	      (*((int *) menu->opt[select].data))--;
	      if (*((int *) menu->opt[select].data) < 0)
		*((int *) menu->opt[select].data) =
		  menu->opt[select].nops - 1;
	    }
	  if ((menu->opt[select].flags & MB_VOLSET) == MB_VOLSET)
	    resetVolumes ();
	  if ((menu->opt[select].flags & MB_RETURN) == MB_RETURN)
	    done = TRUE;

	  if (gfx->mclick)
	    Mix_PlayChannel (-1, gfx->mclick, 0);

	  break;

	case KRIGHT:
	  if ((menu->opt[select].flags & MB_CHOOSE) == MB_CHOOSE
	      && menu->opt[select].nops > 0)
	    {
	      (*((int *) menu->opt[select].data))++;
	      if (*((int *) menu->opt[select].data) >= menu->opt[select].nops)
		*((int *) menu->opt[select].data) = 0;
	    }
	  if ((menu->opt[select].flags & MB_VOLSET) == MB_VOLSET)
	    resetVolumes ();
	  if ((menu->opt[select].flags & MB_RETURN) == MB_RETURN)
	    done = TRUE;

	  if (gfx->mclick)
	    Mix_PlayChannel (-1, gfx->mclick, 0);

	  break;
	case KMUP:
	  if (mouse.id == M_OVER && offset < maxoffset)
	    offset++;
	  break;
	case KMDOWN:
	  if (mouse.id == M_OVER && offset > 0)
	    offset--;
	  break;
	default:
	  break;
	}
      if (select != prevselect)
	{
	  setFader (&mfaders[prevselect], mfaders[prevselect].value, 0,
		    MENUFADE, FALSE);
	  setFader (&mfaders[select], gfx->hlalpha, gfx->hlalpha, MENUFADE,
		    FALSE);
	  prevselect = select;
	  if (select >= maxops + offset)
	    {
	      offset = select - maxops + 1;
	    }
	  else if (select < offset)
	    {
	      offset = select;
	    }
	}
      if (offset < maxoffset)
	setFader (&afaders[A_DOWN], afaders[A_DOWN].value, SDL_ALPHA_OPAQUE,
		  ABLINKTIME * gblOps.useGL + 1, FALSE);
      else
	setFader (&afaders[A_DOWN], afaders[A_DOWN].value,
		  SDL_ALPHA_TRANSPARENT, ABLINKTIME * gblOps.useGL + 1,
		  FALSE);
      if (offset > 0)
	setFader (&afaders[A_UP], afaders[A_UP].value, SDL_ALPHA_OPAQUE,
		  ABLINKTIME * gblOps.useGL + 1, FALSE);
      else
	setFader (&afaders[A_UP], afaders[A_UP].value, SDL_ALPHA_TRANSPARENT,
		  ABLINKTIME * gblOps.useGL + 1, FALSE);

      for (i = offset; i < offset + maxops; i++)
	{
	  updateFader (&mfaders[i], timer.ms);
	  drawMenuTOption (gfx, i, offset, &menu->opt[i], mfaders[i].value);
	}
      if (menu->opt[select].tip != NULL)
	drawTip (gfx, menu->opt[select].tip);
      drawMenuTArrows (gfx, afaders[A_UP].value, afaders[A_DOWN].value);
      animateSprite (&(mouse.sprite[mouse.id]), timer.ms);
      printMouse (gfx, &mouse);

      FlipScreen ();
    }

  free (mfaders);
  return select;
}

void
undrawTip (data_t * gfx)
{
  SDL_Rect dest;
  dest.x = gfx->tipX;
  dest.y = gfx->tipY;
  dest.w = gfx->tipW;
  dest.h = gfx->tipH;
  JPB_PrintSurface (gfx->menuBg, &dest, &dest);
}

void
drawMenuTArrows (data_t * gfx, int alphaUp, int alphaDown)
{
  SDL_Rect dest;
  gfx->upArrow->alpha = alphaUp;
  gfx->dwArrow->alpha = alphaDown;
  dest.x = gfx->mUpArrowX;
  dest.y = gfx->mUpArrowY;
  dest.w = gfx->upArrow->w;
  dest.h = gfx->upArrow->h;
  JPB_PrintSurface (gfx->menuBg, &dest, &dest);
  dest.x = gfx->mDwArrowX;
  dest.y = gfx->mDwArrowY;
  dest.w = gfx->dwArrow->w;
  dest.h = gfx->dwArrow->h;
  JPB_PrintSurface (gfx->menuBg, &dest, &dest);
  gfx->upArrow->alpha = alphaUp;
  gfx->dwArrow->alpha = alphaDown;
  dest.x = gfx->mUpArrowX;
  dest.y = gfx->mUpArrowY;
  dest.w = gfx->upArrow->w;
  dest.h = gfx->upArrow->h;
  JPB_PrintSurface (gfx->upArrow, NULL, &dest);
  dest.x = gfx->mDwArrowX;
  dest.y = gfx->mDwArrowY;
  dest.w = gfx->dwArrow->w;
  dest.h = gfx->dwArrow->h;
  JPB_PrintSurface (gfx->dwArrow, NULL, &dest);
}

void
drawMenuT (data_t * gfx, menu_t * menu, int offset)
{
  int i;

  JPB_PrintSurface (gfx->menuBg, NULL, NULL);

  for (i = offset; i < offset + MIN (menu->nops, gfx->mMaxOps); i++)
    {
      drawMenuTOption (gfx, i, offset, &menu->opt[i], 0);
    }
}

void
drawMenuTOption (data_t * gfx, int opt, int offset, opt_t * option, int alpha)
{
  SDL_Rect rect;
  int capwidth;

  rect.x = gfx->menuX;
  rect.y = gfx->menuY + (opt - offset) * SFont_TextHeight (gfx->menufont);
  rect.w = gfx->menuW;
  rect.h = SFont_TextHeight (gfx->menufont);

  JPB_PrintSurface (gfx->menuBg, &rect, &rect);
  JPB_drawSquare (gfx->hlcolor, alpha, rect.x, rect.y, rect.w, rect.h);

  if (option->flags == MB_RETURN)
    SFont_WriteMaxWidth (gfx->menufont, rect.x + gfx->mMargin, rect.y,
			 rect.w - gfx->mMargin, gfx->mAlign, "...",
			 option->caption);
  else
    {
      SFont_WriteMaxWidth (gfx->menufont, rect.x + gfx->mMargin, rect.y,
			   rect.w - gfx->mMargin, ALEFT, "...",
			   option->caption);
      capwidth =
	SFont_TextWidth (gfx->menufont, option->caption) + gfx->mMargin;
      if ((option->flags & MB_INPUT) == MB_INPUT)
	SFont_WriteMaxWidth (gfx->menufont, rect.x + capwidth, rect.y,
			     rect.w - gfx->mMargin - capwidth, ARIGHT, "...",
			     *((char **) (option->data)));
      else if ((option->flags & MB_KEYDEF) == MB_KEYDEF)
	SFont_WriteMaxWidth (gfx->menufont, rect.x + capwidth, rect.y,
			     rect.w - gfx->mMargin - capwidth, ARIGHT, "...",
			     SDL_GetKeyName (*((SDLKey *) option->data)));
      else if ((option->flags & MB_CHOOSE) == MB_CHOOSE)
	SFont_WriteMaxWidth (gfx->menufont, rect.x + capwidth, rect.y,
			     rect.w - gfx->mMargin - capwidth, ARIGHT, "...",
			     option->opcap[*((int *) option->data)]);
    }
}

void
drawTip (data_t * gfx, char *tip)
{
  SFont_WriteAligned (gfx->tipfont, gfx->tipX, gfx->tipY, gfx->tipW, 0,
		      gfx->tAlign, tip);
}

char *
inputMenu (data_t * gfx, char *tip, char *inittext, int maxWidth)
{
  SDL_Event event;
  SDL_Rect rect;
  char ch = '\0';
  char text[MAX_CHAR];
  char *retText = NULL;
  int len;
  int prevUnic;

  drawMenu (gfx, 0, NULL);

  if (tip != NULL)
    drawTip (gfx, tip);

  sprintf (text, "%s", inittext);
  len = strlen (text);
  text[len] = '|';
  text[len + 1] = '\0';

  rect.x = gfx->menuX;
  rect.y = gfx->menuY;
  rect.w = gfx->menuW;
  rect.h = SFont_AlignedHeight (gfx->menufont, gfx->menuW, 0, text);

  //JPB_drawSquare(gfx->hlcolor, gfx->hlalpha, rect.x, rect.y, rect.w,rect.h);
  SFont_WriteAligned (gfx->menufont, rect.x + gfx->mMargin, rect.y,
		      gfx->menuW - gfx->mMargin, 0, ACENTER, text);
  FlipScreen ();

  prevUnic = SDL_EnableUNICODE (TRUE);
  while (ch != SDLK_RETURN)
    {
      while (SDL_PollEvent (&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      ch = event.key.keysym.unicode;

	      rect.h =
		SFont_AlignedHeight (gfx->menufont, gfx->menuW - gfx->mMargin,
				     0, text);
	      JPB_PrintSurface (gfx->menuBg, &rect, &rect);

	      if ((ch > 31) || (ch == '\b'))
		{
		  if ((ch == '\b') && (strlen (text) > 0))
		    {
		      len = strlen (text);
		      text[strlen (text) - 2] = '|';
		      text[strlen (text) - 1] = '\0';
		    }
		  else
		    {
		      len = strlen (text);
		      text[len - 1] = ch;
		      text[len] = '|';
		      text[len + 1] = '\0';
		    }
		}
	      if (strlen (text) > maxWidth)
		text[maxWidth] = '\0';

	      //JPB_drawSquare(gfx->hlcolor, gfx->hlalpha, rect.x, rect.y, rect.w,rect.h);
	      SFont_WriteAligned (gfx->menufont, rect.x + gfx->mMargin,
				  rect.y, gfx->menuW - gfx->mMargin, 0,
				  ACENTER, text);
	      FlipScreen ();
	    }
	}
      SDL_WaitEvent (NULL);
    }
  SDL_EnableUNICODE (prevUnic);
  text[strlen (text) - 1] = '\0';
  if ((retText = malloc (sizeof (char) * (strlen (text) + 1))) == NULL)
    return NULL;
  strcpy (retText, text);

  return retText;
}

/*
 * OLD MENU SYSTEM. (Still usefull ) ===========================================
 */

/* These four functions are still used by the new menu system */

void
drawMenu (data_t * gfx, int nops, char **ops)
{
  int i;

  JPB_PrintSurface (gfx->menuBg, NULL, NULL);

  for (i = 0; i < nops; i++)
    {
      drawOption (gfx, i, ops[i], 0);
    }
}

void
drawOption (data_t * gfx, int opt, char *option, int alpha)
{
  SDL_Rect rect;

  rect.x = gfx->menuX;
  rect.y = gfx->menuY + opt * SFont_TextHeight (gfx->menufont);
  rect.w = gfx->menuW;
  rect.h = SFont_TextHeight (gfx->menufont);

  JPB_PrintSurface (gfx->menuBg, &rect, &rect);
  JPB_drawSquare (gfx->hlcolor, alpha, rect.x, rect.y, rect.w, rect.h);

  //SFont_Write(gfx->menufont, x, y, option);
  SFont_WriteMaxWidth (gfx->menufont, rect.x + gfx->mMargin, rect.y,
		       rect.w - gfx->mMargin, gfx->mAlign, "...", option);

}

int
checkMenuKeys (mouse_t * mouse)
{
  SDL_Event event;
  int ret = KIDLE;

  while (SDL_PollEvent (&event))
    {
      switch (event.type)
	{
	  /* A key is pressed */
	case SDL_KEYDOWN:
	  switch (event.key.keysym.sym)
	    {
	    case SDLK_RETURN:
	      ret = KENTER;
	      break;
	    case SDLK_ESCAPE:
	      ret = KBACK;
	      break;
	    case SDLK_UP:
	      ret = KUP;
	      break;
	    case SDLK_DOWN:
	      ret = KDOWN;
	      break;
	    case SDLK_LEFT:
	      ret = KLEFT;
	      break;
	    case SDLK_RIGHT:
	      ret = KRIGHT;
	      break;
	    default:
	      break;
	    }
	  break;
	  /* A key UP. */
	case SDL_MOUSEBUTTONDOWN:
	  switch (event.button.button)
	    {
	    case SDL_BUTTON_LEFT:
	      ret = KMENTER;
	      break;
	    case SDL_BUTTON_RIGHT:
	      ret = KBACK;
	      break;
	    case SDL_BUTTON_WHEELDOWN:
	      ret = KMUP;
	      break;
	    case SDL_BUTTON_WHEELUP:
	      ret = KMDOWN;
	      break;
	    default:
	      break;
	    }
	  break;
	case SDL_MOUSEBUTTONUP:
	  mouse->clicked--;
	  break;
	case SDL_KEYUP:
	  break;
	  /* Quit: */
	case SDL_QUIT:
	  break;
	  /* Default */
	default:
	  break;
	}
    }

  return ret;
}


/*
 * This has been converted to a wrapper to the new menu system so I don't have to
 * debug and add features to two pieces of almost identical code.
 */

/*
 * Usage:
 *  The first value must be the number of options passed. If, additionally, you
 *  want an explanatory text appearing on top of the menu, turn this value to
 *  negative.
 *      Examples: playMenu (3, "option1", "option2", "option3");
 *                playMenu (-2, "option1", "explanation1", "option2", "explanation2");
 */

int
playMenu (data_t * gfx, int nops, ...)
{
  va_list p;
  char **options = NULL;
  char **tips = NULL;
  int i;
  int select = 0;

  va_start (p, nops);

  options = malloc (sizeof (char *) * abs (nops));
  if (nops < 0)
    tips = malloc (sizeof (char *) * abs (nops));

  for (i = 0; i < abs (nops); i++)
    {
      options[i] = va_arg (p, char *);
      if (nops < 0)
	tips[i] = va_arg (p, char *);
    }
  va_end (p);

  select = playMenuTab (gfx, nops, options, tips);
  free (options);
  free (tips);
  return select;
}

int
playMenuTab (data_t * gfx, int nops, char **options, char **tips)
{
  menu_t menu;
  int ret;
  int i;

  initMenuT (&menu);

  for (i = 0; i < abs (nops); i++)
    {
      if (nops < 0)
	addMenuTOption (&menu, options[i], tips[i], 0, NULL, NONE);
      else
	addMenuTOption (&menu, options[i], NULL, 0, NULL, NONE);
    }

  ret = playMenuT (gfx, &menu);

  freeMenuT (&menu);
  return ret;
}
