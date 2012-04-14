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
#include "tools.h"
#include "records.h"
#include "replay.h"
#include "menu.h"
#include "effects.h"

extern L_gblOptions gblOps;
extern SDL_Surface *screen;

void
freeGame (game_t * game)
{
  int i;

  if (gblOps.recReplay)
    freeReplay (&(game->replay));
  for (i = 0; i < game->numHeros; i++)
    {
      freeTrails (&(game->heros[i]));
      freeBlurs (&(game->heros[i]));
    }
  free (game->heros);
}

void
initGame (game_t * game, data_t * gfx, int numHeros)
{
  int i, j;

  /* Initialize the floors appearing on the screen */
  game->floorTop = GRIDHEIGHT - 4;
  game->mapIndex = 0;
  game->scrollCount = 0;
  game->scrollTotal = 0;
  for (i = 0; i < GRIDHEIGHT; i++)
    {
      makeFloor (game, i);
    }
  game->numHeros = numHeros;
  game->heros = malloc (sizeof (hero_t) * numHeros);
  game->T_count = 0;
  game->T_speed = 200;
  game->deadHeros = 0;

  for (i = 0; i < numHeros; i++)
    {
      for (j = 0; j < HEROANIMS; j++)
	{
	  initializeSpriteCtlRot (&game->heros[i].sprite[j],
				  gfx->heroSprite[i][j]);
	}

      initHeroKeys (&game->heros[i], i);
      game->heros[i].x =
	GRIDWIDTH * BLOCKSIZE / 2 - BLOCKSIZE + isOdd (i) * (i + 1) * 20;
      game->heros[i].y = (GRIDHEIGHT - 4) * BLOCKSIZE - HEROSIZE;
      game->heros[i].vx = game->heros[i].vy = 0;
      game->heros[i].dir = 0;
      game->heros[i].jump = 0;
      game->heros[i].floor = 0;
      game->heros[i].id = game->heros[i].previd = H_STAND;
      game->heros[i].dead = FALSE;
      game->heros[i].prevLives = -1;
      game->heros[i].trail = NULL;
      game->heros[i].blur = NULL;
      if (numHeros > 1)
	game->heros[i].lives = gblOps.mpLives - 1;
      else
	game->heros[i].lives = 0;
      game->heros[i].angle = game->heros[i].rotateto = 0;
    }

  if (gblOps.recReplay)
    initReplay (game);
}

/*
 * Returns wether we want to play again or not.
 */
int
playGame (data_t * gfx, int numHeros)
{
  L_timer timer;
  game_t game;
  int done = FALSE;
  int r;

  Mix_PlayMusic (gfx->musgame, -1);

  drawBg (gfx->gameBg, 0, 0, gblOps.w, gblOps.h);
  initGame (&game, gfx, numHeros);
  initTimer (&timer, getFps ());
  FlipScreen ();

  updateTimer (&timer);
  while (!done)
    {
      if ((r = updateInput (&game)))
	{
	  if (r == PAUSED)
	    {
	      done = pauseGame (gfx, &game, _("PAUSE"));
	    }
	  else
	    {
	      done =
		yesNoQuestion (gfx, &game,
			       _("Are you sure you want to end this match? (Y/n)"));
	    }
	  continueTimer (&timer);
	}
      
      if ((SDL_GetAppState () & SDL_APPINPUTFOCUS) != SDL_APPINPUTFOCUS)
	{
	  done = pauseGame (gfx, &game, _("PAUSE"));
	  continueTimer (&timer);
	}
      
      updateTimer (&timer);
      if (updateGame (&game, gfx, timer.ms))
	done = ENDMATCH;
      updateScore (gfx, &game, timer.totalms);
      if (gblOps.recReplay)
	updateReplay (&game, timer.ms);
      FlipScreen ();		/* Apply changes to the screen */
    }
  
  if (done == ENDMATCH)
    {
      if (gblOps.recReplay)
	endReplay (&game, timer.totalms);
      r = endMatch (gfx, &game, timer.totalms / 1000);
    }
  else
    {
      r = FALSE;
    }

  freeGame (&game);

  return r;
}

int
updateGame (game_t * game, data_t * gfx, float ms)
{
  int i;
  int done = TRUE;

  float fact = ms / DELAY;

  if (game->T_speed < 5000)
    game->T_speed += fact;
  else if (game->T_speed < 10000)
    game->T_speed += fact / 4;
  else if (game->T_speed < 15000)
    game->T_speed += fact / 8;

  game->T_count += game->T_speed * fact;

  if (gblOps.scrollMode == HARDSCROLL)
    {
      if (game->T_count > 20000)
	{
	  game->T_count -= 20000;
	  hardScrollUp (game);
	}
    }
  else
    {
      softScrollUp (game, 1 / (20000 / game->T_speed / BLOCKSIZE) * fact);
    }

  for (i = 0; i < game->numHeros; i++)
    {
      if (game->heros[i].dead == FALSE)
	{
	  if (updateHero (game, gfx, i, ms) == DEAD)
	    {
	      playHeroSound (gfx, S_DIE, &(game->replay));
	      if (game->heros[i].lives < 1)
		{
		  game->heros[i].dead = TRUE;
		  game->deadHeros++;
		}
	      else
		{
		  reliveHero (game, i);
		}
	      if (game->deadHeros == game->numHeros)
		{
		  done = TRUE;
		}
	      game->heros[i].lives--;
	    }
	  done = FALSE;
	}
    }

  drawGame (gfx, game);

  return done;
}

int
pauseGame (data_t * gfx, game_t * game, char *text)
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
  SFont_WriteAligned (gfx->textfont, x + BLOCKSIZE, y + BLOCKSIZE / 2,
		      w - 2 * BLOCKSIZE, 0, ACENTER, text);
  FlipScreen ();

  while (!done)
    {
      SDL_WaitEvent (NULL);
      while (SDL_PollEvent (&event))
	{
	  if (event.type == SDL_KEYDOWN)
	    {
	      if (event.key.keysym.sym == KEY_QUIT)
		{
		  ret = TRUE;
		  done = TRUE;
		}
	      else if (event.key.keysym.sym == SDLK_p
		       || event.key.keysym.sym == SDLK_PAUSE)
		{
		  ret = FALSE;
		  done = TRUE;
		}
	      else
		{
		  ret = FALSE;
		  done = TRUE;
		  SDL_PushEvent (&event);
		  break;
		}
	    }
	}
    }

  if (ret)
    {
      return yesNoQuestion (gfx, game,
			    _
			    ("Do you really want to quit this match? (Y/n)"));
    }

  return ret;
}

int
endMatch (data_t * gfx, game_t * game, int time)
{
  int i;
  int r, bstrec = 0, fact, newrec = FALSE;
  records_t rec;

  if (game->numHeros > 1)
    fact = gblOps.mpLives;
  else
    fact = 1;

  for (i = 0; i < game->numHeros; i++)
    {
      if ((r =
	   checkRecord (gblOps.records, game->heros[i].floor / fact, time)))
	{
	  makeRecord (&rec, gblOps.pname[i], game->heros[i].floor / fact,
		      time);
	  addRecord (gblOps.records, &rec, r);
	  bstrec = MIN (r, bstrec);
	  if (bstrec == 0)
	    bstrec = r;

	  newrec = TRUE;
	}
    }
  if (gblOps.recReplay)
    {
      if (yesNoQuestion
	  (gfx, game,
	   _("Do you want to view or save the replay of this match?")))
	{
	  saveReplayMenu (gfx, &(game->replay));
	  drawBg (gfx->gameBg, 0, 0, gblOps.w, gblOps.h);
	  drawScore (gfx, game, time);
	}
      drawGame (gfx, game);
    }
  if (newrec)
    {
      Mix_PlayChannel (-1, gfx->grecord, 0);
      if (yesNoQuestion
	  (gfx, game,
	   _ ("Congratulations! You have made a new local highscore. "
	      "Do you want to play again? (Y/n)")))
	{
	  return TRUE;
	}
      drawGame (gfx, game);
      drawRecords (gfx, gblOps.records, bstrec - 1);
      pressAnyKey ();
      return FALSE;
    }
  else
    {
      return yesNoQuestion (gfx, game,
			    _("Game Over. Do you want to play again? (Y/n)"));
    }
}


/* ==========================================================================
   Tools
   ========================================================================== */

