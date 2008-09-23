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

void
setFpsTimer (L_timer * time, int rate)
{
  time->ratechanged = TRUE;
  time->framecount = 0;
  time->rate = rate;
  time->rateticks = (1000.0 / (float) rate);
}

Uint32
updateTimer (L_timer * time)
{
  Uint32 current_ticks;
  Uint32 target_ticks;
  Uint32 the_delay;
  Uint32 currms;
  Uint32 ret = FALSE;

  if (time->rate > 0)
    {
      time->framecount++;

      currms = current_ticks = SDL_GetTicks ();
      time->ms = currms - time->mscount;
      time->mscount = currms;

      target_ticks = time->lastticks +
	(Uint32) ((float) time->framecount * time->rateticks);

      if (current_ticks <= target_ticks)
	{
	  the_delay = target_ticks - current_ticks;
	  SDL_Delay (the_delay);
	}
      else
	{
	  time->framecount = 0;
	  time->lastticks = SDL_GetTicks ();
	  //ret = TRUE;
	  if (!time->ratechanged)
	    ret = current_ticks - target_ticks;
	}
    }
  else
    {				/* Dont limit FPS */
      currms = SDL_GetTicks ();
      time->ms = currms - time->mscount;
      time->mscount = currms;
    }
  time->totalms += time->ms;
  time->ratechanged = FALSE;
  return ret;
}

void
continueTimer (L_timer * time)
{
  time->mscount = SDL_GetTicks ();
  time->framecount = 0;
  time->ratechanged = TRUE;
}

void
initTimer (L_timer * time, int rate)
{
  time->framecount = 0;

  time->ratechanged = FALSE;
  time->notdelayed = 0;
  time->rate = rate;
  time->rateticks = (1000.0 / (float) time->rate);
  time->mscount = time->lastticks = SDL_GetTicks ();
  time->totalms = time->ms = 0;
}
