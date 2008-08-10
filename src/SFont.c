
/******************************************************************************
 *      MODIFIED BY JUAN PEDRO BOLIVAR. May not work outside this project     *
 ******************************************************************************/

/*  SFont: a simple font-library that uses special .pngs as fonts
    Copyright (C) 2003 Karl Bartel

    License: GPL or LGPL (at your choice)
    WWW: http://www.linux-games.com/sfont/

    This program is free software; you can redistribute it and/or modify        
    it under the terms of the GNU General Public License as published by        
    the Free Software Foundation; either version 2 of the License, or           
    (at your option) any later version.                                         
                                                                                
    This program is distributed in the hope that it will be useful,       
    but WITHOUT ANY WARRANTY; without even the implied warranty of              
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               
    GNU General Public License for more details.                
                                                                               
    You should have received a copy of the GNU General Public License           
    along with this program; if not, write to the Free Software                 
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA   
                                                                                
    Karl Bartel
    Cecilienstr. 14                                                    
    12307 Berlin
    GERMANY
    karlb@gmx.net                                                      
*/

#include <SDL/SDL.h>

#include <assert.h>
#include <stdlib.h>

#include "SFont.h"
#include "surface.h"

static Uint32
GetPixel (SDL_Surface * Surface, Sint32 X, Sint32 Y)
{
  Uint8 *bits;
  Uint32 Bpp;

  assert (X >= 0);
  assert (X < Surface->w);

  Bpp = Surface->format->BytesPerPixel;
  bits = ((Uint8 *) Surface->pixels) + Y * Surface->pitch + X * Bpp;

  // Get the pixel
  switch (Bpp)
    {
    case 1:
      return *((Uint8 *) Surface->pixels + Y * Surface->pitch + X);
      break;
    case 2:
      return *((Uint16 *) Surface->pixels + Y * Surface->pitch / 2 + X);
      break;
    case 3:
      {				// Format/endian independent 
	Uint8 r, g, b;
	r = *((bits) + Surface->format->Rshift / 8);
	g = *((bits) + Surface->format->Gshift / 8);
	b = *((bits) + Surface->format->Bshift / 8);
	return SDL_MapRGB (Surface->format, r, g, b);
      }
      break;
    case 4:
      return *((Uint32 *) Surface->pixels + Y * Surface->pitch / 4 + X);
      break;
    }

  return -1;
}

SFont_Font *
SFont_InitFont (char *file, SDL_Surface * Surface, Uint8 gl, Uint8 alpha)
{
  int x = 0, i = 0;
  Uint32 pixel;
  SFont_Font *Font;
  Uint32 pink;

  if (Surface == NULL)
    return NULL;

  Font = (SFont_Font *) malloc (sizeof (SFont_Font));
  //Font->Surface = Surface;

  //SDL_LockSurface(Surface);

  pink = SDL_MapRGB (Surface->format, 255, 0, 255);
  while (x < Surface->w)
    {
      if (GetPixel (Surface, x, 0) == pink)
	{
	  Font->CharPos[i++] = x;
	  while ((x < Surface->w) && (GetPixel (Surface, x, 0) == pink))
	    x++;
	  Font->CharPos[i++] = x;
	}
      x++;
    }
  Font->MaxPos = x - 1;

  pixel = GetPixel (Surface, 0, Surface->h - 1);
  //SDL_UnlockSurface(Surface);

  //SDL_SetColorKey(Surface, SDL_SRCCOLORKEY, pixel);

  Font->Surface = JPB_LoadImg (file, gl, alpha, 0, 0);

  return Font;
}

void
SFont_FreeFont (SFont_Font * FontInfo)
{
  JPB_FreeSurface (FontInfo->Surface);
  free (FontInfo);
}

void
SFont_Write (SFont_Font * Font, int x, int y, char *text)
{
  char *c;
  int charoffset;
  SDL_Rect srcrect, dstrect;

  if (text == NULL)
    return;

  // these values won't change in the loop
  srcrect.y = 1;
  dstrect.y = y;
  srcrect.h = dstrect.h = Font->Surface->h - 1;

  for (c = text; *c != '\0'; c++)
    {
      charoffset = ((int) (*c - 33)) * 2 + 1;
      // skip spaces and nonprintable characters
      if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos)
	{
	  x += Font->CharPos[2] - Font->CharPos[1];
	  continue;
	}

      srcrect.w = dstrect.w =
	(Font->CharPos[charoffset + 2] + Font->CharPos[charoffset + 1]) / 2 -
	(Font->CharPos[charoffset] + Font->CharPos[charoffset - 1]) / 2;
      srcrect.x =
	(Font->CharPos[charoffset] + Font->CharPos[charoffset - 1]) / 2;
      dstrect.x =
	x - (float) (Font->CharPos[charoffset] -
		     Font->CharPos[charoffset - 1]) / 2;

      //JPB_PrintSurface(Font->Surface, &srcrect, &dstrect);
      JPB_PrintSurface (Font->Surface, &srcrect, &dstrect);


      x += Font->CharPos[charoffset + 1] - Font->CharPos[charoffset];
    }
}

int
SFont_TextWidth (const SFont_Font * Font, const char *text)
{
  const char *c;
  int charoffset = 0;
  int width = 0;

  if (text == NULL)
    return 0;

  for (c = text; *c != '\0'; c++)
    {
      charoffset = ((int) *c - 33) * 2 + 1;
      // skip spaces and nonprintable characters
      if (*c == ' ' || charoffset < 0 || charoffset > Font->MaxPos)
	{
	  width += Font->CharPos[2] - Font->CharPos[1];
	  continue;
	}

      width += Font->CharPos[charoffset + 1] - Font->CharPos[charoffset];
    }

  return width;
}

int
SFont_TextHeight (const SFont_Font * Font)
{
  return Font->Surface->h - 1;
}

void
SFont_WriteCenter (SDL_Surface * Surface, SFont_Font * Font,
		   int y, char *text)
{
  SFont_Write (Font, Surface->w / 2 - SFont_TextWidth (Font, text) / 2,
	       y, text);
}

/* Added by Juan Pedro Bolívar Puente */
int
charWidth (SFont_Font * Font, char c)
{
  int charoffset = ((int) c - 33) * 2 + 1;
  /* skip spaces and nonprintable characters */
  if (c == ' ' || charoffset < 0 || charoffset > Font->MaxPos)
    {
      return (Font->CharPos[2] - Font->CharPos[1]);
    }
  return (Font->CharPos[charoffset + 1] - Font->CharPos[charoffset]);
}

int
findWord (SFont_Font * Font, char *text, int i)
{
  char *c;
  for (c = text + i; *c != ' ' && *c != '/' && *c != '\\' && *c != '\0'; c++)
    {
      i++;
    }
  return i;
}

int
wordWidth (SFont_Font * Font, char *text, int i, int end)
{
  int width = 0;
  for (; i < end; i++)
    {
      width += charWidth (Font, text[i]);
    }
  return width;
}

void
copyWord (SFont_Font * Font, char *text, char *dest, int start, int end)
{
  int i;
  int len = strlen (dest);
  for (i = 0; i <= (end - start); i++)
    {
      dest[len + i] = text[start + i];
    }
  dest[len + i] = '\0';
}

void
SFont_WriteAligned (SFont_Font * Font, int x, int y, int w,
		    int gap, int align, char *text)
{
  char buf[512] = "";
  int width = 0;
  int ww = 0;
  int len = strlen (text);
  int i = 0;
  int nextWord = 0;

  if (text == NULL)
    return;

  for (i = 0; i < len; i++)
    {
      ww = wordWidth (Font, text, i, nextWord + 1);
      if ((width + ww) >= w)
	{
	  switch (align)
	    {
	    case ALEFT:
	      SFont_Write (Font, x, y, buf);
	      break;
	    case ARIGHT:
	      SFont_Write (Font, x + w - width, y, buf);
	      break;
	    case ACENTER:
	      SFont_Write (Font, x + (w - width) / 2, y, buf);
	      break;
	    default:
	      break;
	    }
	  width = 0;
	  buf[0] = '\0';
	  y += Font->Surface->h + gap;
	}
      width += ww;
      copyWord (Font, text, buf, i, nextWord);
      i = nextWord;
      nextWord = findWord (Font, text, i + 1);
    }

  switch (align)
    {
    case ALEFT:
      SFont_Write (Font, x, y, buf);
      break;
    case ARIGHT:
      SFont_Write (Font, x + w - width, y, buf);
      break;
    case ACENTER:
      SFont_Write (Font, x + (w - width) / 2, y, buf);
      break;
    default:
      break;
    }
}

int
SFont_AlignedHeight (SFont_Font * Font, int w, int gap, char *text)
{
  char buf[512] = "";
  int width = 0;
  int ww = 0;
  int len = strlen (text);
  int i = 0;
  int y = 0;
  int nextWord = 0;

  if (text == NULL)
    return 0;

  for (i = 0; i < len; i++)
    {
      ww = wordWidth (Font, text, i, nextWord + 1);
      if ((width + ww) >= w)
	{
	  width = 0;
	  buf[0] = '\0';
	  y += Font->Surface->h + gap;
	}
      width += ww;
      copyWord (Font, text, buf, i, nextWord);
      i = nextWord;
      nextWord = findWord (Font, text, i + 1);
    }
  return y += Font->Surface->h;
}

int
SFont_FillWith (SFont_Font * Font, int x, int y, int w, char c)
{
  int charoffset = ((int) c - 33) * 2 + 1;
  int charw = charWidth (Font, c);
  int i, j = 0;
  SDL_Rect srcrect, dstrect;

  srcrect.y = 1;
  dstrect.y = y;
  srcrect.h = dstrect.h = Font->Surface->h - 1;
  for (i = x + (x % charw); i <= x + w - charw; i += charw)
    {
      srcrect.w = dstrect.w =
	(Font->CharPos[charoffset + 2] + Font->CharPos[charoffset + 1]) / 2 -
	(Font->CharPos[charoffset] + Font->CharPos[charoffset - 1]) / 2;
      srcrect.x =
	(Font->CharPos[charoffset] + Font->CharPos[charoffset - 1]) / 2;
      dstrect.x =
	i - (float) (Font->CharPos[charoffset] -
		     Font->CharPos[charoffset - 1]) / 2;

      //JPB_PrintSurface(Font->Surface, &srcrect, &dstrect);
      JPB_PrintSurface (Font->Surface, &srcrect, &dstrect);
      j++;
    }
  return j;
}

void
SFont_WriteMaxWidth (SFont_Font * Font, int x, int y, int w, int align,
		     char *tag, char *text)
{
  int tagw = SFont_TextWidth (Font, tag);
  int width = SFont_TextWidth (Font, text);
  int i = 0;
  char *buf = NULL;

  if (width >= w - tagw)
    {
      buf = malloc (sizeof (char) * (strlen (text) + 1));
      width = 0;
      while (width <= w - tagw)
	{
	  buf[i] = text[i];
	  buf[i + 1] = '\0';
	  width += charWidth (Font, text[i]);
	  i++;
	}
      width -= charWidth (Font, text[i]);
      buf[--i] = '\0';
      strcat (buf, tag);
      width += tagw;
    }
  else
    {
      buf = text;
    }

  switch (align)
    {
    case ALEFT:
      SFont_Write (Font, x, y, buf);
      break;
    case ARIGHT:
      SFont_Write (Font, x + w - width, y, buf);
      break;
    case ACENTER:
      SFont_Write (Font, x + (w - width) / 2, y, buf);
      break;
    default:
      break;
    }

  if (buf != text)
    free (buf);

}

int
SFont_SetAlpha (SFont_Font * font, int alpha)
{
  int r;
  r = font->Surface->alpha;
  font->Surface->alpha = alpha;
  return r;
}
