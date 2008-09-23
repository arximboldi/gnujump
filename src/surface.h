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

#ifndef SURFACE_H
#define SURFACE_H

#include "SDL_2dgl.h"

#define LARGE_GL -1
#define TRUE 1
#define FALSE 0

/* MY SURFACE TYPE */
typedef struct surface
{
  SDL_Surface *surf;
  GL2D_SurfaceGL *surfGL;
  GL2D_LargeSurfaceGL *LsurfGL;
  Uint8 alpha;
  Uint16 w, h;
  Sint8 gl;
}
JPB_surface;

/* MY ROTABLE SURFACE TYPE */
/* NOTE: If using OpenGL, you can't use it for surfaces larger than the maximun
 * texture size of you graphics card. */
typedef struct rot_surface
{
  SDL_Surface *surf;
  SDL_Surface *rSurf;		//this is a rotated copy of the picture.
  GL2D_SurfaceGL *surfGL;
  Uint8 alpha;
  Uint16 w, h;
  Uint8 gl;			//is this an openGL surface?
  float angle;
}
JPB_surfaceRot;

void JPB_drawSquare (Uint32 color, Uint8 alpha, int x, int y, int w, int h);

void drawLine (SDL_Surface * dest, Uint8 dr, Uint8 dg, Uint8 db, Uint8 alpha,
	       int x0, int y0, int x1, int y1);

void JPB_drawLine (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x0, int y0, int x1,
		   int y1);


/********************
 *	ROTABLE SURFACE *
 ********************/

/*
 * JPB_LoadImgRot ()
 * Loads a rotable surface
 */
JPB_surfaceRot *JPB_LoadImgRot (char *file,
				Uint8 gl, Uint8 alpha, Uint8 trans,
				Uint8 rev);

/*
 * JPB_FreeSurfaceRot()
 * Frees a rotable surface.
 */
void JPB_FreeSurfaceRot (JPB_surfaceRot * surface);

/*
 * JPB_PrintSurfaceRot()
 * Prints a rotable surface on the screen. The x and y values of the destiny rectangle
 * define the where the centre of the pic will be. The picture is blitted rotated around its
 * centre "angle" degrees.
 */
void JPB_PrintSurfaceRot (JPB_surfaceRot * src, SDL_Rect * src_r,
			  SDL_Rect * dest_r, float angle);

/*
 * JPB_CreateSurfaceRot()
 * Creates a rotable surface from an SDL_Surface.
 */
JPB_surfaceRot *JPB_CreateSurfaceRot (SDL_Surface * src, Uint8 gl);

/*******************
 * SURFACE         *
 *******************/
JPB_surface *JPB_CreateSurface (SDL_Surface * src, Uint8 gl);

void JPB_FreeSurface (JPB_surface * surface);

JPB_surface *JPB_LoadImg (char *file,
			  Uint8 gl, Uint8 alpha, Uint8 trans, Uint8 rev);

void JPB_PrintSurface (JPB_surface * src, SDL_Rect * src_r,
		       SDL_Rect * dest_r);

/*
** BlitSurface()
** Blits a surface with alpha blending options...
*/
int BlitSurface (SDL_Surface * src, SDL_Rect * src_r,
		 SDL_Surface * dest, SDL_Rect * dest_r, Uint8 alpha);

/*
 * This blits a SDL_Surface with an angle of rotation...
 */
int BlitRot (SDL_Surface * src, SDL_Surface * dest, SDL_Rect * dest_r,
	     Sint16 angle, Uint8 alpha);


//tools.........................................................................

void drawSquareAlpha (SDL_Surface * dest, Uint32 color, Uint8 alpha,
		      int x, int y, int w, int h);

/*
** flipscreen()
** Swaps buffers.
*/
void FlipScreen ();

/*
** getpixel()
** Function from the SDL tutorial. Gets a pixel form a given surface.
*/
Uint32 getpixel (SDL_Surface * surface, int x, int y);

/*
** putpixel()
** Function from the SDL tutorial. Draws a pixel in a given surface.
*/
void putpixel (SDL_Surface * surface, int x, int y, Uint32 pixel);

/*
** Slock()
** Locks a surface.
*/
void Slock (SDL_Surface * door);

/*
** Sulock()
** Unlocks a surface.
*/
void Sulock (SDL_Surface * door);

/*
** LoadIMG()
** Loads a picture form a BMP image and converts it to the screen bps.
*/
SDL_Surface *LoadImg (char *file, Uint8 Use_Alpha, Uint8 trans);

/*
** SetTrans()
** Sets a color picked from a pixel in a surface as transparent in that surface.
*/
void SetTrans (SDL_Surface * src, int x, int y);

/*
** ReversePic()
** Horizontal mirror effect.
*/
SDL_Surface *ReversePic (SDL_Surface * source);

#endif
