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


#ifndef _MENU_H_
#define _MENU_H_

#include "sprite.h"
#include "game.h"

#define MENUFADE 1000
#define ABLINKTIME 300
#define TIPFADE 0
/* If larger than 0 too slow in software mode */

#define NONE -1

enum
{
  KIDLE,
  KUP,
  KDOWN,
  KENTER,
  KLEFT,
  KRIGHT,
  KMUP,
  KMDOWN,
  KMENTER,
  KBACK
};

enum
{
  A_DOWN,
  A_UP,
  ARROWS
};

enum
{
  MB_INPUT = 1,
  MB_RETURN = 2,
  MB_KEYDEF = 4,
  MB_CHOOSE = 8,
  MB_VOLSET = 16,
};

typedef struct option
{
  char *caption;
  char *tip;

  int flags;
  int nops;
  char **opcap;
  void *data;
}
opt_t;

typedef struct
{
  int id;
  int x;
  int y;
  int clicked;
  L_spriteCtl sprite[M_STATES];
}
mouse_t;

typedef struct menu
{
  int nops;
  opt_t *opt;
}
menu_t;

/* NEW MENUS */
void initMenuT (menu_t * menu);

void addMenuTOption (menu_t * menu, char *caption, char *tip, int flags,
		     void *data, int nops, ...);

void freeMenuTOption (opt_t * opt);

void freeMenuT (menu_t * menu);

int playMenuT (data_t * gfx, menu_t * menu);

void drawMenuTArrows (data_t * gfx, int alphaUp, int alphaDown);

void drawMenuT (data_t * gfx, menu_t * menu, int offset);

void drawMenuTOption (data_t * gfx, int opt, int offset, opt_t * option,
		      int alpha);

char *inputMenu (data_t * gfx, char *tip, char *inittext, int maxWidth);

void undrawTip (data_t * gfx);

/* OLD MENUS*/
int playMenu (data_t * gfx, int nops, ...);

int playMenuTab (data_t * gfx, int nops, char **options, char **tips);

void drawTip (data_t * gfx, char *tip);

int checkMenuKeys (mouse_t * mouse);

void drawMenu (data_t * gfx, int nops, char **ops);

void drawOption (data_t * gfx, int opt, char *option, int alpha);

/* ACTUAL MENUS */
void saveReplayMenu (data_t * gfx, replay_t * rep);

void mainMenu (data_t * gfx);

void newGameMenu (data_t * gfx);

void configurePlayersMenu (data_t * gfx);

void optionsMenu (data_t * gfx);

void folderOptionsMenu (data_t * gfx);

void soundOptionsMenu (data_t * gfx);

void gfxOptionsMenu (data_t * gfx);

void gameOptionsMenu (data_t * gfx);

void themeMenu (data_t * gfx);

void viewReplayMenu (data_t * gfx);

void chooseThemeMenu (data_t * gfx);

int chooseLangMenu (data_t * gfx);

int manageDirsMenu (data_t * gfx, char ***folders, int nfolders);

#endif //_MENU_H_
