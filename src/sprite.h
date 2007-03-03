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

#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "surface.h"


/*
** Determines a position relative to whatever matter.
*/
typedef struct position {
	int x;
	int y;
} L_position;

typedef struct sprite_data {
	JPB_surface **pic;
	float* time;
	Uint8 sides;
	int nFrames;
} L_spriteData;

typedef struct sprite_data_rot {
	JPB_surfaceRot **pic;
	float* time;
	Uint8 sides;
	int nFrames;
} L_spriteDataRot;

typedef struct sprite_control {
	L_spriteData* sdata;
	float elpTime;
	int frame;
} L_spriteCtl;

typedef struct sprite_control_rot {
	L_spriteDataRot* sdata;
	float elpTime;
	int frame;
} L_spriteCtlRot;

L_spriteDataRot* loadSpriteDataRot ( char* filename, int sides, char* path );
L_spriteData* loadSpriteData ( char* filename, int sides, char* path );
int animateSprite ( L_spriteCtl *sprite, float dt );
int animateSpriteRot ( L_spriteCtlRot *sprite, float dt );
void printSprite ( L_spriteCtl *sprite, SDL_Rect *src_r, SDL_Rect *dest_r, int side );
void printSpriteRot ( L_spriteCtlRot *sprite, SDL_Rect *src_r, SDL_Rect *dest_r, int side, float angle );
void freeSpriteData ( L_spriteData *sprite );
void freeSpriteDataRot ( L_spriteDataRot *sprite );
JPB_surfaceRot* getFrameRot ( L_spriteCtlRot *sprite, int side );
void initializeSpriteCtlRot ( L_spriteCtlRot *sprite, L_spriteDataRot *data );
void initializeSpriteCtl ( L_spriteCtl *sprite, L_spriteData *data );

#endif //_SPRITE_H_
