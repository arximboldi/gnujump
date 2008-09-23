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

#include "gnujump.h"
#include "game.h"


int
yesNoQuestion (data_t * gfx, game_t * game, char *text)
{
  int w = (GRIDWIDTH - 2) * BLOCKSIZE;
  int h =
    BLOCKSIZE + SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE, 0,
				     text);
  int x = gfx->gameX + BLOCKSIZE;
  int y = gfx->gameY + (GRIDHEIGHT * BLOCKSIZE - h) / 2;
  int ret = FALSE;
  int done = FALSE;
  SDL_Event event;

  drawAnimatedSquare (gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);

  x += BLOCKSIZE;
  y += BLOCKSIZE / 2;
  w -= 2 * BLOCKSIZE;

  SFont_WriteAligned (gfx->textfont, x, y, w, 0, ACENTER, text);
  FlipScreen ();

  while (!done)
    {
      while (SDL_PollEvent (&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      if (event.key.keysym.sym == KEY_QUIT ||
		  event.key.keysym.sym == SDLK_n)
		{
		  ret = FALSE;
		  done = TRUE;
		}
	      else if (event.key.keysym.sym == SDLK_y
		       || event.key.keysym.sym == SDLK_RETURN)
		{
		  ret = TRUE;
		  done = TRUE;
		}
	    }
	}
      SDL_WaitEvent (NULL);
    }

  return ret;
}

void
drawRecords (data_t * gfx, records_t * rtab, int hl)
{
  int x, x1, x2, x3, x4, x5, x6, y, w, h, i;
  char buf[128];

  x = gfx->gameX + 2 * BLOCKSIZE;
  w = GRIDWIDTH * BLOCKSIZE - 4 * BLOCKSIZE;
  h = BLOCKSIZE + 12 * gfx->textfont->Surface->h
    + SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE, 0,
			   _
			   ("Press any key to continue. Read the README or the 'man' page to learn the meaning of the 'mode' column."));
  y = gfx->gameY + (GRIDHEIGHT * BLOCKSIZE - h) / 2;

  drawAnimatedSquare (gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);
  /*
     x1:#  x2:Name                       x3:Floor x4:Mode x5:Time x6: Date
     ---------------------------------------------------------------------
   */
  y += BLOCKSIZE;

  x1 = x + BLOCKSIZE;

  x2 = x1 + SFont_TextWidth (gfx->textfont, "#   ");
  sprintf (buf, "%s %s %s %s", _("Floor"), _("Mode"), _("Time"),
	   rtab[0].date);

  x3 = x1 + w - 2 * BLOCKSIZE - SFont_TextWidth (gfx->textfont, buf);
  sprintf (buf, "%s %s %s %s", _("Floor"), _("Mode"), _("Time"), _("Date"));
  SFont_Write (gfx->textfont, x3, y, buf);
  sprintf (buf, "%s ", _("Floor"));

  x4 = x3 + SFont_TextWidth (gfx->textfont, buf);
  sprintf (buf, "%s ", _("Mode"));

  x5 = x4 + SFont_TextWidth (gfx->textfont, buf);
  sprintf (buf, "%s ", _("Time"));

  x6 = x5 + SFont_TextWidth (gfx->textfont, buf);
  sprintf (buf, "#  %s ", _("Date"));
  SFont_Write (gfx->textfont, x1, y, buf);

  for (i = 0; i < MAX_RECORDS; i++)
    {
      y += gfx->textfont->Surface->h - 1;

      if (hl == i)
	{
	  sprintf (buf, ">");
	  SFont_Write (gfx->textfont,
		       x1 - SFont_TextWidth (gfx->textfont, buf), y, buf);
	}

      sprintf (buf, "%d", i + 1);
      SFont_Write (gfx->textfont, x1, y, buf);

      SFont_Write (gfx->textfont, x2, y, rtab[i].pname);
      SFont_FillWith (gfx->textfont,
		      x2 + SFont_TextWidth (gfx->textfont, rtab[i].pname) + 3,
		      y,
		      x3 - 6 - (x2 +
				SFont_TextWidth (gfx->textfont,
						 rtab[i].pname)), '.');

      sprintf (buf, "%d", rtab[i].floor);
      SFont_Write (gfx->textfont, x3, y, buf);

      SFont_Write (gfx->textfont, x4, y, rtab[i].mode);

      sprintf (buf, "%d", rtab[i].time);
      SFont_Write (gfx->textfont, x5, y, buf);

      sprintf (buf, "%s", rtab[i].date);
      SFont_Write (gfx->textfont, x6, y, buf);
    }
  y += gfx->textfont->Surface->h * 2;
  SFont_WriteAligned (gfx->textfont, x1, y, w - 2 * BLOCKSIZE,
		      0, ALEFT,
		      _
		      ("Press any key to continue. Read the README or the 'man' page to learn the meaning of the 'mode' column."));

  FlipScreen ();
}

void
drawCredits (data_t * gfx)
{
  int x, y, w, h;

  x = gfx->gameX + 2 * BLOCKSIZE;
  w = GRIDWIDTH * BLOCKSIZE - 4 * BLOCKSIZE;
  h = BLOCKSIZE + 10 * SFont_TextHeight (gfx->textfont);
  y = gfx->gameY + (GRIDHEIGHT * BLOCKSIZE - h) / 2;

  drawAnimatedSquare (gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);

  y += SFont_TextHeight (gfx->textfont);
  x += BLOCKSIZE;

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, _("SOURCE CODE AUTHOR"));
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0, _("SOURCE CODE AUTHOR"));

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, AUTHOR);
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0, AUTHOR) + SFont_TextHeight (gfx->textfont);

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, _("THIS GRAPHICS THEME AUTHOR"));
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0, _("THIS GRAPHICS THEME AUTHOR"));

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, gfx->gfxauth);
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0,
			    gfx->gfxauth) + SFont_TextHeight (gfx->textfont);

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, _("THIS SOUND THEME AUTHOR"));
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0, _("THIS SOUND THEME AUTHOR"));

  SFont_WriteAligned (gfx->textfont, x, y, w - 2 * BLOCKSIZE,
		      0, ACENTER, gfx->sndauth);
  y += SFont_AlignedHeight (gfx->textfont, w - 2 * BLOCKSIZE,
			    0,
			    gfx->sndauth) + SFont_TextHeight (gfx->textfont);

  /* No idea how to get the translation author with gettext */
  /*
     SFont_WriteAligned(gfx->textfont, x, y, w-2*BLOCKSIZE,
     0, ACENTER, _("THIS TRANSLATION AUTHOR"));
     y += SFont_AlignedHeight(gfx->textfont, w-2*BLOCKSIZE,
     0, _("THIS TRANSLATION AUTHOR"));

     SFont_WriteAligned(gfx->textfont, x, y, w-2*BLOCKSIZE,
     0, ACENTER, gfx->langauth);
     y += SFont_AlignedHeight(gfx->textfont, w-2*BLOCKSIZE,
     0, gfx->langauth) + SFont_TextHeight(gfx->textfont);
   */
  FlipScreen ();
}
