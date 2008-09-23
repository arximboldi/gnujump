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

#ifndef _GNUJUMP_H_
#define _GNUJUMP_H_

/*
 * HEADERS
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_opengl.h>

#include "gettext.h"
#define _(str) gettext(str)

#include "surface.h"
#include "sprite.h"
#include "SFont.h"

/*
 DEFINITIONS
*/

/* Me */
#define AUTHOR "Juan Pedro Bolivar Puente"

/* Config, theme and language files format version */
#define PROT_VERS "03"
#define THEME_VERS "03"
#define SOUND_VERS "01"
#define LANG_VERS  "01"
#define REP_VERS 3

#ifndef HAVE_CONFIG_H
#define VERSION "1.0.5"
#endif

/* Some code switchs */
//#define DEVEL
#define GLFINISH

/* Files */
#define THEMEFILE "/config.theme"
#define SOUNDFILE "/config.sounds"
#define DEFTHEME "default"
#define CFGFILE "gnujump.cfg"
#define HSCFILE "gnujump.hsc"
#define REPEXTOLD ".rep"
#define REPEXT ".gjr"
#define CONFDIR ".gnujump"

#ifdef WIN32
#define DEFSOUND "sound"
#else
#ifdef DEVEL
#define DEFSOUND "sound"
#else
#define DEFSOUND DATA_PREFIX"/"PACKAGE"/sound"
#endif
#endif

#define MAX_CHAR 512

#define TRUE 1
#define FALSE 0
#define ON 1
#define OFF 0

/* Hero rotation modes */
enum
{
  ROTNONE,
  ROTORIG,
  ROTFULL
};

/* Scrolling modes */
enum
{
  HARDSCROLL,
  SOFTSCROLL
};

enum
{
  FPS40,
  FPS100,
  FPS300,
  FPSNOLIMIT
};

enum
{
  BPP32,
  BPP16,
  BPP8,
  BPPAUTO
};

enum
{
  NOTRAIL,
  THINTRAIL,
  NORMALTRAIL,
  STRONGTRAIL
};

#define MAXBLUR 9

/* Player states*/
enum
{
  H_STAND, H_WALK, H_JUMP, HEROANIMS
};

/* Mouse states*/
enum
{
  M_IDLE, M_OVER, M_DOWN, M_STATES
};


/* Maximun number of players */
#define MAX_PLAYERS 4

/* The number of records */
#define MAX_RECORDS 10

/* Be careful, these values are not arbitrary */
#define RIGHT 1
#define LEFT  0

/* Delay and rate in original mode*/
#define DELAY   25
#define FPSRATE 40


/* Keys ids */
enum
{
  LEFTK,
  RIGHTK,
  JUMPK,
  KEYS
};

/* Default keys */
#define KEY_QUIT SDLK_ESCAPE

#define KEY_LEFT1 SDLK_LEFT
#define KEY_LEFT2 SDLK_a
#define KEY_LEFT3 SDLK_j
#define KEY_LEFT4 SDLK_KP4

#define KEY_RIGHT1 SDLK_RIGHT
#define KEY_RIGHT2 SDLK_d
#define KEY_RIGHT3 SDLK_l
#define KEY_RIGHT4 SDLK_KP6

#define KEY_UP1 SDLK_UP
#define KEY_UP2 SDLK_w
#define KEY_UP3 SDLK_i
#define KEY_UP4 SDLK_KP8

/* Default player name */
#define PNAME "Player"

#define MIN(a,b) ((a) < (b) ? (a) : (b))

/*
  TYPES
*/

/*
 * A record entry
 */
typedef struct
{
  char *pname;
  int floor;
  char *mode;
  int time;
  char date[64];
} records_t;

/*
 * Global Options
 */
typedef struct
{
  int useGL;
  GLint texFilter;		/* use GL_NEAREST or GL_LINEAR */
  int aa;
  int w;
  int h;
  int bpp;
  int fullsc;
  int sndvolume;
  int musvolume;

  int fps;
  int rotMode;
  int scrollMode;
  int scrollBg;
  int trailMode;
  int blur;
  int mpLives;
  int nplayers;
  int recReplay;
  int repFps;

  /* Players */
  SDLKey keys[MAX_PLAYERS][KEYS];
  char *pname[MAX_PLAYERS];

  /* Data files */
  char *dataDir;
  char **themeDirs;
  char *repDir;
  char **repDirs;
  int nrfolders;
  int ntfolders;

  /* The records tab, organized from best to worst */
  records_t records[MAX_RECORDS];
} L_gblOptions;

/*
 * The theme data structure
 */
typedef struct
{
  /* Sound */
  Mix_Chunk *gjump;
  Mix_Chunk *gfall;
  Mix_Chunk *gdie;
  Mix_Chunk *grecord;
  Mix_Chunk *gquestion;
  Mix_Chunk *mclick;
  Mix_Chunk *mback;
  Mix_Music *musmenu;
  Mix_Music *musgame;
  int soundloaded;

  /* Mouse */
  L_spriteData *mouse[M_STATES];
  int mouseX;
  int mouseY;

  /* Menu */
  JPB_surface *menuBg;
  JPB_surface *upArrow;
  JPB_surface *dwArrow;
  int mUpArrowX;
  int mDwArrowX;
  int mUpArrowY;
  int mDwArrowY;
  int menuX;
  int menuY;
  int menuW;
  int menuH;
  int mMaxOps;
  int mMargin;
  int tipX;
  int tipY;
  int tipW;
  int tipH;
  int mAlign;
  int tAlign;
  SFont_Font *menufont;
  SFont_Font *tipfont;
  Uint32 hlcolor;
  Uint8 hlalpha;

  /* Players */
  L_spriteDataRot *heroSprite[MAX_PLAYERS][HEROANIMS];
  Uint8 tcolorr[MAX_PLAYERS];
  Uint8 tcolorg[MAX_PLAYERS];
  Uint8 tcolorb[MAX_PLAYERS];

  /* In game screen */
  JPB_surface *gameBg;
  JPB_surface *livePic;
  int liveAlign;
  int gameX;
  int gameY;
  int gameTileH;
  int borderTileH;
  int borderTileW;
  int scoreX[MAX_PLAYERS];
  int scoreY[MAX_PLAYERS];
  int livesX[MAX_PLAYERS];
  int livesY[MAX_PLAYERS];
  int timeX;
  int timeY;
  SFont_Font *scorefont;
  SFont_Font *textfont;
  SFont_Font *timefont;
  Uint32 gcolor;
  Uint8 galpha;

  JPB_surface *floorL;
  JPB_surface *floorR;
  JPB_surface *floorC;

  /* Credits */
  char *gfxauth;
  char *sndauth;
  char *langauth;
} data_t;


#endif //_GNUJUMP_H_
