/*
 * GNUjump
 * =======
 *
 * Copyright (C) 2005-2006, Juan Pedro Bolivar Puente
 *
 * GNUjump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNUjump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNUjump; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "gnujump.h"
#include "effects.h"
#include "surface.h"
#include "sprite.h"
#include "tools.h"

extern L_gblOptions gblOps;

/* ==========================================================================
   Particles Rules
   ========================================================================== */

void addParticle(particle_t** list, JPB_surfaceRot* pic, float x, float y, int vx,
				int vy, int ax, int ay, int rv, int rot, int av, int alpha)
{
}

void updateParticles(particle_t** list)
{
}

void drawParticles(data_t* gfx, particle_t* list)
{
}

void freeParticles(particle_t** list)
{
}


/* ==========================================================================
   The player trails
   ========================================================================== */

trail_t* makeTrail(float x0, float y0, float x1, float y1)
{
	trail_t* trail = malloc(sizeof(trail_t));
	
	trail->next = NULL;
	trail->alpha = 255;
	trail->x0 = x0;
	trail->y0 = y0;
	trail->x1 = x1;
	trail->y1 = y1;
	
	return trail;
}

/* Returns the last trail in the list */
void updateTrails(hero_t* hero, float msec)
{
	trail_t* trail;
	int delta;
	
	delta = (float)TRAILSTART/TRAILFADE * msec;
	trail = hero->trail;
	while (trail != NULL && trail->next != NULL) {
		trail->alpha -= delta;
		if (trail->alpha <= 0) {
			hero->trail = trail->next;
			free(trail);
			trail = hero->trail;
		} else
			trail = trail->next;
	}
	if (trail != NULL) {
		trail->alpha -= delta;
		if (trail->alpha <= 0) {
			hero->trail = trail->next;
			free(trail);
			trail = hero->trail;
		}
	}
	
	if (trail == NULL)
		hero->trail = makeTrail(hero->x, hero->y, hero->x, hero->y);
	else
		trail->next = makeTrail(trail->x1, trail->y1, hero->x, hero->y);
}
 
void scrollTrails(hero_t* hero, float dy)
{
	trail_t* trail;

	trail = hero->trail;
	while (trail != NULL) {
		trail->y1 += dy;
		trail->y0 += dy;
		trail = trail->next;
	}
}

void drawTrail(data_t* gfx, trail_t* trail, int player)
{
	int x0, y0, x1, y1;
	while (trail != NULL) {
		if (trail->y1+1 < BLOCKSIZE*(GRIDHEIGHT-1) &&
			trail->y0+1 < BLOCKSIZE*(GRIDHEIGHT-1)) {	
			x0 = trail->x0 + gfx->gameX + HEROSIZE/2;
			y0 = trail->y0 + gfx->gameY + HEROSIZE/2;
			x1 = trail->x1 + gfx->gameX + HEROSIZE/2;
			y1 = trail->y1 + gfx->gameY + HEROSIZE/2;
			switch (gblOps.trailMode) {
			case THINTRAIL:
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0, y0, x1, y1);
				break;
			case NORMALTRAIL:
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0, y0, x1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha/2, x0-1, y0, x1-1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha/2, x0, y0+1, x1, y1+1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha/2, x0+1, y0, x1+1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha/2, x0, y0-1, x1, y1-1);
				break;
			case STRONGTRAIL:
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0, y0, x1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0-1, y0, x1-1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0, y0+1, x1, y1+1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha, x0+1, y0, x1+1, y1);
				JPB_drawLine(gfx->tcolorr[player], gfx->tcolorg[player],
					gfx->tcolorb[player], trail->alpha/2, x0, y0-1, x1, y1-1);
				break;
			default: break;
			}
		}
		
		trail = trail->next;
	}
}

void freeTrails(hero_t* hero)
{
	trail_t* trail = hero->trail;
	trail_t* last;
	while (trail != NULL) {
		last = trail;
		trail = trail->next;
 		free(last);
	}
}

/* ==========================================================================
   The player blur
   ========================================================================== */

blur_t* makeBlur(float x, float y, int angle, JPB_surfaceRot* pic)
{
	blur_t* blur = malloc(sizeof(trail_t));
	
	blur->next = NULL;
	blur->alpha = BLURSTART;
	blur->x = x;
	blur->y = y;
	blur->angle = angle;
	blur->pic = pic;
	
	return blur;
}

void updateBlurs(hero_t* hero, float msec)
{
	blur_t* blur;
	int falpha;
	int delta;
	
	delta = (float)BLURSTART/BLURFADE * msec;
	blur = hero->blur;

	while (blur != NULL && blur->next != NULL) {
		blur->alpha -= delta;
		if (blur->alpha <= 0) {
			hero->blur = blur->next;
			free(blur);
			blur = hero->blur;
		} else
			blur = blur->next;
	}
	if (blur != NULL) {
		blur->alpha -= delta;
		if (blur->alpha <= 0) {
			hero->blur = blur->next;
			free(blur);
			blur = hero->blur;
		}
	}
	if (blur != NULL) falpha = blur->alpha;
	else falpha = 0;
	
	if ((falpha > BLURGAP) || (hero->vx == 0 && hero->vy == 0)) return;
	
	if (blur == NULL)
		hero->blur = makeBlur(hero->x, hero->y, hero->angle, getFrameRot(&hero->sprite[hero->id],hero->dir));
	else
		blur->next = makeBlur(hero->x, hero->y, hero->angle, getFrameRot(&hero->sprite[hero->id],hero->dir));
}
 
void scrollBlurs(hero_t* hero, float dy)
{
	blur_t* blur;

	blur = hero->blur;
	while (blur != NULL) {
		blur->y += dy;
		blur = blur->next;
	}
}

void drawBlur(data_t* gfx, blur_t* blur, int player)
{
    SDL_Rect dest;
	int alpha;
    /* In rotating surface, X and Y refer to the centre of the image */
	while (blur != NULL) {
		if (blur->y < BLOCKSIZE*(GRIDHEIGHT)-HEROSIZE) {
			alpha = blur->pic->alpha;
			blur->pic->alpha = blur->alpha * (float)gblOps.blur/MAXBLUR;
			dest.x = blur->x+ gfx->gameX -(blur->pic->w - HEROSIZE)/2
				+ blur->pic->w/2;
			dest.y = blur->y+ gfx->gameY -(blur->pic->h - HEROSIZE)
				+ blur->pic->h/2;
			JPB_PrintSurfaceRot(blur->pic, NULL, &dest, blur->angle);
			blur->pic->alpha = alpha;
		}
		blur = blur->next;
	}
}

void freeBlurs(hero_t* hero)
{
	blur_t* blur = hero->blur;
	blur_t* last;
	while (blur != NULL) {
		last = blur;
		blur = blur->next;
		free(last);
	}
}

