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

trail_t *
makeTrail (float x0, float y0, float x1, float y1)
{
  trail_t *trail = malloc (sizeof (trail_t));

  trail->next = NULL;
  trail->alpha = 255;
  trail->x0 = x0;
  trail->y0 = y0;
  trail->x1 = x1;
  trail->y1 = y1;

  return trail;
}

void
updateTrails (hero_t * hero, float msec)
{
  trail_t *trail;
  int delta;

  delta = (float) TRAILSTART / TRAILFADE * msec;
  trail = hero->trail;
  while (trail != NULL && trail->next != NULL)
    {
      trail->alpha -= delta;
      if (trail->alpha <= 0)
	{
	  hero->trail = trail->next;
	  free (trail);
	  trail = hero->trail;
	}
      else
	trail = trail->next;
    }
  if (trail != NULL)
    {
      trail->alpha -= delta;
      if (trail->alpha <= 0)
	{
	  hero->trail = trail->next;
	  free (trail);
	  trail = hero->trail;
	}
    }

  if (trail == NULL)
    hero->trail = makeTrail (hero->x, hero->y, hero->x, hero->y);
  else
    trail->next = makeTrail (trail->x1, trail->y1, hero->x, hero->y);
}

void
scrollTrails (hero_t * hero, float dy)
{
  trail_t *trail;

  trail = hero->trail;
  while (trail != NULL)
    {
      trail->y1 += dy;
      trail->y0 += dy;
      trail = trail->next;
    }
}

void
drawTrail (data_t * gfx, trail_t * trail, int player)
{
  int x0, y0, x1, y1;
  while (trail != NULL)
    {
      if (trail->y1 + 1 < BLOCKSIZE * (GRIDHEIGHT - 1) &&
	  trail->y0 + 1 < BLOCKSIZE * (GRIDHEIGHT - 1))
	{
	  x0 = trail->x0 + gfx->gameX + HEROSIZE / 2;
	  y0 = trail->y0 + gfx->gameY + HEROSIZE / 2;
	  x1 = trail->x1 + gfx->gameX + HEROSIZE / 2;
	  y1 = trail->y1 + gfx->gameY + HEROSIZE / 2;
	  switch (gblOps.trailMode)
	    {
	    case THINTRAIL:
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0, y0, x1,
			    y1);
	      break;
	    case NORMALTRAIL:
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0, y0, x1,
			    y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha / 2, x0 - 1,
			    y0, x1 - 1, y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha / 2, x0,
			    y0 + 1, x1, y1 + 1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha / 2, x0 + 1,
			    y0, x1 + 1, y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha / 2, x0,
			    y0 - 1, x1, y1 - 1);
	      break;
	    case STRONGTRAIL:
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0, y0, x1,
			    y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0 - 1, y0,
			    x1 - 1, y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0, y0 + 1,
			    x1, y1 + 1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha, x0 + 1, y0,
			    x1 + 1, y1);
	      JPB_drawLine (gfx->tcolorr[player], gfx->tcolorg[player],
			    gfx->tcolorb[player], trail->alpha / 2, x0,
			    y0 - 1, x1, y1 - 1);
	      break;
	    default:
	      break;
	    }
	}

      trail = trail->next;
    }
}

void
freeTrails (hero_t * hero)
{
  trail_t *trail = hero->trail;
  trail_t *last;
  while (trail != NULL)
    {
      last = trail;
      trail = trail->next;
      free (last);
    }
}
