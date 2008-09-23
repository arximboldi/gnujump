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
#include "effects.h"

extern L_gblOptions gblOps;

int
updateHero (game_t * game, data_t * gfx, int num, float ms)
{
  int acc = 0;
  int st = 0;
  int floor;
  hero_t *hero = &game->heros[num];
  float fact = ms / DELAY;

  st = updateHeroPosition (game, num, fact);

  /* If we are too fast we should keep on rolling */
  if (hero->y < MINSKY)
    {
      if (gblOps.scrollMode == HARDSCROLL)
	{
	  hardScrollUp (game);
	}
      else
	{
	  softScrollUp (game, MINSKY - hero->y);
	}

      game->T_count = 0;
    }
  else if (hero->y >= GRIDHEIGHT * BLOCKSIZE)
    {
      return DEAD;
    }

  if (hero->st == 0 && st > 0)
    playHeroSound (gfx, S_FALL, &(game->replay));

  /* If the player is standing in the floor... */
  if (st > 0)
    {
      acc = 3;
      floor = (game->floorTop - hero->y / BLOCKSIZE) / FLOORGAP + 1;
      hero->rotateto = 0;

      if (gblOps.scrollMode == HARDSCROLL)
	hero->y = st * BLOCKSIZE;
      else
	hero->y = st * BLOCKSIZE + game->scrollCount + 1;

      hero->vy = 0;
      if (hero->left.pressed || hero->right.pressed)
	hero->id = H_WALK;
      else
	hero->id = H_STAND;

      if (hero->up.pressed)
	{
	  hero->jump = abs (hero->vx) / 4 + 7;
	  hero->vy = -hero->jump / 2 - 12;
	  hero->rotateto = 90 + 180 * hero->dir;
	  st = 0;
	  playHeroSound (gfx, S_JUMP, &(game->replay));
	}
    }
  else
    {
      acc = 2;
      floor = 0;
      hero->id = H_JUMP;
    }

  /* If we are floating in the air */
  if (st == 0)
    {
      if (hero->jump > 0)
	{
	  hero->vy = -hero->jump / 2 - 12;
	  if (hero->up.pressed)
	    hero->jump -= fact;
	  else
	    hero->jump = 0;
	}
      else
	{
	  hero->vy += 2 * fact;
	  if (hero->vy > 0)
	    hero->rotateto = 180;
	  if (hero->vy > BLOCKSIZE)
	    hero->vy = BLOCKSIZE;
	}
    }

  /* Set actions */
  if (hero->left.pressed)
    {
      hero->vx -= acc * fact;
      if (hero->vx < -32)
	hero->vx = -32;
      hero->dir = LEFT;
    }
  else if (hero->right.pressed)
    {
      hero->vx += acc * fact;
      if (hero->vx > 32)
	hero->vx = 32;
      hero->dir = RIGHT;
    }
  else if (st > 0)
    {
      if (hero->vx < -2)
	hero->vx += 3 * fact;
      else if (hero->vx > 2)
	hero->vx -= 3 * fact;
      else
	hero->vx = 0;
    }

  rotateHero (hero, ms);

  hero->st = st;
  if (hero->id != hero->previd)
    {
      hero->previd = hero->id;
      hero->sprite[hero->id].elpTime = hero->sprite[hero->id].frame = 0;
    }

  animateSpriteRot (&hero->sprite[hero->id], ms);

  if (floor != 0)
    {
      hero->prevFloor = hero->floor;
      hero->floor = floor;
    }

  if (gblOps.trailMode != NOTRAIL)
    updateTrails (hero, ms);
  if (gblOps.useGL && gblOps.blur)
    updateBlurs (hero, ms);

  return TRUE;
}

/* Updates player position and returns wether it is standing or not */
int
updateHeroPosition (game_t * game, int num, float fact)
{
  float Dy, diff, Dycount;
  int st = 0;
  hero_t *hero = &game->heros[num];

  hero->x += hero->vx / 2 * fact;

  /* The blue bricks block your way */
  if (hero->x < BLOCKSIZE)
    {
      hero->x = BLOCKSIZE;
      hero->vx = -hero->vx / 2;
    }
  else if (hero->x > GRIDWIDTH * BLOCKSIZE - 3 * BLOCKSIZE)
    {
      hero->x = GRIDWIDTH * BLOCKSIZE - 3 * BLOCKSIZE;
      hero->vx = -hero->vx / 2;
    }

  Dy = Dycount = hero->vy * fact;

  /* We have to take into account that, at low FPS rates, we might be
     going too fast and missing floors */
  if (Dy >= BLOCKSIZE)
    {
      while (Dycount > 0)
	{
	  if ((st = isStand (game, hero->x, hero->y)))
	    {
	      return st;
	    }
	  if (Dycount > BLOCKSIZE - 1)
	    diff = BLOCKSIZE - 1;
	  else
	    diff = Dycount;
	  Dycount -= diff;
	  hero->y += diff;
	}
      return st;
    }
  else
    {
      hero->y += Dy;
      if (hero->vy < 0)
	return FALSE;
      else
	return isStand (game, hero->x, hero->y);
    }

}

/* Returns whether a hero being at (ix,iy) would be standing or not and the
   Y position of that floor */
int
isStand (game_t * game, int ix, int iy)
{
  int y;

  if (iy + HEROSIZE > (GRIDHEIGHT - 1) * BLOCKSIZE)
    return FALSE;

  y = ((iy + HEROSIZE - game->scrollCount) / BLOCKSIZE);
  if (y >= GRIDHEIGHT)
    return FALSE;

  if (game->floor_l[(y + game->mapIndex) % GRIDHEIGHT] * BLOCKSIZE < (ix + 24)
      && (ix - 8) <
      game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] * BLOCKSIZE)
    return y - 2;
  else
    return FALSE;
}

void
rotateHero (hero_t * hero, float ms)
{
  float fact = ms / DELAY;
  if (gblOps.rotMode == ROTORIG)
    {
      hero->angle = hero->rotateto;
    }
  else if (gblOps.rotMode == ROTFULL)
    {
      if (hero->rotateto == 0)
	hero->angle = 0;
      else
	{
	  hero->angspeed = abs (hero->vx) * bool2sign (hero->dir)
	    + abs (hero->vy) * bool2sign (hero->dir);
	  hero->angle += hero->angspeed * fact;
	}
    }
}

void
reliveHero (game_t * game, int num)
{
  int i, y;
  hero_t *hero = &game->heros[num];

  for (i = (game->mapIndex + MINSKYGRID + 1) % GRIDHEIGHT, y = MINSKYGRID - 1;
       game->floor_r[i] < 0; i = (i + 1) % GRIDHEIGHT, y++);

  hero->y = y * BLOCKSIZE;
  hero->x =
    (game->floor_l[i] +
     (game->floor_r[i] - game->floor_l[i]) / 2) * BLOCKSIZE;
  hero->id = H_STAND;
  hero->vx = 0;
  hero->vy = -2;
  hero->prevLives = hero->lives;
}

int
isFloor (game_t * game, int x, int y)
{
  y -= game->scrollCount;
  if ((game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] > x) &&
      (game->floor_l[(y + game->mapIndex) % GRIDHEIGHT] < x))
    {
      return TRUE;
    }
  else
    {
      return FALSE;
    }
}

void
makeFloor (game_t * game, int y)
{
  int f, x1, x2, fvec;
  static int fpos = GRIDWIDTH / 2;

  f = game->floorTop - y;

  if (f % 250 == 0)
    {				/* If Floor % 50 = 0, you can relax */
      game->floor_l[(y + game->mapIndex) % GRIDHEIGHT] = 1;
      game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] = GRIDWIDTH - 2;
      return;
    }
  if (f % 5 == 0)
    {				/* Each five blocks, we create a floor */

      fvec =
	(rnd (2) * 2 - 1) * (rnd ((GRIDWIDTH - 11) / 4) +
			     (GRIDWIDTH - 11) / 4);
      fpos = (fpos + fvec + GRIDWIDTH - 11) % (GRIDWIDTH - 11);

      x1 = (fpos + 5 - rnd (3) - 2);
      x2 = (fpos + 5 + rnd (3) + 2);

      game->floor_l[(y + game->mapIndex) % GRIDHEIGHT] = x1;
      game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] = x2;
    }
  else
    {
      game->floor_l[(y + game->mapIndex) % GRIDHEIGHT] = GRIDWIDTH;
      game->floor_r[(y + game->mapIndex) % GRIDHEIGHT] = -1;
    }
}

void
hardScrollUp (game_t * game)
{
  scrollGrid (game);
  scrollHeros (game, BLOCKSIZE);
}

void
softScrollUp (game_t * game, float scroll)
{
  game->scrollCount += scroll;

  while (game->scrollCount >= BLOCKSIZE)
    {
      game->scrollCount -= BLOCKSIZE;
      scrollGrid (game);
    }

  scrollHeros (game, scroll);
}

void
scrollGrid (game_t * game)
{
  game->floorTop++;
  if (--game->mapIndex < 0)
    game->mapIndex = GRIDHEIGHT - 1;

  makeFloor (game, 0);

  if (gblOps.recReplay)
    game->replay.scrolls++;
}

void
scrollHeros (game_t * game, float scroll)
{
  int i;

  game->scrollTotal += scroll;

  for (i = 0; i < game->numHeros; i++)
    {
      game->heros[i].y += scroll;
      scrollTrails (&(game->heros[i]), scroll);
      scrollBlurs (&(game->heros[i]), scroll);
    }
}
