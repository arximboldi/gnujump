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

#include <dirent.h>

#include "gnujump.h"
#include "tools.h"

extern L_gblOptions gblOps;

long
getFileSize (char *fname)
{
  struct stat file;

  if (!stat (fname, &file))
    {
      return file.st_size;
    }

  return 0;
}

int
getFps ()
{
  switch (gblOps.fps)
    {
    case FPSNOLIMIT:
      return -1;
    case FPS100:
      return 100;
    case FPS300:
      return 300;
    case FPS40:
      return 40;
    default:
      return -1;
    }
}

int
isOdd (int n)
{
  if (n % 2 == 0)
    return 1;
  else
    return -1;
}

int
bool2sign (int n)
{
  if (n == 0)
    return -1;
  else
    return 1;
}

void
pressAnyKey ()
{
  SDL_Event event;

  do
    {
      SDL_WaitEvent (NULL);
      SDL_PollEvent (&event);
    }
  while (event.type != SDL_KEYDOWN && event.type != SDL_MOUSEBUTTONDOWN);

  while (SDL_PollEvent (&event));
}

#ifdef WIN32
#include "win/scandir.c"
#endif

int
getDirList (char *folder, char ***tab)
{
  struct dirent **namelist;
  struct stat buf;
  char sbuf[256];
  int n;
  int r = 0;


  n = scandir (folder, &namelist, 0, alphasort);
  if (n < 0)
    fprintf (stderr, _("WARNING: Folder (%s) doesn't exist.\n"), folder);
  else
    {
      while (n--)
	{
	  sprintf (sbuf, "%s/%s", folder, namelist[n]->d_name);
	  stat (sbuf, &buf);
	  if (S_ISDIR (buf.st_mode)
	      && (strcmp (namelist[n]->d_name, ".") != 0)
	      && (strcmp (namelist[n]->d_name, "..") != 0))
	    {
	      *tab = realloc (*tab, sizeof (char *) * (r + 1));
	      if (((*tab)[r] =
		   malloc (sizeof (char) * strlen (namelist[n]->d_name) +
			   1)) != NULL)
		{
		  strcpy ((*tab)[r], namelist[n]->d_name);
		}
	      r++;
	    }
	  free (namelist[n]);
	}
      free (namelist);
    }
  return r;
}

int
getFileList (char *folder, char ***tab)
{
  struct dirent **namelist;
//    struct stat buf;
  char sbuf[256];
  int n;
  int r = 0;


  n = scandir (folder, &namelist, 0, alphasort);
  if (n < 0)
    fprintf (stderr, _("WARNING: Folder (%s) doesn't exist.\n"), folder);
  else
    {
      while (n--)
	{
	  sprintf (sbuf, "%s/%s", folder, namelist[n]->d_name);
	  if ((strcmp (namelist[n]->d_name, ".") != 0)
	      && (strcmp (namelist[n]->d_name, "..") != 0))
	    {
	      *tab = realloc (*tab, sizeof (char *) * (r + 1));
	      if (((*tab)[r] =
		   malloc (sizeof (char) * strlen (namelist[n]->d_name) +
			   1)) != NULL)
		{
		  strcpy ((*tab)[r], namelist[n]->d_name);
		}
	      r++;
	    }
	  free (namelist[n]);
	}
      free (namelist);
    }
  return r;
}

int
sumStringTabs (char ***a, int an, char **b, int bn)
{
  int i;

  if ((*a = realloc (*a, sizeof (char *) * (an + bn))) != NULL)
    {
      for (i = an; i < an + bn; i++)
	{
	  (*a)[i] = b[i - an];
	}
      return i;
    }
  else
    {
      return 0;
    }
}

int
sumStringTabs_Cat (char ***a, int an, char **b, int bn, char *string)
{
  int i;
  char *newstr = NULL;
  if ((*a = realloc (*a, sizeof (char *) * (an + bn))) != NULL)
    {
      for (i = an; i < an + bn; i++)
	{
	  newstr =
	    malloc (sizeof (char) *
		    (strlen (string) + strlen (b[i - an]) + 2));
	  strcpy (newstr, string);
	  strcat (newstr, "/");
	  strcat (newstr, b[i - an]);
	  (*a)[i] = newstr;
	}
      return i;
    }
  else
    {
      return 0;
    }
}

int
checkExtension (char *file, char *ext)
{
  int i, j;

  for (i = strlen (file) - 1, j = 0; i >= 0 && j < strlen (ext); i--, j++)
    if (ext[strlen (ext) - j - 1] != file[i])
      return FALSE;

  if (j == strlen (ext))
    return TRUE;

  return FALSE;
}

//==============================================================================
// FADER
//==============================================================================

void
setFader (fader_t * fader, int start, int target, int time, int loop)
{
  fader->value = fader->start = start;
  fader->target = target;
  fader->time = time;
  fader->loop = loop;
}

int
updateFader (fader_t * fader, int ms)
{
  int swap;
  fader->value += fader->delta =
    (fader->target - fader->start) / fader->time * ms;
  if ((fader->target > fader->start && fader->value > fader->target)
      || (fader->target < fader->start && fader->value < fader->target))
    {
      fader->value = fader->target;
      fader->delta = 0;
      if (fader->loop)
	{
	  swap = fader->start;
	  fader->start = fader->target;
	  fader->target = swap;
	}
      return TRUE;
    }
  //if (fader->delta < 1) fader->delta = 1;
  return FALSE;
}

//==============================================================================
// RANDOM
//==============================================================================

int
rnd (int range)
{
  return (float) rand () / RAND_MAX * range;
}

void
srnd (void)
{
  srand (time (NULL));
}

//==============================================================================
// FILE READING & WRITING
//==============================================================================

void
putValue_int (FILE * tfile, char *data, int value)
{
  fprintf (tfile, " %s = %d \n", data, value);
}

void
putValue_str (FILE * tfile, char *data, char *value)
{
  fprintf (tfile, " %s = \"%s\" \n", data, value);
}

void
putComment (FILE * tfile, char *comment)
{
  fprintf (tfile, "# %s \n", comment);
}

void
putLine (FILE * tfile)
{
  fprintf (tfile, "\n");
}

void
findNextValue (FILE * tfile)
{
  char empty[MAX_CHAR];
  char c;

  fscanf (tfile, "%[ \n\t\f\r]", empty);
  c = getc (tfile);
  if (c == '#')
    {
      //fscanf(tfile, "%[^\n\f\r]",empty);
      fgets (empty, MAX_CHAR, tfile);
      findNextValue (tfile);
    }
  else
    {
      //fseek(tfile,-1,SEEK_CUR);
      ungetc (c, tfile);
    }
}

void
skipValueStr (FILE * tfile)
{
  char str[MAX_CHAR];

  findNextValue (tfile);
  fscanf (tfile, "%[^\n\t\f\r =]", str);
  fscanf (tfile, "%[= \n\t\f\r]", str);
  fscanf (tfile, "\"%[^\"]\"", str);
}

int
getValue_int (FILE * tfile, char *value)
{
  char tvalue[MAX_CHAR];
  char empty[MAX_CHAR];
  int data;

  findNextValue (tfile);
  fscanf (tfile, "%[^\n\t\f\r =]", tvalue);
  if (!strcmp (value, tvalue))
    {
      fscanf (tfile, "%[= \n\t\f\r]", empty);
      fscanf (tfile, "%i", &data);
      return data;
    }
  else
    {
      printf (_("ERROR: Value Mismatch ('%s' expected but '%s' obtained)\n"),
	      value, tvalue);
      return 0;
    }
}

float
getValue_float (FILE * tfile, char *value)
{
  char tvalue[MAX_CHAR];
  char empty[MAX_CHAR];
  float data;

  findNextValue (tfile);
  fscanf (tfile, "%[^\n\t\f\r =]", tvalue);
  if (!strcmp (value, tvalue))
    {
      fscanf (tfile, "%[= \n\t\f\r]", empty);
      fscanf (tfile, "%f", &data);
      return data;
    }
  else
    {
      printf (_("ERROR: Value Mismatch ('%s' expected but '%s' obtained)\n"),
	      value, tvalue);
      return 0;
    }
}

void
getValue_str (FILE * tfile, char *value, char *data, char *path)
{
  char tvalue[MAX_CHAR];
  char empty[MAX_CHAR];
  char info[MAX_CHAR];

  findNextValue (tfile);
  fscanf (tfile, "%[^\n\t\f\r =]", tvalue);
  if (!strcmp (value, tvalue))
    {
      fscanf (tfile, "%[= \n\t\f\r]", empty);
      fscanf (tfile, "\"%[^\"]\"", info);
      // If the string is a path, we add the data folder path before.
      if (path != NULL)
	{
	  sprintf (data, "%s/%s", path, info);
	}
      else
	{
	  sprintf (data, "%s", info);
	}
      return;
    }
  else
    {
      printf (_("ERROR: Value Mismatch ('%s' expected but '%s' obtained)\n"),
	      value, tvalue);
      return;
    }
}

char *
getValue_charp (FILE * tfile, char *value)
{
  char tvalue[MAX_CHAR];
  char empty[MAX_CHAR];
  char info[MAX_CHAR];
  char *ret = NULL;

  findNextValue (tfile);
  fscanf (tfile, "%[^\n\t\f\r =]", tvalue);
  if (!strcmp (value, tvalue))
    {
      fscanf (tfile, "%[= \n\t\f\r]", empty);
      fscanf (tfile, "\"%[^\"]\"", info);

      ret = malloc (sizeof (char) * (strlen (info) + 1));
      strcpy (ret, info);

      return ret;
    }
  else
    {
      printf (_("ERROR: Value Mismatch ('%s' expected but '%s' obtained)\n"),
	      value, tvalue);
      return NULL;
    }
}
