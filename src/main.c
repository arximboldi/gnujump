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

#include "setup.h"
#include "game.h"
#include "menu.h"
#include "records.h"
#include "replay.h"

SDL_Surface *screen = NULL;
L_gblOptions gblOps;

void displayHelp ();

void displayInfo ();

int parseArgs (int argc, char *argv[], char **replay);

int
main (int argc, char *argv[])
{
  data_t gfxdata;
  char *cfgFile;
  char *hscFile;
  char *homeDir;
  char *replay = NULL;

  /* Init gettext */
  setlocale (LC_ALL, "");
  textdomain (PACKAGE);
  bindtextdomain (PACKAGE, LOCALEDIR);

  /* Get the config file name */
#ifndef WIN32
  homeDir = getenv ("HOME");
  cfgFile =
    malloc (sizeof (char) *
	    (strlen (homeDir) + strlen (CONFDIR) + strlen (CFGFILE) + 3));
  hscFile =
    malloc (sizeof (char) *
	    (strlen (homeDir) + strlen (CONFDIR) + strlen (HSCFILE) + 3));
  sprintf (cfgFile, "%s/" CONFDIR "/", homeDir);
  sprintf (hscFile, "%s/" CONFDIR "/", homeDir);
#else
  cfgFile = malloc (sizeof (char) * strlen (CFGFILE) + 1);
  hscFile = malloc (sizeof (char) * strlen (HSCFILE) + 1);
  cfgFile[0] = hscFile[0] = '\0';
#endif
  strcat (cfgFile, CFGFILE);
  strcat (hscFile, HSCFILE);

  if (!loadConfigFile (cfgFile))
    {
      initGblOps ();		/* Set default options */
    }
  if (!loadRecords (hscFile, gblOps.records))
    {
      defaultRecords (gblOps.records);
    }

  /* Parse args */
  if (parseArgs (argc, argv, &replay))
    {
      return 1;
    }

  EngineInit ();

  gfxdata.soundloaded = FALSE;
  if (!loadGraphics (&gfxdata, gblOps.dataDir))
    return 1;

  //gblOps.fps = 20;
  if (replay != NULL)
    loadReplay (&gfxdata, replay);
  else
    mainMenu (&gfxdata);

  writeConfigFile (cfgFile);
  writeRecords (hscFile, gblOps.records);

  /* Free some things */
  freeGraphics (&gfxdata);
  freeSounds (&gfxdata);

  cleanGblOps ();

  Mix_CloseAudio ();

  free (cfgFile);
  free (hscFile);

  printf (_("\nHave a nice day!\n\n"));

  return 0;
}

void
displayHelp ()
{
  printf (_("GNUjump, an xjump clone. By Juan Pedro Bolivar Puente.\n\
This software can be redistributed and modified under the terms of the GPL.\n\n\
Usage: sdljump [REPLAY_FILE] [OPTIONS] \n\
Availible options:\n\
-b <int>  --bpp <int>     Sets the screen bitdepth to <int>. \n\
-f        --fullscreen    Force fullscreen mode.\n\
-s        --software      Force software rendering.\n\
-o        --opengl        Force OpenGL rendering.\n\
-a        --antialias     Force antialiasing for rotating sprites. \n\
-n        --no-aa         Disables antialiasing for rotating sprites. \n\
-?        --help          Displays this help screen.\n\
\nExample: sdljump myRep.rep -o -f\n"));
}

void
displayInfo ()
{
  printf
    ("\n*********************************************************************"
     "\n*                            GNUjump                                *"
     "\n*********************************************************************"
     "\nCopyright (C) 2005, Juan Pedro Bolivar Puente\n");

  printf ("\nVERSION: %s", VERSION);

  printf
    ("\n\n GNUjump is free software; you can redistribute it and/or modify\n"
     "it under the terms of the GNU General Public License as published by\n"
     "the Free Software Foundation; either version 3 of the License, or\n"
     "(at your option) any later version.\n\n"
     "SDLjump is distributed in the hope that it will be useful,\n"
     "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
     "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
     "GNU General Public License for more details.\n\n"
     "You should have received a copy of the GNU General Public License\n"
     "along with GNUjump; if not, write to the Free Software\n"
     "Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n");
}

int
parseArgs (int argc, char *argv[], char **replay)
{
  int i;
  for (i = 1; i < argc; i++)
    {
      if (argv[i][0] == '-')
	{
	  if (argv[i][1] != '-')
	    {			/* Short arg */
	      switch (argv[i][1])
		{
		case 'w':
		  if ((i + 1) < argc)
		    {
		      gblOps.w = atoi (argv[i + 1]);
		      i++;
		    }
		  break;
		case 'h':
		  if ((i + 1) < argc)
		    {
		      gblOps.h = atoi (argv[i + 1]);
		      i++;
		    }
		  break;
		case 'b':
		  if ((i + 1) < argc)
		    {
		      gblOps.bpp = atoi (argv[i + 1]);
		      i++;
		    }
		  break;
		case 'f':
		  gblOps.fullsc = TRUE;
		  break;
		case 'o':
		  gblOps.useGL = TRUE;
		  break;
		case 's':
		  gblOps.useGL = FALSE;
		  break;
		case 'a':
		  gblOps.aa = 1;
		  break;
		case 'n':
		  gblOps.aa = 0;
		  break;
		case '?':
		  displayHelp ();
		  return 1;
		  break;
		}
	    }
	  else
	    {			/* Long arg */
	      if (strcmp (argv[i], "--width") == 0)
		{
		  if ((i + 1) < argc)
		    {
		      gblOps.w = atoi (argv[i + 1]);
		      i++;
		    }
		}
	      if (strcmp (argv[i], "--height") == 0)
		{
		  if ((i + 1) < argc)
		    {
		      gblOps.h = atoi (argv[i + 1]);
		      i++;
		    }
		}
	      if (strcmp (argv[i], "--bpp") == 0)
		{
		  if ((i + 1) < argc)
		    {
		      gblOps.bpp = atoi (argv[i + 1]);
		      i++;
		    }
		}
	      if (strcmp (argv[i], "--fullscreen") == 0)
		{
		  gblOps.fullsc = TRUE;
		}
	      if (strcmp (argv[i], "--opengl") == 0)
		{
		  gblOps.useGL = TRUE;
		}
	      if (strcmp (argv[i], "--software") == 0)
		{
		  gblOps.useGL = FALSE;
		}
	      if (strcmp (argv[i], "--antialias") == 0)
		{
		  gblOps.aa = 1;
		}
	      if (strcmp (argv[i], "--no-aa") == 0)
		{
		  gblOps.aa = 0;
		}
	      if (strcmp (argv[i], "--help") == 0)
		{
		  displayHelp ();
		  return 1;
		}
	    }
	}
      else
	{			/* Replay */
	  if (*replay != NULL)
	    free (*replay);
	  *replay = malloc ((strlen (argv[i]) + 1) * sizeof (char));
	  strcpy (*replay, argv[i]);
	}
    }

  return FALSE;
}
