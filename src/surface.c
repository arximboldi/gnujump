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
#include "surface.h"

#include "SDL_rotozoom.h"

extern SDL_Surface *screen;
extern L_gblOptions gblOps;

#define applyAlpha(alpha, bc, c) ((alpha)*(bc) + (1-(alpha))*(c))

void
JPB_drawLine (Uint8 r, Uint8 g, Uint8 b, Uint8 a, int x0, int y0, int x1,
	      int y1)
{
  if (gblOps.useGL)
    {
      GL2D_DrawLine (r, g, b, a, x0, y0, x1, y1);
    }
  else
    {
      drawLine (screen, r, g, b, a, x0, y0, x1, y1);
    }
}

/* Interesting article on line drawing:
   http://www.cs.unc.edu/~mcmillan/comp136/Lecture6/Lines.html */
void
drawLine (SDL_Surface * dest, Uint8 dr, Uint8 dg, Uint8 db, Uint8 alpha,
	  int x0, int y0, int x1, int y1)
{
  int dy = y1 - y0;
  int dx = x1 - x0;
  int stepx, stepy;
  int fraction;
  Uint8 r, g, b;
  Uint32 pixel;
  float a = (float) alpha / 255;

  if (dy < 0)
    {
      dy = -dy;
      stepy = -1;
    }
  else
    {
      stepy = 1;
    }
  if (dx < 0)
    {
      dx = -dx;
      stepx = -1;
    }
  else
    {
      stepx = 1;
    }
  dy <<= 1;
  dx <<= 1;

  /* We dont draw the first pixel, because it would overlap with another
     trail segment */
  if (dx > dy)
    {
      fraction = dy - (dx >> 1);
      while (x0 != x1)
	{
	  if (fraction >= 0)
	    {
	      y0 += stepy;
	      fraction -= dx;
	    }
	  x0 += stepx;
	  fraction += dy;

	  /* TODO: We should create a function for this */
	  pixel = getpixel (dest, x0, y0);
	  SDL_GetRGB (pixel, dest->format, &r, &g, &b);
	  r = applyAlpha (a, dr, r);
	  g = applyAlpha (a, dg, g);
	  b = applyAlpha (a, db, b);
	  pixel = SDL_MapRGB (dest->format, r, g, b);
	  putpixel (dest, x0, y0, pixel);
	}
    }
  else
    {
      fraction = dx - (dy >> 1);
      while (y0 != y1)
	{
	  if (fraction >= 0)
	    {
	      x0 += stepx;
	      fraction -= dy;
	    }
	  y0 += stepy;
	  fraction += dx;

	  pixel = getpixel (dest, x0, y0);
	  SDL_GetRGB (pixel, dest->format, &r, &g, &b);
	  r = applyAlpha (a, dr, r);
	  g = applyAlpha (a, dg, g);
	  b = applyAlpha (a, db, b);
	  pixel = SDL_MapRGB (dest->format, r, g, b);
	  putpixel (dest, x0, y0, pixel);
	}
    }
}

void
JPB_drawSquare (Uint32 color, Uint8 alpha, int x, int y, int w, int h)
{
  Uint8 r, g, b;
  if (gblOps.useGL)
    {
      SDL_GetRGB (color, screen->format, &r, &g, &b);
      GL2D_DrawRect (r, g, b, alpha, x, y, w, h);
    }
  else
    {
      drawSquareAlpha (screen, color, alpha, x, y, w, h);
    }
}

JPB_surfaceRot *
JPB_LoadImgRot (char *file, Uint8 gl, Uint8 alpha, Uint8 trans, Uint8 rev)
{
  SDL_Surface *temp1 = NULL, *temp2 = NULL;
  JPB_surfaceRot *surface = NULL;

  temp1 = LoadImg (file, alpha, trans);
  if (temp1 != NULL)
    {
      if (rev)
	{
	  temp2 = temp1;
	  temp1 = ReversePic (temp1);
	  SDL_FreeSurface (temp2);
	}
      surface = JPB_CreateSurfaceRot (temp1, gl);
      SDL_FreeSurface (temp1);
    }

  return surface;
}

JPB_surfaceRot *
JPB_CreateSurfaceRot (SDL_Surface * src, Uint8 gl)
{
  JPB_surfaceRot *surface = NULL;

  surface = malloc (sizeof (JPB_surfaceRot));

  if (gl)
    {
      surface->gl = TRUE;
      surface->surfGL = GL2D_CreateSurfaceGL (src, gblOps.texFilter);
      surface->w = surface->surfGL->w;
      surface->h = surface->surfGL->h;
    }
  else
    {
      if ((src->flags & SDL_SRCALPHA) == SDL_SRCALPHA)
	surface->surf = SDL_DisplayFormatAlpha (src);
      else
	surface->surf = SDL_DisplayFormat (src);
      if (gblOps.texFilter == GL_NEAREST)
	surface->rSurf = rotozoomSurface (surface->surf, 0, 1, 0);
      else
	surface->rSurf = rotozoomSurface (surface->surf, 0, 1, 1);

      surface->w = surface->surf->w;
      surface->h = surface->surf->h;
      surface->gl = FALSE;
    }
  surface->alpha = -1;

  return surface;
}


JPB_surface *
JPB_LoadImg (char *file, Uint8 gl, Uint8 alpha, Uint8 trans, Uint8 rev)
{
  SDL_Surface *temp1 = NULL, *temp2 = NULL;
  JPB_surface *surface = NULL;

  temp1 = LoadImg (file, alpha, trans);
  if (temp1 != NULL)
    {
      if (rev)
	{
	  temp2 = temp1;
	  temp1 = ReversePic (temp1);
	  SDL_FreeSurface (temp2);
	}
      surface = JPB_CreateSurface (temp1, gl);
      SDL_FreeSurface (temp1);
    }

  return surface;
}

JPB_surface *
JPB_CreateSurface (SDL_Surface * src, Uint8 gl)
{
  JPB_surface *surface = NULL;
  GLint texSize;

  surface = malloc (sizeof (JPB_surface));

  if (gl)
    {
      glGetIntegerv (GL_MAX_TEXTURE_SIZE, &texSize);
      //texSize = 1024;
      if (texSize < src->w)
	{
	  surface->gl = LARGE_GL;
	  surface->LsurfGL =
	    GL2D_CreateLargeSurfaceGL (src, gblOps.texFilter);
	  surface->w = surface->LsurfGL->w;
	  surface->h = surface->LsurfGL->h;
	}
      else
	{
	  surface->gl = TRUE;
	  surface->surfGL = GL2D_CreateSurfaceGL (src, gblOps.texFilter);
	  surface->w = surface->surfGL->w;
	  surface->h = surface->surfGL->h;
	}
    }
  else
    {
      if ((src->flags & SDL_SRCALPHA) == SDL_SRCALPHA)
	surface->surf = SDL_DisplayFormatAlpha (src);
      else
	surface->surf = SDL_DisplayFormat (src);

      surface->w = surface->surf->w;
      surface->h = surface->surf->h;
      surface->gl = FALSE;
    }
  surface->alpha = -1;

  return surface;
}

void
JPB_FreeSurface (JPB_surface * surface)
{
  if (surface->gl == LARGE_GL)
    GL2D_FreeLargeSurfaceGL (surface->LsurfGL);
  else if (surface->gl == TRUE)
    GL2D_FreeSurfaceGL (surface->surfGL);
  else if (surface->gl == FALSE)
    SDL_FreeSurface (surface->surf);

  free (surface);
}

void
JPB_FreeSurfaceRot (JPB_surfaceRot * surface)
{
  if (surface->gl == TRUE)
    GL2D_FreeSurfaceGL (surface->surfGL);
  else if (surface->gl == FALSE)
    {
      SDL_FreeSurface (surface->surf);
      SDL_FreeSurface (surface->rSurf);
    }
  free (surface);
}

void
JPB_PrintSurfaceRot (JPB_surfaceRot * src, SDL_Rect * src_r,
		     SDL_Rect * dest_r, float angle)
{

  if (src->gl == TRUE)
    {
      GL2D_BlitGLrot (src->surfGL, dest_r->x, dest_r->y, angle, src->alpha);
      src->angle = angle;
    }
  else if (src->gl == FALSE)
    {
      if (angle != src->angle)
	{
	  SDL_FreeSurface (src->rSurf);
	  if (gblOps.texFilter == GL_NEAREST)
	    src->rSurf = rotozoomSurface (src->surf, angle, 1, 0);
	  else
	    src->rSurf = rotozoomSurface (src->surf, angle, 1, 1);
	  src->angle = angle;
	}
      dest_r->x -= src->rSurf->w / 2;
      dest_r->y -= src->rSurf->h / 2;
      BlitSurface (src->rSurf, src_r, screen, dest_r, src->alpha);
    }
}

void
JPB_PrintSurface (JPB_surface * src, SDL_Rect * src_r, SDL_Rect * dest_r)
{

  if (src->gl == TRUE)
    GL2D_BlitSurfaceGL (src->surfGL, src_r, dest_r, src->alpha);
  else if (src->gl == FALSE)
    BlitSurface (src->surf, src_r, screen, dest_r, src->alpha);
  else if (src->gl == LARGE_GL)
    GL2D_BlitLargeSurfaceGL (src->LsurfGL, src_r, dest_r, src->alpha);

}



/* Probably not very fast, but enough for my needs*/
void
drawSquareAlpha (SDL_Surface * dest, Uint32 color, Uint8 alpha,
		 int x, int y, int w, int h)
{
  int i, j;
  Uint32 pixel;
  Uint8 r, g, b, dr, dg, db;
  float alphaF = (float) alpha / 255;

  if (x + w > dest->w)
    w = dest->w - x;
  if (y + h > dest->h)
    h = dest->h - y;

  /* Negative width/height values do reverse drawing */
  if (w < 0)
    {
      w = -w;
      x -= w;
    }
  if (h < 0)
    {
      h = -h;
      y -= h;
    }

  SDL_GetRGB (color, dest->format, &dr, &dg, &db);
  Slock (dest);
  for (i = 0; i < w; i++)
    {
      for (j = 0; j < h; j++)
	{
	  pixel = getpixel (dest, i + x, j + y);
	  SDL_GetRGB (pixel, dest->format, &r, &g, &b);
	  r = applyAlpha (alphaF, dr, r);
	  g = applyAlpha (alphaF, dg, g);
	  b = applyAlpha (alphaF, db, b);
	  pixel = SDL_MapRGB (dest->format, r, g, b);
	  putpixel (dest, i + x, j + y, pixel);
	}
    }
  Sulock (dest);
}

int
BlitSurface (SDL_Surface * src, SDL_Rect * src_r,
	     SDL_Surface * dest, SDL_Rect * dest_r, Uint8 alpha)
{
  //SDL_Surface * sdl_surface_copy = NULL;
  int ret;

  if (alpha == 0)
    return FALSE;

  if (alpha != 255)
    {
      /*if ((src->flags & SDL_SRCALPHA)==SDL_SRCALPHA)
         {
         sdl_surface_copy = SDL_CreateRGBSurface (src->flags,
         src->w, src->h, src->format->BitsPerPixel,
         src->format->Rmask, src->format->Gmask,
         src->format->Bmask,
         0);

         //colorkey = SDL_MapRGB(sdl_surface_copy->format, 255, 0, 255);

         //SDL_FillRect(sdl_surface_copy, NULL, colorkey);
         //SDL_SetColorKey(sdl_surface_copy, SDL_SRCCOLORKEY, colorkey);

         SDL_BlitSurface(src, NULL, sdl_surface_copy, NULL);

         SDL_SetAlpha(sdl_surface_copy, SDL_SRCALPHA, alpha);

         ret = SDL_BlitSurface(sdl_surface_copy, src_r, dest, dest_r);

         SDL_FreeSurface (sdl_surface_copy);
         return ret;
         return FALSE;
         }else
         { */
      SDL_SetAlpha (src, SDL_SRCALPHA, alpha);
      ret = SDL_BlitSurface (src, src_r, dest, dest_r);
      SDL_SetAlpha (src, 0, alpha);
      return ret;
      //}
    }
  else
    {
      ret = SDL_BlitSurface (src, src_r, dest, dest_r);
      return ret;
    }
}

int
BlitRot (SDL_Surface * src, SDL_Surface * dest, SDL_Rect * dest_r,
	 Sint16 angle, Uint8 alpha)
{
  SDL_Surface *rot = NULL;
  int ret;

  rot = rotozoomSurface (src, angle, 1, 0);
  ret = BlitSurface (rot, NULL, dest, dest_r, alpha);
  SDL_FreeSurface (rot);
  return ret;
}

void
FlipScreen ()
{
  if (gblOps.useGL)
    {
      SDL_GL_SwapBuffers ();
#ifdef GLFINISH
      glFinish ();
#endif
      //glClear(GL_COLOR_BUFFER_BIT);
    }
  else
    SDL_Flip (screen);
}

Uint32
getpixel (SDL_Surface * surface, int x, int y)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to retrieve */
  Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

  if (x >= surface->w || y >= surface->h || x < 0 || y < 0)
    return 0;

  switch (bpp)
    {
    case 1:
      return *p;

    case 2:
      return *(Uint16 *) p;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
      return *(Uint32 *) p;

    default:
      return 0;			/* shouldn't happen, but avoids warnings */
    }
}


void
putpixel (SDL_Surface * surface, int x, int y, Uint32 pixel)
{
  int bpp = surface->format->BytesPerPixel;
  /* Here p is the address to the pixel we want to set */
  Uint8 *p = (Uint8 *) surface->pixels + y * surface->pitch + x * bpp;

  if (x >= surface->w || y >= surface->h || x < 0 || y < 0)
    return;

  switch (bpp)
    {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *) p = pixel;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
	{
	  p[0] = (pixel >> 16) & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = pixel & 0xff;
	}
      else
	{
	  p[0] = pixel & 0xff;
	  p[1] = (pixel >> 8) & 0xff;
	  p[2] = (pixel >> 16) & 0xff;
	}
      break;

    case 4:
      *(Uint32 *) p = pixel;
      break;
    }
}


void
Slock (SDL_Surface * door)
{
  if (SDL_MUSTLOCK (door))
    {
      if (SDL_LockSurface (door) < 0)
	{
	  fprintf (stderr, "Can't lock screen: %s\n", SDL_GetError ());
	}
    }
}


void
Sulock (SDL_Surface * door)
{
  if (SDL_MUSTLOCK (door))
    {
      SDL_UnlockSurface (door);
    }
}


void
SetTrans (SDL_Surface * src, int x, int y)
{
  Uint32 pixel;

  Slock (src);
  pixel = getpixel (src, x, y);
  Sulock (src);
  SDL_SetColorKey (src, SDL_SRCCOLORKEY | SDL_RLEACCEL, pixel);
}

void
SetRLE (SDL_Surface * src)
{
  SDL_SetColorKey (src, SDL_RLEACCEL, 0);
}

SDL_Surface *
LoadImg (char *file, Uint8 Use_Alpha, Uint8 trans)
{
  SDL_Surface *temp1 = NULL, *temp2 = NULL;

  temp1 = IMG_Load (file);
  if (trans)
    SetTrans (temp1, 0, 0);

  if (!Use_Alpha && !gblOps.useGL)
    {
      temp2 = SDL_DisplayFormat (temp1);
      SDL_FreeSurface (temp1);
    }
  else
    {
      SDL_SetAlpha (temp1, SDL_RLEACCEL | SDL_SRCALPHA, SDL_ALPHA_OPAQUE);
      temp2 = SDL_DisplayFormatAlpha (temp1);
      SDL_FreeSurface (temp1);
    }

  return temp2;
}


SDL_Surface *
ReversePic (SDL_Surface * source)
{
  SDL_Surface *destiny = NULL;
  Uint32 pixel;
  int i, j, w, h;

  //Generates a surface dor the destiny and makes the pinter aim to it...
  if ((source->flags & SDL_SRCALPHA) == SDL_SRCALPHA)
    destiny = SDL_DisplayFormatAlpha (source);
  else
    destiny = SDL_DisplayFormat (source);

  w = source->w;
  h = source->h;
  //locks the surfaces...
  Slock (source);
  Slock (destiny);

  for (j = 0; j < h; j++)
    {

      for (i = 0; i < w; i++)
	{
	  pixel = getpixel (source, i, j);	//Gets the pixel...
	  putpixel (destiny, ((w - 1) - i), j, pixel);
	  //And puts it in the other side.
	}
    }

  //unlocking surfaces...
  Sulock (destiny);
  Sulock (source);

  return destiny;		//Here we go!

}
