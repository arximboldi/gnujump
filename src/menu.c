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

#include <stdarg.h>

#include "gnujump.h"
#include "menu.h"
#include "game.h"
#include "setup.h"
#include "tools.h"
#include "replay.h"

extern SDL_Surface *screen;
extern L_gblOptions gblOps;

void
saveReplayMenu (data_t * gfx, replay_t * rep)
{
  char *fname;
  char *comment;
  int done = FALSE;
  menu_t menu;

  fname = malloc (sizeof (char) * (strlen ("default") + 1));
  sprintf (fname, "default");
  comment = malloc (sizeof (char) * (strlen ("no comment") + 1));
  sprintf (comment, "no comment");

  initMenuT (&menu);
  addMenuTOption (&menu, _("Play"), _("Watch this replay."), MB_RETURN, NULL,
		  0);
  addMenuTOption (&menu, _("Filename"),
		  _("Give a nice filename to your replay."), MB_INPUT,
		  &(fname), 0);
  addMenuTOption (&menu, _("Comment"),
		  _("Attach some annotations or explanations to the replay"),
		  MB_INPUT, &(comment), 0);
  addMenuTOption (&menu, _("Save"),
		  _
		  ("Store the replay to the disk with the given parameters."),
		  MB_RETURN, NULL, 0);
  addMenuTOption (&menu, _("Cancel"),
		  _("I do not want to store this replay."), MB_RETURN, NULL,
		  0);

  while (!done)
    {
      switch (playMenuT (gfx, &menu))
	{
	case 0:
	  while (playReplay (gfx, rep));
	  break;
	case 3:
	  saveReplay (rep, fname, comment);
	  done = TRUE;
	  break;
	case 4:
	case NONE:
	  done = TRUE;
	  break;
	default:
	  break;
	}
    }

  free (fname);
  free (comment);

  freeMenuT (&menu);
}

void
mainMenu (data_t * gfx)
{
  int opt;
  int done = FALSE;

  if (gfx->musmenu)
    Mix_PlayMusic (gfx->musmenu, -1);

  while (!done)
    {
      opt = playMenu (gfx, -6,
		      _("New Game"), _("Play a new game."),
		      _("Options"), _("Configure some settings."),
		      _("Highscores"),
		      _("Have a look at the local Hall of Fame."),
		      _("Replays"), _("Watch previously played games."),
		      _("Credits"), _("See who made this program."),
		      _("Quit"), _("See you some other day :)"));

      switch (opt)
	{
	case 0:
	  newGameMenu (gfx);
	  break;
	case 1:
	  optionsMenu (gfx);
	  break;
	case 2:
	  JPB_PrintSurface (gfx->gameBg, NULL, NULL);
	  drawRecords (gfx, gblOps.records, -1);
	  pressAnyKey ();
	  break;
	case 3:
	  viewReplayMenu (gfx);
	  break;
	case 4:
	  JPB_PrintSurface (gfx->gameBg, NULL, NULL);
	  drawCredits (gfx);
	  pressAnyKey ();
	  break;
	case 5:
	case NONE:
	  done = TRUE;
	  break;
	default:
	  break;
	}
    }
}

void
newGameMenu (data_t * gfx)
{
  int opt;
  int done = FALSE;
  menu_t menu;

  initMenuT (&menu);

  addMenuTOption (&menu, _("Start Game"), _("Start playing this match"), 0,
		  NULL, NONE);
  addMenuTOption (&menu, _("Players:"),
		  _("Set the number of players for this match."), MB_CHOOSE,
		  &gblOps.nplayers, 4, "1", "2", "3", "4");
  addMenuTOption (&menu, _("MP Lives:"),
		  _
		  ("Set the number of tries each player will have in multiplayer games."),
		  MB_CHOOSE, &gblOps.mpLives, 5, "1", "2", "3", "4", "5");
  addMenuTOption (&menu, _("Replay:"),
		  _
		  ("Activate this option if you want the match to be recorder while you play."),
		  MB_CHOOSE, &gblOps.recReplay, 2, _("Off"), _("On"));
  addMenuTOption (&menu, _("Configure Players"),
		  _
		  ("Select some options for each player, such as keys, name, etc."),
		  0, NULL, NONE);
  addMenuTOption (&menu, _("Back"), _("Leave this menu"), 0, NULL, NONE);


  while (!done)
    {
      gblOps.mpLives -= 1;	// Start at 0
      opt = playMenuT (gfx, &menu);
      gblOps.mpLives += 1;	// restore mplives
      if (opt == 0)
	{
	  while (playGame (gfx, gblOps.nplayers + 1))
	    SDL_Delay (1);

	  if (gfx->musmenu)
	    Mix_PlayMusic (gfx->musmenu, -1);
	}
      else if (opt == 4)
	configurePlayersMenu (gfx);
      else if (opt == 5 || opt == NONE)
	done = TRUE;
    }

  freeMenuT (&menu);
}

void
configurePlayersMenu (data_t * gfx)
{
  int opt;
  int player = 0;
  int done = FALSE;
  menu_t menu;

  while (!done)
    {
      initMenuT (&menu);
      addMenuTOption (&menu, _("Player:"),
		      _("Choose which player do you want to configure."),
		      MB_RETURN | MB_CHOOSE, &player, 4, "1", "2", "3", "4");
      addMenuTOption (&menu, _("Name:"), _("Choose a name for this player."),
		      MB_INPUT, &(gblOps.pname[player]), 0);
      addMenuTOption (&menu, _("Left Key:"),
		      _
		      ("Press enter and then press the key you want to use."),
		      MB_KEYDEF, &(gblOps.keys[player][LEFTK]), 0);
      addMenuTOption (&menu, _("Right Key:"),
		      _
		      ("Press enter and then press the key you want to use."),
		      MB_KEYDEF, &(gblOps.keys[player][RIGHTK]), 0);
      addMenuTOption (&menu, _("Jump Key:"),
		      _
		      ("Press enter and then press the key you want to use."),
		      MB_KEYDEF, &(gblOps.keys[player][JUMPK]), 0);

      addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

      opt = playMenuT (gfx, &menu);
      if (opt == NONE || opt == 5)
	done = TRUE;

      freeMenuT (&menu);
    }
}

void
optionsMenu (data_t * gfx)
{
  int opt;
  int done = FALSE;
  menu_t menu;

  initMenuT (&menu);
  addMenuTOption (&menu, _("Choose Theme"),
		  _("Change the appearance and sounds of the game."), 0, NULL,
		  NONE);
  /* addMenuTOption(&menu, _("Choose Language"), _("Select a language."), 0, NULL, NONE); */
  addMenuTOption (&menu, _("Game Options"), _("Change the gameplay."), 0,
		  NULL, NONE);
  addMenuTOption (&menu, _("Graphic Options"),
		  _("Modify how the game is displayed in the screen."), 0,
		  NULL, NONE);
  addMenuTOption (&menu, _("Sound Options"),
		  _("Modify options related to sound effects and music."), 0,
		  NULL, NONE);
  addMenuTOption (&menu, _("Manage Folders"),
		  _("Change the folders where themes and replays are."), 0,
		  NULL, NONE);
  addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

  while (!done)
    {
      opt = playMenuT (gfx, &menu);

      switch (opt)
	{
	case 0:
	  chooseThemeMenu (gfx);
	  break;
	  /*case 1:
	     if (chooseLangMenu(gfx)) done = TRUE;
	     break; */
	case 1:
	  gameOptionsMenu (gfx);
	  break;
	case 2:
	  gfxOptionsMenu (gfx);
	  break;
	case 3:
	  soundOptionsMenu (gfx);
	  break;
	case 4:
	  folderOptionsMenu (gfx);
	  break;
	case 5:
	case NONE:
	  done = TRUE;
	  break;
	default:
	  break;
	}
    }

  freeMenuT (&menu);
}

void
soundOptionsMenu (data_t * gfx)
{
  int opt;
  int done = FALSE;
  menu_t menu;

  initMenuT (&menu);
  addMenuTOption (&menu, _("Sound Volume:"),
		  _
		  ("Set how low you want to hear the sounds. 0 means off. 9 is the max value."),
		  MB_CHOOSE | MB_VOLSET, &(gblOps.sndvolume), 10, "0", "1",
		  "2", "3", "4", "5", "6", "7", "8", "9");
  addMenuTOption (&menu, _("Music Volume:"),
		  _
		  ("Set how low you want to hear the music. 0 means off. 9 is the max value."),
		  MB_CHOOSE | MB_VOLSET, &(gblOps.musvolume), 10, "0", "1",
		  "2", "3", "4", "5", "6", "7", "8", "9");
  addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

  while (!done)
    {
      opt = playMenuT (gfx, &menu);

      switch (opt)
	{
	case 2:
	case NONE:
	  done = TRUE;
	  break;
	default:
	  break;
	}
    }

  freeMenuT (&menu);
}

void
folderOptionsMenu (data_t * gfx)
{
  int opt;
  int done = FALSE;
  menu_t menu;

  initMenuT (&menu);
  addMenuTOption (&menu, _("Replay save folder:"),
		  _("Choose where new recorded replays should be stored."),
		  MB_INPUT, &(gblOps.repDir), NONE);
  addMenuTOption (&menu, _("Replay Folders"),
		  _
		  ("Manage the folders where GNUjump should look for replays."),
		  0, NULL, NONE);
  addMenuTOption (&menu, _("Theme Folders"),
		  _
		  ("Manage the folders where GNUjump should look for themes."),
		  0, NULL, NONE);
  addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

  while (!done)
    {
      opt = playMenuT (gfx, &menu);

      switch (opt)
	{
	case 1:
	  gblOps.nrfolders =
	    manageDirsMenu (gfx, &gblOps.repDirs, gblOps.nrfolders);
	  break;
	case 2:
	  gblOps.ntfolders =
	    manageDirsMenu (gfx, &gblOps.themeDirs, gblOps.ntfolders);
	  break;
	case 3:
	  done = TRUE;
	  break;
	case NONE:
	  done = TRUE;
	  break;
	default:
	  break;
	}
    }

  freeMenuT (&menu);
}

void
gameOptionsMenu (data_t * gfx)
{
  menu_t menu;

  initMenuT (&menu);

  addMenuTOption (&menu, _("FPS Limit:"),
		  _
		  ("The original Xjump worked at 40 fps. Higher FPS imply more fluid gameplay, but limiting it is good if you don't want to use all your CPU."),
		  MB_CHOOSE, &gblOps.fps, 4, "Xjump", "100 FPS", "300 FPS",
		  _("No limit."));

  addMenuTOption (&menu, _("Rotation:"),
		  _("Change how the player rotates when jumping."), MB_CHOOSE,
		  &gblOps.rotMode, 3, _("None"), _("Xjump"), _("Full"));

  addMenuTOption (&menu, _("Scrolling"),
		  _("How do you want the tower to fall."), MB_CHOOSE,
		  &gblOps.scrollMode, 2, _("Xjump"), _("Soft"));

  addMenuTOption (&menu, _("Scroll BG"),
		  _("Do you want the background an walls to scroll?"),
		  MB_CHOOSE, &gblOps.scrollMode, 2, _("No"), _("Yes"));

  addMenuTOption (&menu, _("Trail:"),
		  _
		  ("Set this if you want the player to leave a trail behind him."),
		  MB_CHOOSE, &gblOps.trailMode, 4, _("None"), _("Thin"),
		  _("Normal"), _("Strong"));

  addMenuTOption (&menu, _("Blur:"),
		  _
		  ("Nice blur effect. Set a level from 0 to 9. Only availible in OpenGL mode."),
		  MB_CHOOSE, &gblOps.blur, 10, "0", "1", "2", "3", "4", "5",
		  "6", "7", "8", "9");

  addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

  playMenuT (gfx, &menu);

  freeMenuT (&menu);
}

void
gfxOptionsMenu (data_t * gfx)
{
  menu_t menu;
  int done = 0;
  int ogl = gblOps.useGL;

  initMenuT (&menu);

  addMenuTOption (&menu, _("Fullscreen:"),
		  _
		  ("Choose wether you want to play in a window or feel the overwhelming sensation of falling in all your screen!"),
		  MB_CHOOSE | MB_RETURN, &gblOps.fullsc, 2, _("Off"),
		  _("On"));

  addMenuTOption (&menu, _("OpenGL:"),
		  _
		  ("Use OpenGL if you've got a 3d card. In most cases it will improve the performance. Some fading effects are only availible in this mode."),
		  MB_CHOOSE | MB_RETURN, &ogl, 2, _("Off"), _("On"));

  addMenuTOption (&menu, _("Color depth:"),
		  _
		  ("How many colors do you want to see in the screen. Usually auto is the best option, but when playing in fullscreen lower values might improve performance."),
		  MB_CHOOSE | MB_RETURN, &gblOps.bpp, 4, "32 bpp", "16 bpp",
		  "8 bpp", _("Auto"));

  addMenuTOption (&menu, _("Antialiasing:"),
		  _
		  ("Antialiasing will improve the look of the character when full rotation is set."),
		  MB_CHOOSE | MB_RETURN, &gblOps.aa, 2, _("Off"), _("On"));

  addMenuTOption (&menu, _("Back"), _("Leave this menu."), 0, NULL, NONE);

  while (!done)
    {
      switch (playMenuT (gfx, &menu))
	{
	case 0:
	  setWindow ();
	  break;
	case 1:
	case 2:
	case 3:
	  gblOps.useGL = ogl;
	  freeGraphics (gfx);
	  loadGraphics (gfx, gblOps.dataDir);

	  if (gfx->musmenu)
	    Mix_PlayMusic (gfx->musmenu, -1);
	  break;
	case 4:
	case NONE:
	default:
	  done = TRUE;
	  break;
	}
    }

  freeMenuT (&menu);
}

void
viewReplayMenu (data_t * gfx)
{
  char **options = NULL;
  char **tips = NULL;
  char **dirs = NULL;
  char **buf = NULL;
  char **fullpaths = NULL;
  int *index = NULL;

  char *str = NULL;
  int nf = 0, n = 0, no = 0;
  int i = 0;
  int r;
  int done = FALSE;

  for (i = 0; i < gblOps.nrfolders; i++)
    {
      n = getFileList (gblOps.repDirs[i], &buf);
      sumStringTabs (&dirs, nf, buf, n);
      nf = sumStringTabs_Cat (&fullpaths, nf, buf, n, gblOps.repDirs[i]);
      free (buf);
      buf = NULL;
    }

  no = 0;
  for (i = 0; i < nf; i++)
    {
      if (checkExtension (fullpaths[i], REPEXT)
	  || checkExtension (fullpaths[i], REPEXTOLD))
	str = getReplayComment (fullpaths[i]);
      else
	str = NULL;

      if (str != NULL)
	{
	  no++;

	  index = realloc (index, sizeof (int) * no);
	  tips = realloc (tips, sizeof (char *) * no);
	  options = realloc (options, sizeof (char *) * no);

	  tips[no - 1] = str;
	  options[no - 1] = dirs[i];
	  index[no - 1] = i;
	}
    }

  while (!done)
    {
      r = playMenuTab (gfx, -no, options, tips);
      if (r >= 0 && r < no)
	{
	  loadReplay (gfx, fullpaths[index[r]]);
	  if (gfx->musmenu)
	    Mix_PlayMusic (gfx->musmenu, -1);
	}
      else if (r == NONE)
	{
	  done = TRUE;
	}
    }

  for (i = 0; i < nf; i++)
    {
      free (dirs[i]);
      free (fullpaths[i]);
    }
  for (i = 0; i < no; i++)
    {
      free (tips[i]);
    }

  free (tips);
  free (options);
  free (index);
  free (dirs);
  free (fullpaths);
}

int
chooseLangMenu (data_t * gfx)
{
/*	char **options = NULL;
	char **tips = NULL;
	char **dirs = NULL;
	char **buf = NULL;
	char **fullpaths = NULL;
	int *index = NULL;

	char* str = NULL;
	int nf = 0, n = 0, no = 0;
	int i = 0;
	int r;
	int ret = FALSE;
	
	for (i=0; i < gblOps.nlfolders; i++) {
		n = getFileList(gblOps.langDirs[i], &buf);
		sumStringTabs(&dirs, nf, buf, n);
		nf = sumStringTabs_Cat(&fullpaths, nf, buf, n, gblOps.langDirs[i]);
		free(buf);
		buf = NULL;
	}
	
	no = 0;
	for (i = 0; i < nf; i++) {
		if (checkExtension(fullpaths[i], LANGEXT))
			str = getLangComment(fullpaths[i]);
		else str = NULL;
		
		if (str != NULL) {
			no++;
			
			index = realloc(index, sizeof(int)*no);
			tips = realloc(tips, sizeof(char*)*no);
			options = realloc(options, sizeof(char*)*no);
			
			tips[no-1] = str;
			options[no-1] = dirs[i];
			index[no-1] = i;
		}
	}

	r = playMenuTab(gfx,-no, options, tips);
	if (r >= 0 && r < no) {
		gblOps.langFile = realloc(gblOps.langFile, sizeof(char)* (strlen( fullpaths[index[r]] )+1) );
		strcpy(gblOps.langFile, fullpaths[index[r]]);
		freeLanguage(gfx);
		loadLanguage(gfx, gblOps.langFile);
		ret = TRUE;
	}
	
	for (i = 0; i < nf; i++) {
		free(dirs[i]);
		free(fullpaths[i]);
	}
	for (i = 0; i< no; i++) {
		free(tips[i]);
	}
	
	free(tips);
	free(options);
	free(index);
	free(dirs);
	free(fullpaths);
	
	return ret;*/ return 0;
}

void
chooseThemeMenu (data_t * gfx)
{
  char **options = NULL;
  char **tips = NULL;
  char **dirs = NULL;
  char **buf = NULL;
  char **fullpaths = NULL;
  int *index = NULL;

  char *str = NULL;
  int nf = 0, n = 0, no = 0;
  int i = 0;
  int r;

  for (i = 0; i < gblOps.ntfolders; i++)
    {
      n = getDirList (gblOps.themeDirs[i], &buf);
      sumStringTabs (&dirs, nf, buf, n);
      nf = sumStringTabs_Cat (&fullpaths, nf, buf, n, gblOps.themeDirs[i]);
      free (buf);
      buf = NULL;
    }

  no = 0;
  for (i = 0; i < nf; i++)
    {
      str = getThemeComment (fullpaths[i]);
      if (str != NULL)
	{
	  no++;

	  index = realloc (index, sizeof (int) * no);
	  tips = realloc (tips, sizeof (char *) * no);
	  options = realloc (options, sizeof (char *) * no);

	  tips[no - 1] = str;
	  options[no - 1] = dirs[i];
	  index[no - 1] = i;
	}
    }

  r = playMenuTab (gfx, -no, options, tips);
  if (r >= 0 && r < no)
    {
      gblOps.dataDir = realloc (gblOps.dataDir,
				sizeof (char) *
				(strlen (fullpaths[index[r]]) + 1));
      strcpy (gblOps.dataDir, fullpaths[index[r]]);
      freeGraphics (gfx);
      loadGraphics (gfx, gblOps.dataDir);
      if (gfx->musmenu)
	Mix_PlayMusic (gfx->musmenu, -1);
    }

  for (i = 0; i < nf; i++)
    {
      free (dirs[i]);
      free (fullpaths[i]);
    }
  for (i = 0; i < no; i++)
    {
      free (tips[i]);
    }

  free (tips);
  free (options);
  free (index);
  free (dirs);
  free (fullpaths);
}


int
manageDirsMenu (data_t * gfx, char ***folders, int nfolders)
{
  char **options = NULL;
  char **tips = NULL;
  char *buf = NULL;
  int i;
  int done = FALSE;
  int opt;

  while (!done)
    {
      options = malloc (sizeof (char *) * (nfolders + 2));
      tips = malloc (sizeof (char *) * (nfolders + 2));

      options[0] = _("Add");
      options[nfolders + 1] = _("Back");
      tips[0] = _("Add a folder to the list.");
      tips[nfolders + 1] = _("Leave this menu.");

      for (i = 0; i < nfolders; i++)
	{
	  options[i + 1] = (*folders)[i];
	  tips[i + 1] = _("View more options about this folder.");
	}

      opt = playMenuTab (gfx, -(nfolders + 2), options, tips);

      if (opt == 0)
	{			/* Add a new item */
	  nfolders++;
	  *folders = realloc (*folders, sizeof (char *) * nfolders);

	  buf = getcwd (NULL, 0);
	  (*folders)[nfolders - 1] =
	    inputMenu (gfx, _("Write a full or relative path."), buf,
		       gfx->menuW);

	  free (buf);
	  buf = NULL;
	}
      else if (opt > 0 && opt < nfolders + 1)
	{
	  /* Display the menu to edit an entry */
	  switch (playMenu (gfx, -3,
			    _("Modify"), _(""),
			    _("Delete"), _(""),
			    _("Back"), _("Leave this menu.")))
	    {
	    case 0:		/* edit */
	      buf = (*folders)[opt - 1];
	      (*folders)[opt - 1] =
		inputMenu (gfx, _("Write a full or relative path."), buf,
			   256);
	      free (buf);
	      break;
	    case 1:		/* delete */
	      buf = (*folders)[opt - 1];
	      for (i = opt; i < nfolders; i++)
		{
		  (*folders)[i - 1] = (*folders)[i];
		}
	      free (buf);
	      buf = NULL;
	      nfolders--;
	      *folders = realloc ((*folders), sizeof (char *) * nfolders);
	      break;
	    default:
	      break;
	    }

	}
      else
	{
	  done = TRUE;
	}

      free (options);
      free (tips);
    }

  return nfolders;
}
