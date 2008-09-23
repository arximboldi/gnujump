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

#ifndef _GAME_H_
#define _GAME_H_

/* What to return when the player is dead */
#define DEAD -1

#define PAUSED 2
#define ENDMATCH 2

#define MSGTIME 300

/* Size of the gaming grid (it has 1 row more at the begining) */
#define GRIDWIDTH  32
#define GRIDHEIGHT 24

/* Size of a square in the grid*/
#define BLOCKSIZE 16
#define HEROSIZE 32

/* The minimum space on top*/
#define MINSKY 80
#define MINSKYGRID 6

/* Space (in grid squares) between one floor and another */
#define FLOORGAP 5

/* Sounds */
enum
{
  S_JUMP = 1,
  S_FALL = 2,
  S_DIE = 4
};


typedef struct key
{
  Sint8 pressed;
  int def;
} L_key;

typedef struct timer
{
  Uint32 framecount;
  float rateticks;
  Uint32 lastticks;
  float ms;
  Uint32 mscount;
  Uint32 rate;
  Uint32 totalms;
  Uint32 notdelayed;
  int ratechanged;
} L_timer;

typedef struct
{
  void *buf;
  void *bst;
  int bufsize;
  int bodysize;
  int nframes;
  int fps;
  int totalms;
  int record;

  int speed;
  int deadHero[MAX_PLAYERS];
  int scrolls;
  int sounds;
  float timer;
  float mspf;
} replay_t;

typedef struct trail_s
{
  float x0, y0;
  float x1, y1;
  int alpha;
  struct trail_s *next;
} trail_t;

typedef struct blur_s
{
  JPB_surfaceRot *pic;
  float x, y;
  int angle;
  int alpha;
  struct blur_s *next;
} blur_t;

typedef struct particle_s
{
  JPB_surfaceRot *pic;
  float x, y;
  float vx, vy;
  float ax, ay;
  int rot;
  int rv;
  int alpha;
  int av;

  struct particle_s *next;
} particle_t;

typedef struct hero
{
  trail_t *trail;
  blur_t *blur;

  L_spriteCtlRot sprite[HEROANIMS];
  L_key up;
  L_key left;
  L_key right;
  float x;
  float y;
  float vx;
  float vy;
  float jump;
  int id;
  int previd;
  int dir;
  int st;

  float angle;
  float rotateto;
  float angspeed;

  int floor;
  int prevFloor;
  int dead;
  int lives;
  int prevLives;
} hero_t;

typedef struct game
{
  int floorTop;
  int mapIndex;
  double scrollTotal;
  double scrollCount;
  int floor_l[GRIDHEIGHT];
  int floor_r[GRIDHEIGHT];

  hero_t *heros;
  int numHeros;
  int deadHeros;

  float T_speed;
  float T_count;
  float T_timer;

  replay_t replay;
} game_t;

void drawCredits (data_t * gfx);

void softScrollUp (game_t * game, float scroll);

int drawBg (JPB_surface * surf, int x, int y, int w, int h);

void drawAnimatedSquare (data_t * gfx, Uint32 color, Uint8 alpha, int x,
			 int y, int w, int h, int time);

void drawScore (data_t * gfx, game_t * game, Uint32 currtime);

int drawFloor (data_t * gfx, int x, int y, int bw);

void makeFloor (game_t * game, int y);

void hardScrollUp (game_t * game);

void scrollGrid (game_t * game);

void initGame (game_t * game, data_t * gfx, int numHeros);

void playHeroSounds (data_t * gfx, hero_t * hero);

int playGame (data_t * gfx, int numHeros);

int pauseGame (data_t * gfx, game_t * game, char *text);

int endMatch (data_t * gfx, game_t * game, int time);

void playHeroSound (data_t * gfx, int sound, replay_t * rep);

void continueTimer (L_timer * time);

int yesNoQuestion (data_t * gfx, game_t * game, char *text);

int updateInput ();

void freeGame ();

void drawGame (data_t * gfx, game_t * game);

void markHeroKeys (SDL_Event * event, hero_t * hero);

void unmarkHeroKeys (SDL_Event * event, hero_t * hero);

void initHeroKeys (hero_t * hero, int num);

int isFloor (game_t * game, int x, int y);

void scrollHeros (game_t * game, float scroll);

int isStand (game_t * game, int ix, int iy);

int updateHero (game_t * game, data_t * gfx, int num, float ms);

void reliveHero (game_t * game, int num);

int updateHeroPosition (game_t * game, int num, float fact);

int updateGame (game_t * game, data_t * gfx, float ms);

void drawHero (data_t * gfx, hero_t * hero);

void initTimer (L_timer * time, int rate);

Uint32 updateTimer (L_timer * time);

void rotateHero (hero_t * hero, float ms);

void updateScore (data_t * gfx, game_t * game, Uint32 currtime);

void drawRecords (data_t * gfx, records_t * rtab, int hl);

void setFpsTimer (L_timer * time, int rate);

#endif //_GAME_H_
