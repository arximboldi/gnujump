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
#include "effects.h"
#include "tools.h"

extern L_gblOptions gblOps;
extern SDL_Surface *screen;

void
playHeroSound (data_t * gfx, int sound, replay_t * rep)
{
  if (sound == S_JUMP && gfx->gjump)
    Mix_PlayChannel (-1, gfx->gjump, 0);
  if (sound == S_FALL && gfx->gfall)
    Mix_PlayChannel (-1, gfx->gfall, 0);
  if (sound == S_DIE && gfx->gdie)
    Mix_PlayChannel (-1, gfx->gdie, 0);
  if (gblOps.recReplay)
    rep->sounds |= sound;
}

void
drawHero (data_t * gfx, hero_t * hero)
{
  SDL_Rect dest;

  /* In rotating surface, X and Y refer to the centre of the image */
  dest.x =
    hero->x + gfx->gameX - (getFrameRot (&hero->sprite[hero->id], 0)->w -
			    HEROSIZE) / 2 +
    getFrameRot (&hero->sprite[hero->id], 0)->w / 2;
  dest.y =
    hero->y + gfx->gameY - (getFrameRot (&hero->sprite[hero->id], 0)->h -
			    HEROSIZE) + getFrameRot (&hero->sprite[hero->id],
						     0)->h / 2;

  printSpriteRot (&hero->sprite[hero->id], NULL, &dest, hero->dir,
		  hero->angle);
}

void
drawScrolledBgPart (data_t * gfx, int x, int y, int w, int h, int th, int sc)
{
  SDL_Rect src, dest;

  sc = th - sc;
  src.x = x;
  src.y = y + sc;
  src.w = w;
  src.h = th - sc;
  dest.x = x;
  dest.y = y;
  dest.w = w;
  dest.h = th - sc;
  JPB_PrintSurface (gfx->gameBg, &src, &dest);

  dest.y += dest.h;
  src.y = y;
  dest.h = src.h = th;

  for (; dest.y < y + h - th; dest.y += dest.h)
    {
      //printf("TH:%d Y:%d SC: %d\n", th, dest.y, sc);
      JPB_PrintSurface (gfx->gameBg, &src, &dest);
    }

  src.h = dest.h = y + h - dest.y;
  JPB_PrintSurface (gfx->gameBg, &src, &dest);

}

void
drawGame (data_t * gfx, game_t * game)
{
  int i, y, x, width;

  //printf("%d\n", gblOps.scrollBg);
  if (gblOps.scrollBg && gfx->gameTileH)
    {
      drawScrolledBgPart (gfx, gfx->gameX + BLOCKSIZE, gfx->gameY,
			  BLOCKSIZE * (GRIDWIDTH - 2), BLOCKSIZE * GRIDHEIGHT,
			  gfx->gameTileH,
			  (int) (game->scrollTotal / 2) % gfx->gameTileH);
      //printf("Total: %f\n", game->scrollTotal);
      if (gfx->borderTileH)
	{
	  drawScrolledBgPart (gfx, gfx->gameX + BLOCKSIZE - gfx->borderTileW,
			      gfx->gameY, gfx->borderTileW,
			      BLOCKSIZE * GRIDHEIGHT, gfx->borderTileH,
			      (int) (game->scrollTotal * 2) %
			      gfx->borderTileH);
	  drawScrolledBgPart (gfx, gfx->gameX + (GRIDWIDTH - 1) * BLOCKSIZE,
			      gfx->gameY, gfx->borderTileW,
			      BLOCKSIZE * GRIDHEIGHT, gfx->borderTileH,
			      (int) (game->scrollTotal * 2) %
			      gfx->borderTileH);
	}
      else
	{
	  drawBg (gfx->gameBg,
		  gfx->gameX, gfx->gameY,
		  BLOCKSIZE, BLOCKSIZE * GRIDHEIGHT);

	  drawBg (gfx->gameBg,
		  gfx->gameX + BLOCKSIZE * (GRIDWIDTH - 1), gfx->gameY,
		  BLOCKSIZE, BLOCKSIZE * GRIDHEIGHT);
	}
    }
  else
    {
      drawBg (gfx->gameBg, gfx->gameX, gfx->gameY, BLOCKSIZE * GRIDWIDTH,
	      BLOCKSIZE * GRIDHEIGHT);
    }

  for (y = game->floorTop % 5; y < GRIDHEIGHT; y += 5)
    {
      x = game->floor_l[(y + game->mapIndex) % GRIDHEIGHT];
      width = game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] - x + 1;
      if ((y * BLOCKSIZE + game->scrollCount) < (GRIDHEIGHT - 1) * BLOCKSIZE)
	drawFloor (gfx, x, y * BLOCKSIZE + game->scrollCount, width);
    }

  for (i = 0; i < game->numHeros; i++)
    {
      drawTrail (gfx, game->heros[i].trail, i);
      drawBlur (gfx, game->heros[i].blur, i);
    }

  for (i = 0; i < game->numHeros; i++)
    if (game->heros[i].y < BLOCKSIZE * GRIDHEIGHT - HEROSIZE)
      drawHero (gfx, &(game->heros[i]));
}

int
drawBg (JPB_surface * surf, int x, int y, int w, int h)
{
  SDL_Rect rect;
  rect.x = x;
  rect.h = h;
  rect.w = w;
  rect.y = y;
  JPB_PrintSurface (surf, &rect, &rect);
  return TRUE;
}

int
drawFloor (data_t * gfx, int x, int y, int bw)
{
  SDL_Rect rect;
  SDL_Rect src;
  int j;

  rect.x = x * BLOCKSIZE + gfx->gameX;
  rect.y = y + gfx->gameY;
  src.x = src.y = 0;

  src.w = gfx->floorL->w;
  src.h =
    MIN (gfx->floorL->h, (gfx->gameY + (GRIDHEIGHT) * BLOCKSIZE) - rect.y);
  JPB_PrintSurface (gfx->floorL, &src, &rect);

  for (j = 1; j < bw - 1; j++)
    {
      src.w = gfx->floorC->w;
      src.h =
	MIN (gfx->floorC->h,
	     (gfx->gameY + (GRIDHEIGHT) * BLOCKSIZE) - rect.y);
      rect.x = j * BLOCKSIZE + x * BLOCKSIZE + gfx->gameX;
      JPB_PrintSurface (gfx->floorC, &src, &rect);
    }
  src.w = gfx->floorR->w;
  src.h =
    MIN (gfx->floorR->h, (gfx->gameY + (GRIDHEIGHT) * BLOCKSIZE) - rect.y);
  rect.x =
    j * BLOCKSIZE + x * BLOCKSIZE + gfx->gameX + BLOCKSIZE - gfx->floorR->w;
  JPB_PrintSurface (gfx->floorR, &src, &rect);

  return TRUE;
}

void
drawAnimatedSquare (data_t * gfx, Uint32 color, Uint8 alpha, int x, int y,
		    int w, int h, int time)
{
  fader_t fader;
  L_timer timer;
  int xc = 0;
  int ret = 0;

  if (gfx->gquestion)
    Mix_PlayChannel (-1, gfx->gquestion, 0);

  x += w / 2;
  initTimer (&timer, getFps ());
  updateTimer (&timer);
  setFader (&fader, 1, w / 2 + 1, time, 0);

  do
    {
      updateTimer (&timer);
      ret = updateFader (&fader, timer.ms);
      JPB_drawSquare (color, alpha, x + xc, y, fader.value - xc, h);
      JPB_drawSquare (color, alpha, x - xc, y, xc - fader.value, h);
      xc = fader.value;
      FlipScreen ();
    }
  while (!ret);

}

void
drawScore (data_t * gfx, game_t * game, Uint32 currtime)
{
  int i, j;
  int x, y;
  SDL_Rect rect;
  char score[32];
  char time[32];

  x = gfx->timeX;
  y = gfx->timeY;
  sprintf (time, "%d", currtime);
  SFont_Write (gfx->timefont, x, y, time);

  for (i = 0; i < game->numHeros; i++)
    {
      sprintf (score, "%d", game->heros[i].floor);

      x = gfx->scoreX[i];
      y = gfx->scoreY[i];

      SFont_Write (gfx->scorefont, x, y, score);

      rect.x = x = gfx->livesX[i];
      rect.y = y = gfx->livesY[i];
      rect.w = gfx->livePic->w;
      rect.h = gfx->livePic->h;

      for (j = 0; j <= game->heros[i].lives; j++)
	{
	  JPB_PrintSurface (gfx->livePic, NULL, &rect);
	  if (gfx->liveAlign)
	    {
	      y += gfx->livePic->h;
	      rect.y = y;
	    }
	  else
	    {
	      x += gfx->livePic->w;
	      rect.x = x;
	    }
	}

      game->heros[i].prevLives = game->heros[i].lives;
    }
}

void
updateScore (data_t * gfx, game_t * game, Uint32 currtime)
{
  int i, j;
  int x, y;
  SDL_Rect rect;
  char score[32];
  char prevscore[32];
  char time[32];
  char prevtime[32];
  static int lastsec = -1;

  if ((currtime /= 1000) != lastsec)
    {
      sprintf (time, "%d", currtime);
      sprintf (prevtime, "%d", lastsec);
      lastsec = currtime;
      x = gfx->timeX;
      y = gfx->timeY;

      drawBg (gfx->gameBg, x - 1, y - 1,
	      SFont_TextWidth (gfx->timefont, prevtime) + 2,
	      SFont_TextHeight (gfx->timefont) + 2);

      SFont_Write (gfx->timefont, x, y, time);
    }

  for (i = 0; i < game->numHeros; i++)
    {
      if (game->heros[i].floor != game->heros[i].prevFloor)
	{
	  sprintf (score, "%d", game->heros[i].floor);
	  sprintf (prevscore, "%d", game->heros[i].prevFloor);

	  x = gfx->scoreX[i];
	  y = gfx->scoreY[i];

	  drawBg (gfx->gameBg, x - 1, y - 1,
		  SFont_TextWidth (gfx->scorefont, prevscore) + 2,
		  SFont_TextHeight (gfx->scorefont) + 2);

	  SFont_Write (gfx->scorefont, x, y, score);
	}
      if (game->heros[i].prevLives != game->heros[i].lives)
	{
	  rect.x = x = gfx->livesX[i];
	  rect.y = y = gfx->livesY[i];
	  rect.w = gfx->livePic->w;
	  rect.h = gfx->livePic->h;

	  if (gfx->liveAlign)
	    {
	      drawBg (gfx->gameBg, x, y,
		      gfx->livePic->w,
		      gfx->livePic->h * (game->heros[i].lives + 2));
	    }
	  else
	    {
	      drawBg (gfx->gameBg, x, y,
		      gfx->livePic->w * (game->heros[i].lives + 2),
		      gfx->livePic->h);
	    }

	  for (j = 0; j <= game->heros[i].lives; j++)
	    {
	      JPB_PrintSurface (gfx->livePic, NULL, &rect);
	      if (gfx->liveAlign)
		{
		  y += gfx->livePic->h;
		  rect.y = y;
		}
	      else
		{
		  x += gfx->livePic->w;
		  rect.x = x;
		}
	    }

	  game->heros[i].prevLives = game->heros[i].lives;
	}
    }
}
