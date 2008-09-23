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
#include "effects.h"
#include "surface.h"
#include "sprite.h"
#include "tools.h"

extern L_gblOptions gblOps;

blur_t *
makeBlur (float x, float y, int angle, JPB_surfaceRot * pic)
{
  blur_t *blur = malloc (sizeof (trail_t));

  blur->next = NULL;
  blur->alpha = BLURSTART;
  blur->x = x;
  blur->y = y;
  blur->angle = angle;
  blur->pic = pic;

  return blur;
}

void
updateBlurs (hero_t * hero, float msec)
{
  blur_t *blur;
  int falpha;
  int delta;

  delta = (float) BLURSTART / BLURFADE * msec;
  blur = hero->blur;

  while (blur != NULL && blur->next != NULL)
    {
      blur->alpha -= delta;
      if (blur->alpha <= 0)
	{
	  hero->blur = blur->next;
	  free (blur);
	  blur = hero->blur;
	}
      else
	blur = blur->next;
    }
  if (blur != NULL)
    {
      blur->alpha -= delta;
      if (blur->alpha <= 0)
	{
	  hero->blur = blur->next;
	  free (blur);
	  blur = hero->blur;
	}
    }
  if (blur != NULL)
    falpha = blur->alpha;
  else
    falpha = 0;

  if ((falpha > BLURGAP) || (hero->vx == 0 && hero->vy == 0))
    return;

  if (blur == NULL)
    hero->blur =
      makeBlur (hero->x, hero->y, hero->angle,
		getFrameRot (&hero->sprite[hero->id], hero->dir));
  else
    blur->next =
      makeBlur (hero->x, hero->y, hero->angle,
		getFrameRot (&hero->sprite[hero->id], hero->dir));
}

void
scrollBlurs (hero_t * hero, float dy)
{
  blur_t *blur;

  blur = hero->blur;
  while (blur != NULL)
    {
      blur->y += dy;
      blur = blur->next;
    }
}

void
drawBlur (data_t * gfx, blur_t * blur, int player)
{
  SDL_Rect dest;
  int alpha;
  /* In rotating surface, X and Y refer to the centre of the image */
  while (blur != NULL)
    {
      if (blur->y < BLOCKSIZE * (GRIDHEIGHT) - HEROSIZE)
	{
	  alpha = blur->pic->alpha;
	  blur->pic->alpha = blur->alpha * (float) gblOps.blur / MAXBLUR;
	  dest.x = blur->x + gfx->gameX - (blur->pic->w - HEROSIZE) / 2
	    + blur->pic->w / 2;
	  dest.y = blur->y + gfx->gameY - (blur->pic->h - HEROSIZE)
	    + blur->pic->h / 2;
	  JPB_PrintSurfaceRot (blur->pic, NULL, &dest, blur->angle);
	  blur->pic->alpha = alpha;
	}
      blur = blur->next;
    }
}

void
freeBlurs (hero_t * hero)
{
  blur_t *blur = hero->blur;
  blur_t *last;
  while (blur != NULL)
    {
      last = blur;
      blur = blur->next;
      free (last);
    }
}
