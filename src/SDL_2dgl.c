/* "SDL_2dgl.c" [Implementation]
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
 * Special thanx to Nehe tutorials and Supertux for their help on learning Opengl,
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


#include "SDL_2dgl.h"

#include <stdlib.h>
#include <math.h>

/* A nice tool for texture blitting*/
static int
power_of_two (int input)
{
  int value = 1;

  while (value < input)
    {
      value <<= 1;
    }
  return value;
}

void
GL2D_DrawLine (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x0, int y0, int x1,
	       int y1)
{
  glEnable (GL_BLEND);
  glBegin (GL_LINES);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f (x0, y0);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f (x1, y1);
  glEnd ();
  glDisable (GL_BLEND);
}

void
GL2D_DrawRect (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x, int y, int w, int h)
{
  glEnable (GL_BLEND);
  glBegin (GL_QUADS);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f (x, y);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f ((float) w + x, y);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f ((float) w + x, (float) h + y);
  glColor4f ((float) r / 255, (float) g / 255, (float) b / 255,
	     (float) a / 255);
  glVertex2f (x, (float) h + y);
  glEnd ();
  glDisable (GL_BLEND);
}

void
GL2D_InitScreenGL (SDL_Surface * scr, int w, int h, int bpp,
		   Uint8 use_fullscreen)
{
  /* Sets GL attributes */
  SDL_GL_SetAttribute (SDL_GL_RED_SIZE, 5);
  SDL_GL_SetAttribute (SDL_GL_GREEN_SIZE, 5);
  SDL_GL_SetAttribute (SDL_GL_BLUE_SIZE, 5);
  SDL_GL_SetAttribute (SDL_GL_DEPTH_SIZE, 16);
  SDL_GL_SetAttribute (SDL_GL_DOUBLEBUFFER, 0);

  /* Starts the screen */
  if (use_fullscreen)
    {
      scr = SDL_SetVideoMode (w, h, bpp, SDL_FULLSCREEN | SDL_OPENGL);
      if (scr == NULL)
	{
	  fprintf (stderr,
		   "\nWarning: I could not set up fullscreen video for "
		   "%ix%i mode.\n"
		   "The Simple DirectMedia error that occured was:\n"
		   "%s\n\n", w, h, SDL_GetError ());
	  use_fullscreen = 0;
	}
    }
  else
    {
      scr = SDL_SetVideoMode (w, h, bpp, SDL_OPENGL);
      if (scr == NULL)
	{
	  fprintf (stderr,
		   "\nError: I could not set up video for %ix%i mode.\n"
		   "The Simple DirectMedia error that occured was:\n"
		   "%s\n\n", w, h, SDL_GetError ());
	  exit (1);
	}
    }

  /* Sets the OpenGL context for 2D rendering */
  glDisable (GL_DEPTH_TEST);
  glDisable (GL_CULL_FACE);

  glViewport (0, 0, scr->w, scr->h);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity ();
  glOrtho (0.0f, scr->w, scr->h, 0.0f, -1.0f, 1.0f);
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity ();
  glClearColor (0.0f, 0.0f, 0.0f, 0.0f);
  glClear (GL_COLOR_BUFFER_BIT);

  return;
}

void
GL2D_BlitSurfaceGL (GL2D_SurfaceGL * src, SDL_Rect * src_r, SDL_Rect * dest_r,
		    Uint8 alpha)
{
  float x, y;

  if (dest_r == NULL)
    x = y = 0;
  else
    {
      x = dest_r->x;
      y = dest_r->y;
    }

  if (src_r == NULL)
    GL2D_BlitGL (src, x, y, alpha);
  else
    GL2D_BlitGLpart (src, (float) src_r->x, (float) src_r->y, (float) x,
		     (float) y, (float) src_r->w, (float) src_r->h, alpha);
}

void
GL2D_BlitGL (GL2D_SurfaceGL * src, float x, float y, Uint8 alpha)
{
  float pw = power_of_two (src->w);
  float ph = power_of_two (src->h);

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4ub (alpha, alpha, alpha, alpha);

  glBindTexture (GL_TEXTURE_2D, src->tex);

  glBegin (GL_QUADS);

  glTexCoord2f (0, 0);
  glVertex2f (x, y);

  glTexCoord2f ((float) src->w / pw, 0);
  glVertex2f (src->w + x, y);

  glTexCoord2f ((float) src->w / pw, (float) src->h / ph);
  glVertex2f (src->w + x, src->h + y);

  glTexCoord2f (0, (float) src->h / ph);
  glVertex2f (x, src->h + y);
  glEnd ();

  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
}

void
GL2D_BlitGLpart (GL2D_SurfaceGL * src,
		 float sx, float sy, float x, float y, float w, float h,
		 Uint8 alpha)
{
  float pw = power_of_two ((src->w));
  float ph = power_of_two ((src->h));

  glBindTexture (GL_TEXTURE_2D, src->tex);

  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4ub (alpha, alpha, alpha, alpha);

  glEnable (GL_TEXTURE_2D);

  glBegin (GL_QUADS);

  glTexCoord2f (sx / pw, sy / ph);
  glVertex2f (x, y);

  glTexCoord2f ((float) (sx + w) / pw, sy / ph);
  glVertex2f (w + x, y);

  glTexCoord2f ((sx + w) / pw, (sy + h) / ph);
  glVertex2f (w + x, h + y);

  glTexCoord2f (sx / pw, (float) (sy + h) / ph);
  glVertex2f (x, h + y);

  glEnd ();

  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);
}

void
GL2D_BlitGLrot (GL2D_SurfaceGL * src, float x, float y, float angle,
		Uint8 alpha)
{

  float pw = power_of_two (src->w);
  float ph = power_of_two (src->h);

  glLoadIdentity ();

  glEnable (GL_TEXTURE_2D);
  glEnable (GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glColor4ub (alpha, alpha, alpha, alpha);

  glBindTexture (GL_TEXTURE_2D, src->tex);

  glTranslatef (x, y, 0.0f);
  glRotatef (-angle, 0.0f, 0.0f, 1.0f);

  glBegin (GL_QUADS);

  glTexCoord2f (0, 0);
  glVertex2f (-(src->w / 2), -(src->h / 2));

  glTexCoord2f (src->w / pw, 0);
  glVertex2f (src->w - (src->w / 2), -(src->h / 2));

  glTexCoord2f (src->w / pw, src->h / ph);
  glVertex2f (src->w - (src->w / 2), src->h - (src->h / 2));

  glTexCoord2f (0, src->h / ph);
  glVertex2f (-(src->w / 2), src->h - (src->h / 2));

  glEnd ();

  glDisable (GL_TEXTURE_2D);
  glDisable (GL_BLEND);

  glLoadIdentity ();
}

void
GL2D_CreateGL (SDL_Surface * surf, GLuint * tex, GLint filter)
{

  Uint32 saved_flags;
  Uint8 saved_alpha;
  Uint32 w, h;
  SDL_Surface *conv = NULL;

  w = power_of_two (surf->w);
  h = power_of_two (surf->h),
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
    conv =
    SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, surf->format->BitsPerPixel,
			  0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
    conv =
    SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, surf->format->BitsPerPixel,
			  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  /* Save the alpha blending attributes */
  saved_flags = surf->flags;	//&(SDL_SRCALPHA|SDL_RLEACCELOK);
  saved_alpha = surf->format->alpha;
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
    {
      SDL_SetAlpha (surf, 0, 0);
    }

  SDL_BlitSurface (surf, 0, conv, 0);

  /* Restore the alpha blending attributes */
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
    {
      SDL_SetAlpha (surf, saved_flags, saved_alpha);
    }

  glGenTextures (1, &*tex);
  glBindTexture (GL_TEXTURE_2D, *tex);

  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
  glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

  glPixelStorei (GL_UNPACK_ROW_LENGTH,
		 conv->pitch / conv->format->BytesPerPixel);
  glTexImage2D (GL_TEXTURE_2D, 0, 4, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		conv->pixels);
  glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);

  SDL_FreeSurface (conv);
}

void
GL2D_FreeSurfaceGL (GL2D_SurfaceGL * src)
{
  glDeleteTextures (1, &src->tex);
  free (src);
}

void
GL2D_FreeLargeSurfaceGL (GL2D_LargeSurfaceGL * src)
{
  int i, j;
  for (i = 0; i < src->hParts; i++)
    {
      for (j = 0; j < src->vParts; j++)
	{
	  GL2D_FreeSurfaceGL (src->pics[i][j]);
	}
      free (src->pics[i]);
    }
  free (src->pics);
  free (src);
}

GL2D_SurfaceGL *
GL2D_CreateSurfaceGL (SDL_Surface * surf, GLint filter)
{
  GL2D_SurfaceGL *dest = NULL;

  dest = malloc (sizeof (GL2D_SurfaceGL));

  if (dest != NULL)
    {
      GL2D_CreateGL (surf, &dest->tex, filter);
      dest->h = surf->h;
      dest->w = surf->w;
    }

  return dest;
}

static SDL_Surface *
CropSurface (SDL_Surface * surf, SDL_Rect * rect)
{
  Uint32 saved_flags;
  Uint8 saved_alpha;
  Uint32 w, h;
  SDL_Surface *conv = NULL;

  w = rect->w;
  h = rect->h;

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  conv =
    SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, surf->format->BitsPerPixel,
			  0xff000000, 0x00ff0000, 0x0000ff00, 0x000000ff);
#else
  conv =
    SDL_CreateRGBSurface (SDL_SWSURFACE, w, h, surf->format->BitsPerPixel,
			  0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
#endif

  // Save the alpha blending attributes
  saved_flags = surf->flags;	//&(SDL_SRCALPHA|SDL_RLEACCELOK);
  saved_alpha = surf->format->alpha;
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
    {
      SDL_SetAlpha (surf, 0, 0);
    }

  SDL_BlitSurface (surf, rect, conv, NULL);

  /* Restore the alpha blending attributes */
  if ((saved_flags & SDL_SRCALPHA) == SDL_SRCALPHA)
    {
      SDL_SetAlpha (surf, saved_flags, saved_alpha);
    }

  return conv;
}

GL2D_LargeSurfaceGL *
GL2D_CreateLargeSurfaceGL (SDL_Surface * surf, GLint filter)
{
  GL2D_LargeSurfaceGL *dest = NULL;
  SDL_Surface *temp = NULL;
  SDL_Rect rect;
  int i, j;

  dest = malloc (sizeof (GL2D_LargeSurfaceGL));

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, &dest->partSize);

  //dest->partSize = 1024;
  //printf("\nThe maximun texture size is: %i\n",dest->partSize);

  dest->w = surf->w;
  dest->h = surf->h;
  dest->hParts = floor (surf->w / dest->partSize) + 1;
  dest->vParts = floor (surf->h / dest->partSize) + 1;

  dest->pics = calloc (dest->hParts, sizeof (GL2D_SurfaceGL **));
  for (i = 0; i < dest->hParts; i++)
    dest->pics[i] = calloc (dest->vParts, sizeof (GL2D_SurfaceGL *));

  for (j = 0; j < dest->vParts; j++)
    {
      for (i = 0; i < dest->hParts; i++)
	{
	  if (i < (dest->hParts - 1))
	    rect.w = dest->partSize;
	  else
	    rect.w = dest->w - (dest->partSize * (dest->hParts - 1));

	  if (j < (dest->vParts - 1))
	    rect.h = dest->partSize;
	  else
	    rect.h = dest->h - (dest->partSize * (dest->vParts - 1));

	  rect.x = dest->partSize * i;
	  rect.y = dest->partSize * j;

	  temp = CropSurface (surf, &rect);
	  dest->pics[i][j] = GL2D_CreateSurfaceGL (temp, filter);
	  SDL_FreeSurface (temp);
	}
    }

  return dest;

}

void
GL2D_BlitLargeGL (GL2D_LargeSurfaceGL * src, float x, float y, Uint8 alpha)
{
  int i, j;

  for (j = 0; j < src->vParts; j++)
    {
      for (i = 0; i < src->hParts; i++)
	{
	  GL2D_BlitGL (src->pics[i][j], x + i * src->partSize,
		       y + j * src->partSize, alpha);
	}
    }
}

void
GL2D_BlitLargeGLpart (GL2D_LargeSurfaceGL * src,
		      float sx, float sy, float x, float y, float w, float h,
		      Uint8 alpha)
{

  int i, j;
  int j0, i0;			//The first part we should blit.
  int jn, in;			//The last part we'll blit.
  //The coordinates of the piece of a part we should print.
  int sx2, sy2, w2, h2;
  //The amount of pixel we have already printed.
  int xCount = 0;
  int yCount = 0;

  j0 = floor (sy / src->partSize);
  jn = floor ((sy + h) / src->partSize);

  i0 = floor (sx / src->partSize);
  in = floor ((sx + w) / src->partSize);

  for (j = j0; j <= jn; j++)
    {
      if (j == j0)
	sy2 = sy - j * src->partSize;
      else
	sy2 = 0;

      if (j == jn)
	{
	  if (j == j0)
	    h2 = h;
	  else
	    h2 = (sy + h) - (j * src->partSize);
	}
      else
	{
	  h2 = src->partSize - sy2;
	}

      xCount = 0;
      for (i = i0; i <= in; i++)
	{
	  // if we are drawing the first or last pic, we should calc what part
	  // of the pic must be drawn.
	  if (i == i0)
	    sx2 = sx - (i * src->partSize);
	  else
	    sx2 = 0;

	  if (i == in)
	    {
	      if (i == i0)
		w2 = w;
	      else
		w2 = (sx + w) - (i * src->partSize);
	    }
	  else
	    {
	      w2 = src->partSize - sx2;
	    }
	  GL2D_BlitGLpart (src->pics[i][j], sx2, sy2, x + xCount, y + yCount,
			   w2, h2, alpha);
	  xCount += w2;
	}
      yCount += h2;
    }
}

void
GL2D_BlitLargeSurfaceGL (GL2D_LargeSurfaceGL * src, SDL_Rect * src_r,
			 SDL_Rect * dest_r, Uint8 alpha)
{
  float x, y;

  if (dest_r == NULL)
    x = y = 0;
  else
    {
      x = dest_r->x;
      y = dest_r->y;
    }

  if (src_r == NULL)
    GL2D_BlitLargeGL (src, x, y, alpha);
  else
    GL2D_BlitLargeGLpart (src, (float) src_r->x, (float) src_r->y, (float) x,
			  (float) y, (float) src_r->w, (float) src_r->h,
			  alpha);

}
