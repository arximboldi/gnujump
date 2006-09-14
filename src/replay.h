/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolívar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * replay.h
 */

/*
    Copyright (C) 2003-2004, Juan Pedro Bolivar Puente

    SDLjump is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    SDLjump is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SDLjump; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _REPLAY_H_
#define _REPLAY_H_

/* replay_t defined in game.h */
#include "game.h"

#define REP_BUFFER_SIZE 500000
#define REP_MIN_FREE 500

enum {
	S_JUMP = 1,
	S_FALL = 2,
	S_DIE = 4
};

/*  
	REPLAY FORMAT:
	==============
	-- HEADER (not stored in the memory buffer) --
	 * Replay file format (Uint32)
	 * Comment (Char until '\0')
	 * Replay FPS (Uint32)
	 * Number of frames (Uint32)
	-- BODY --
	 * Grid data. For every row in descending order:
		floor_l (Uint8)
		floor_r (Uint8)
	 * Number of players (Uint8)
	 * Player data. For every player:
		x (Uint16)
		y (Uint16)
		dir (Uint8)
		rot (Uint16)
		action (Uint8)
		dead (Uint8)
		lives (Uint8)
		floor (Uint16)
	 * For every frame:
	  + sounds played in this frame
	  + scrollCount (Uint8)
	  + Number of scrolls since last frame (Uint8):
	  + For every sroll:
	    floor_l (Uint8)
		floor_r (Uint8)
	  + Player data. For every player:
		x (Uint16)
		y (Uint16)
		dir (Uint8)
		rot (Uint16)
		action (Uint8)
		dead (Uint8)
		lives (Uint8)
		floor (Uint16)
*/

void repPushUInt32(replay_t* rep, Uint32 data);

void repPushUInt16(replay_t* rep, Uint16 data);

void repPushUInt8(replay_t* rep, Uint8 data);

Uint32 repGetUInt32(replay_t* rep);

Uint16 repGetUInt16(replay_t* rep);

Uint8 repGetUInt8(replay_t* rep);

void repPushHeros(game_t* game);

void repPushGrid(game_t* game);

void initReplay(game_t* game);

void updateReplay(game_t* game, float ms);

void freeReplay(replay_t* rep);

void endReplay(replay_t* rep);

void getPlayerReplay(hero_t* hero, replay_t* rep);

void initGameReplay(game_t* game, data_t* gfx, replay_t* rep);

void updateGameReplay(game_t* game, data_t* gfx, replay_t* rep, float ms);

int playReplay(data_t* gfx, replay_t* rep);

int saveReplay(replay_t* rep, char* fname, char* comment);

void scrollReplay(game_t* game, data_t* gfx, replay_t* rep);

char* getReplayComment(char* file);

void freeGameReplay(game_t* game);

int loadReplay(data_t* gfx, char *file);

#endif /* _REPLAY_H_ */

