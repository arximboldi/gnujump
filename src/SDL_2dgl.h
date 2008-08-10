/* "SDL_2dgl.h" [Declarations]
 *  _  _        _
 * |_ | \|      _| _| _ |
 *  _||_/|_ __ |_ |_||_||_      (c) 2004 Juan Pedro Bolivar Puente
 *                    _|
 *
 * This project started on saturday 30 october 2004 at 11:11 A.M. in Huelva (Spain).
 *
 * This small library includes some tools to use OpenGL to render 2d games easily.
 * This project started as part of the LuciferINO project.
 *
 */

/*
    Copyright (C) 2004, 2008, Juan Pedro Bolivar Puente

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef SDL_2DGL_H
#define SDL_2DGL_H

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

/*
 * This is a GL texture thar can be used like if it were an SDL_Surface with this library.
 * Remember that this mustn't be declared as a pointer.
 */
typedef struct GL2D_SurfaceGL
{
  GLuint tex;
  Uint16 w;
  Uint16 h;
}
GL2D_SurfaceGL;

/*
 * The same, but used for pictures larger than the maximum texture size.
 */
typedef struct GL2D_LargeSurfaceGL
{
  GL2D_SurfaceGL ***pics;
  GLint partSize;
  Uint8 hParts, vParts;
  Uint16 w;
  Uint16 h;
}
GL2D_LargeSurfaceGL;

/*
 * Draws a line.
 */
void GL2D_DrawLine (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x0, int y0,
		    int x1, int y1);

/*
 * Frees a large surface.
 */
void GL2D_FreeLargeSurfaceGL (GL2D_LargeSurfaceGL * src);

/*
 * "Blits" a large surface in the screen.
 */
void GL2D_BlitLargeGL (GL2D_LargeSurfaceGL * src, float x, float y,
		       Uint8 alpha);

/*
 * Loads a large surface.
 */
GL2D_LargeSurfaceGL *GL2D_CreateLargeSurfaceGL (SDL_Surface * surf,
						GLint filter);

/*
 * Large surface blitting but using SDL_Rect's
 */
void GL2D_BlitLargeSurfaceGL (GL2D_LargeSurfaceGL * src, SDL_Rect * src_r,
			      SDL_Rect * dest_r, Uint8 alpha);

/*
 * Blits part of a large surface.
 */
void GL2D_BlitLargeGLpart (GL2D_LargeSurfaceGL * src,
			   float sx, float sy, float x, float y, float w,
			   float h, Uint8 alpha);

/*
 * Frees a GL2D_SurfaceGL
 */
void GL2D_FreeSurfaceGL (GL2D_SurfaceGL * src);

/*
 * Initializes the screen to work with 2D OpenGL rendering.
 */
void GL2D_InitScreenGL (SDL_Surface * screen, int w, int h, int bpp,
			Uint8 use_fullscreen);

/*
 * Creates a GL texture from a SDL_Surface
 */
void GL2D_CreateGL (SDL_Surface * surf, GLuint * tex, GLint filter);

/*
 * Creates a SDL_SurfaceGL from a SDL_Surface
 */
GL2D_SurfaceGL *GL2D_CreateSurfaceGL (SDL_Surface * surf, GLint filter);

/*
 * "Blits" a surface using SDL_Rects as references. Specially usefull when working
 * with both software and OpenGL rendering.
 */
void GL2D_BlitSurfaceGL (GL2D_SurfaceGL * src, SDL_Rect * src_r,
			 SDL_Rect * dest_r, Uint8 alpha);

/*
 * "Blits" a SDL_SurfaceGL rotated. X and Y refers to the position where the center
 * of the pic will be.
 */
void GL2D_BlitGLrot (GL2D_SurfaceGL * src, float x, float y, float angle,
		     Uint8 alpha);

/*
 * "Blits" part of a surface.
 */
void GL2D_BlitGLpart (GL2D_SurfaceGL * src,
		      float sx, float sy, float x, float y, float w, float h,
		      Uint8 alpha);

/*
 * "Blits" a whole surface.
 */
void GL2D_BlitGL (GL2D_SurfaceGL * src, float x, float y, Uint8 alpha);

/*
 * Draws a square of the specified r-g-b-a color
 */
void GL2D_DrawRect (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x, int y, int w,
		    int h);


#endif /* SDL_2DGL_H */
