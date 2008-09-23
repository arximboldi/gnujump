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

#include <stdio.h>

#include "gnujump.h"
#include "sprite.h"
#include "tools.h"

extern SDL_Surface *screen;
extern L_gblOptions gblOps;

L_spriteDataRot *
loadSpriteDataRot (char *filename, int sides, char *path)
{
  FILE *tfile;
  L_spriteDataRot *sprite = NULL;
  char fname[MAX_CHAR];
  int i;

  if ((tfile = fopen (filename, "r")) == NULL)
    {
      fprintf (stderr, "ERROR: Could not load sprite file: %s", filename);
      return NULL;
    }
  if ((sprite = malloc (sizeof (L_spriteDataRot))) == NULL)
    {
      return NULL;
    }
  /*checks the number of frames... */
  sprite->nFrames = getValue_int (tfile, "frames");
  sprite->sides = sides;
  sprite->pic = NULL;
  sprite->time = NULL;

  /* Allocate memory for the images and the time data. */
  sprite->pic =
    (JPB_surfaceRot **) calloc ((sprite->nFrames * sprite->sides),
				sizeof (JPB_surfaceRot *));
  sprite->time = malloc (sprite->nFrames * sizeof (float));

  for (i = 0; i < sprite->nFrames; i++)
    {
      /*and loads the pic and time of each frame... */
      getValue_str (tfile, "pic", fname, path);
      sprite->pic[i + (RIGHT * sprite->nFrames)] =
	JPB_LoadImgRot (fname, gblOps.useGL, 1, 0, 0);

      /*Loads the surface of the picture looking to the inverse side. */
      if (sides > 1)
	sprite->pic[i + (LEFT * sprite->nFrames)] =
	  JPB_LoadImgRot (fname, gblOps.useGL, 1, 0, 1);
      sprite->time[i] = getValue_float (tfile, "time");
    }

  fclose (tfile);
  return sprite;
}

L_spriteData *
loadSpriteData (char *filename, int sides, char *path)
{
  FILE *tfile;
  L_spriteData *sprite = NULL;
  char fname[MAX_CHAR];
  int i;

  if ((tfile = fopen (filename, "r")) == NULL)
    {
      fprintf (stderr, "ERROR: Could not load sprite file: %s", filename);
      return NULL;
    }
  if ((sprite = malloc (sizeof (L_spriteData))) == NULL)
    {
      return NULL;
    }
  /*checks the number of frames... */
  sprite->nFrames = getValue_int (tfile, "frames");
  sprite->sides = sides;
  //Allocate memory for the images and the time data.
  sprite->pic =
    malloc (sprite->nFrames * sprite->sides * sizeof (JPB_surface *));
  sprite->time = malloc (sprite->nFrames * sizeof (float));

  for (i = 0; i < sprite->nFrames; i++)
    {
      /*and loads the pic and time of each frame... */
      getValue_str (tfile, "pic", fname, path);
      sprite->pic[i + LEFT * sprite->nFrames] =
	JPB_LoadImg (fname, gblOps.useGL, 1, 0, 0);

      /*Loads the surface of the picture looking to the inverse side. */
      if (sprite->sides > 1)
	sprite->pic[i + RIGHT * sprite->nFrames] =
	  JPB_LoadImg (fname, gblOps.useGL, 1, 0, 1);

      sprite->time[i] = getValue_float (tfile, "time");
    }

  fclose (tfile);
  return sprite;
}

int
animateSprite (L_spriteCtl * sprite, float dt)
{
  sprite->elpTime += dt;
  if (sprite->elpTime >= sprite->sdata->time[sprite->frame])
    {
      sprite->elpTime -= sprite->sdata->time[sprite->frame];
      sprite->frame += 1;
      if (sprite->frame >= sprite->sdata->nFrames)
	{
	  sprite->frame = 0;
	  return TRUE;
	}
    }
  return FALSE;
}

int
animateSpriteRot (L_spriteCtlRot * sprite, float dt)
{
  sprite->elpTime += dt;
  if (sprite->elpTime >= sprite->sdata->time[sprite->frame])
    {
      sprite->elpTime -= sprite->sdata->time[sprite->frame];
      sprite->frame += 1;
      if (sprite->frame >= sprite->sdata->nFrames)
	{
	  sprite->frame = 0;
	  return TRUE;
	}
    }
  return FALSE;
}

void
printSprite (L_spriteCtl * sprite, SDL_Rect * src_r, SDL_Rect * dest_r,
	     int side)
{
  JPB_PrintSurface (sprite->sdata->
		    pic[sprite->frame + sprite->sdata->nFrames * side], src_r,
		    dest_r);
}

void
printSpriteRot (L_spriteCtlRot * sprite, SDL_Rect * src_r, SDL_Rect * dest_r,
		int side, float angle)
{
  JPB_PrintSurfaceRot (sprite->sdata->
		       pic[sprite->frame + sprite->sdata->nFrames * side],
		       src_r, dest_r, angle);
}


void
freeSpriteData (L_spriteData * sprite)
{
  int i, j;
  for (i = 0; i < sprite->sides; i++)
    {
      for (j = 0; j < sprite->nFrames; j++)
	{
	  JPB_FreeSurface (sprite->pic[j + sprite->nFrames * i]);
	}
    }
  free (sprite->pic);
  free (sprite->time);
  free (sprite);
}

void
freeSpriteDataRot (L_spriteDataRot * sprite)
{
  int i, j;

  for (i = 0; i < sprite->sides; i++)
    {
      for (j = 0; j < sprite->nFrames; j++)
	{
	  JPB_FreeSurfaceRot (sprite->pic[j + sprite->nFrames * i]);
	}
    }
  free (sprite->pic);
  free (sprite->time);
  free (sprite);
}

JPB_surfaceRot *
getFrameRot (L_spriteCtlRot * sprite, int side)
{
  return sprite->sdata->pic[sprite->frame + sprite->sdata->nFrames * side];
}

void
initializeSpriteCtlRot (L_spriteCtlRot * sprite, L_spriteDataRot * data)
{
  sprite->elpTime = 0;
  sprite->frame = 0;
  sprite->sdata = data;
}

void
initializeSpriteCtl (L_spriteCtl * sprite, L_spriteData * data)
{
  sprite->elpTime = 0;
  sprite->frame = 0;
  sprite->sdata = data;
}
