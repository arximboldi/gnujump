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


#ifndef _EFFECTS_H_
#define _EFFECTS_H_

/* Trail settings */
#define TRAILSTART 255
#define TRAILFADE 1000

/* Blur settings */
#define BLURSTART 255
#define BLURFADE 400
#define BLURGAP 242

#include "gnujump.h"
#include "game.h"

/*
  Blur
*/

void updateBlurs (hero_t * hero, float msec);

void scrollBlurs (hero_t * hero, float dy);

void drawBlur (data_t * gfx, blur_t * blur, int player);

void freeBlurs (hero_t * hero);

/*
  Trail
*/

trail_t *makeTrail (float x0, float y0, float x1, float y1);

void updateTrails (hero_t * hero, float msec);

void scrollTrails (hero_t * hero, float dy);

void drawTrail (data_t * gfx, trail_t * trail, int player);

void freeTrails (hero_t * hero);

#endif
