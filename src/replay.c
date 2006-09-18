/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolíar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * replay.c
 */

/*
    Copyright (C) 2003-2006, Juan Pedro Bolivar Puente

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

#include "sdljump.h"
#include "replay.h"
#include "game.h"
#include "tools.h"

extern L_gblOptions gblOps;

void repPushUInt32(replay_t* rep, Uint32 data)
{
	*(Uint32*)(rep->buf) = data;
	rep->buf += sizeof(Uint32);
}

void repPushUInt16(replay_t* rep, Uint16 data)
{
	*(Uint16*)(rep->buf) = data;
	rep->buf += sizeof(Uint16);
}

void repPushUInt8(replay_t* rep, Uint8 data)
{
	*(Uint8*)(rep->buf) = data;
	rep->buf += sizeof(Uint8);
}

Uint32 repGetUInt32(replay_t* rep)
{
	rep->buf += sizeof(Uint32);
	return *(Uint32*)(rep->buf - sizeof(Uint32));
}

Uint16 repGetUInt16(replay_t* rep)
{
	rep->buf += sizeof(Uint16);
	return *(Uint16*)(rep->buf - sizeof(Uint16));
}

Uint8 repGetUInt8(replay_t* rep)
{
	rep->buf += sizeof(Uint8);
	return *(Uint8*)(rep->buf - sizeof(Uint8));
}

void endReplay(game_t* game)
{
	updateReplay(game, 0);
	game->replay.bodysize = game->replay.buf - game->replay.bst;
}

void initReplay(game_t* game)
{
	int i;
	
	game->replay.timer = 0;
	game->replay.scrolls = 0;
	game->replay.sounds = 0;
	game->replay.bufsize = REP_BUFFER_SIZE;
	game->replay.bst = malloc(game->replay.bufsize);
	game->replay.buf = game->replay.bst;
	game->replay.nframes = 0;
	game->replay.fps = gblOps.repFps;
	game->replay.mspf = 1000.0/gblOps.repFps;
	
	for (i = 0; i < game->numHeros; i++)
		game->replay.deadHero[i] = 0;
	 
	for (i = 0; i < GRIDHEIGHT; i++) {
		repPushUInt8(&(game->replay), game->floor_l[i]);
		repPushUInt8(&(game->replay), game->floor_r[i]);
	}
	repPushUInt8(&(game->replay), game->numHeros);
	repPushHeros(game);
}

void updateReplay(game_t* game, float ms)
{
	int d = game->replay.buf - game->replay.bst;
	
	if (game->replay.bufsize - d < REP_MIN_FREE) {
		game->replay.bufsize += REP_BUFFER_SIZE;
		game->replay.bst = realloc(game->replay.bst, game->replay.bufsize);
		game->replay.buf = game->replay.bst+d;
	}
	
	game->replay.timer += ms;
	
	if (game->replay.timer >= game->replay.mspf)  {
		game->replay.timer -= game->replay.mspf;
		game->replay.nframes++;
		repPushUInt8(&(game->replay), game->replay.sounds);
		game->replay.sounds = 0;
		repPushGrid(game);
		repPushHeros(game);
	}
}

void repPushGrid(game_t* game)
{
	int i, j;
	
	repPushUInt8(&(game->replay), game->scrollCount);
	repPushUInt8(&(game->replay), game->replay.scrolls);
	for (i = 0, j = game->mapIndex;
		i < game->replay.scrolls;
		i++, j = (j+1)%GRIDHEIGHT) {
		repPushUInt8(&(game->replay), game->floor_l[j]);
		repPushUInt8(&(game->replay), game->floor_r[j]);
	}
	
	game->replay.scrolls = 0;
}

void repPushHeros(game_t* game)
{
	int i;
	for (i = 0; i < game->numHeros; i++) {
		if (! game->replay.deadHero[i]) {
			repPushUInt16(&(game->replay), game->heros[i].x);
			repPushUInt16(&(game->replay), game->heros[i].y);
			repPushUInt8(&(game->replay), game->heros[i].dir);
			repPushUInt16(&(game->replay), game->heros[i].angle);
			repPushUInt8(&(game->replay), game->heros[i].id);
			repPushUInt8(&(game->replay), game->heros[i].dead);
			repPushUInt8(&(game->replay), game->heros[i].lives);
			repPushUInt16(&(game->replay), game->heros[i].floor);
			
			game->replay.deadHero[i] = game->heros[i].dead;
		}
	}
}

void freeReplay(replay_t* rep)
{
	free(rep->bst);
}

void getPlayerReplay(hero_t* hero, replay_t* rep)
{
	int newid;
	hero->x = repGetUInt16(rep);
	hero->y = repGetUInt16(rep);
	hero->dir = repGetUInt8(rep);
	hero->angle = repGetUInt16(rep);
	newid = repGetUInt8(rep);
	hero->previd = hero->id;
	if (newid != hero->id) {
		hero->sprite[newid].elpTime = hero->sprite[newid].frame = 0;
	}
	hero->id = newid;
	hero->dead = repGetUInt8(rep);
	hero->prevLives = hero->lives;
	hero->lives = repGetUInt8(rep);
	if (hero->lives > 4) hero->lives = -1;
	hero->prevFloor = hero->floor;
	hero->floor = repGetUInt16(rep);
}

void initGameReplay(game_t* game, data_t* gfx, replay_t* rep)
{
    int i,j;
    
    rep->buf = rep->bst;

    game->floorTop = GRIDHEIGHT - 4;
    game->mapIndex = 0;
	game->scrollCount = 0;
    
    for (i = 0; i < GRIDHEIGHT; i++) {
		game->floor_l[i] = repGetUInt8(rep);
		game->floor_r[i] = repGetUInt8(rep);
    }
    game->numHeros = repGetUInt8(rep);
    game->heros = malloc(sizeof(hero_t)*game->numHeros);
    for (i = 0; i < game->numHeros; i++) {
    	for (j=0; j<HEROANIMS; j++) {
    		initializeSpriteCtlRot(&game->heros[i].sprite[j], gfx->heroSprite[i][j]);
    	}
    	getPlayerReplay(&(game->heros[i]), rep);
        game->heros[i].prevLives = -1;
    }
}

void scrollReplay(game_t* game, data_t* gfx, replay_t* rep)
{
	int newScrollCount;
	int scrolls;
	int y, x, width, i, j;
	
	newScrollCount = repGetUInt8(rep);
	scrolls = repGetUInt8(rep);
	if (newScrollCount != game->scrollCount || scrolls) {
		for ( y = game->floorTop % 5 ; y < GRIDHEIGHT ; y += 5 ){
			x = game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ];
			width = game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] -x+1;
	
			drawBg(gfx->gameBg, x*BLOCKSIZE + gfx->gameX,
								y*BLOCKSIZE+game->scrollCount-1 + gfx->gameY , 
								width*BLOCKSIZE, BLOCKSIZE+1);
		}
		
		game->mapIndex -= scrolls;
		if (game->mapIndex < 0) game->mapIndex += GRIDHEIGHT;
		game->scrollCount = newScrollCount;
		
		game->floorTop += scrolls;
		for (i = game->mapIndex, j = 0; j < scrolls; j++, i = (i+1)%GRIDHEIGHT) {
			game->floor_l[i] = repGetUInt8(rep);
			game->floor_r[i] = repGetUInt8(rep);
		}
		
		for ( y = game->floorTop % 5 ; y < GRIDHEIGHT ; y += 5 ) {
			x = game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ];
			width = game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] -x+1;
			if( y*BLOCKSIZE+game->scrollCount < (GRIDHEIGHT-1)*BLOCKSIZE)
				drawFloor(gfx,x, y*BLOCKSIZE+game->scrollCount, width);
		}
	}
	game->scrollCount = newScrollCount;
}

void playRepSounds(data_t* gfx, replay_t* rep)
{
	int sounds = repGetUInt8(rep);
	if ((sounds & S_JUMP) == S_JUMP) Mix_PlayChannel(-1, gfx->gjump, 0);
	if ((sounds & S_FALL) == S_FALL) Mix_PlayChannel(-1, gfx->gfall, 0);
	if ((sounds & S_DIE) == S_DIE) Mix_PlayChannel(-1, gfx->gdie, 0);
}

void updateGameReplay(game_t* game, data_t* gfx, replay_t* rep, float ms)
{
    int i;
    
    for (i=0; i<game->numHeros; i++) {
		undrawHero(i, gfx, game);
    }
    
    playRepSounds(gfx, rep);
    scrollReplay(game, gfx, rep);
    for (i=0; i<game->numHeros; i++) {
		if (game->heros[i].dead == FALSE) {
			getPlayerReplay(&(game->heros[i]), rep);
			animateSpriteRot(&(game->heros[i].sprite[game->heros[i].id]), ms);
			drawHero(gfx, &(game->heros[i]));
		}
    }

}

int updateInputReplay()
{
    int done = 0;
    SDL_Event event;
    
    while( SDL_PollEvent( &event ) ){
        switch( event.type ){
            /* A key is pressed */
            case SDL_KEYDOWN: 
                if( event.key.keysym.sym == KEY_QUIT){
                    done = TRUE;
                }
				if( event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_PAUSE) {
                    done = PAUSED;
                }
                break;
            /* Quit: */
            case SDL_QUIT:
                done = TRUE;
                break;
            /* Default */
            default:
                break;
        }
    }
    
    return done;
}

int playReplay(data_t* gfx, replay_t* rep)
{
	L_timer timer;
    game_t game;
	int done = FALSE;
	int r, i = 0;
    
    Mix_PlayMusic(gfx->musgame, -1);
    
    drawBg(gfx->gameBg,0,0,gblOps.w,gblOps.h);
    initGameReplay(&game, gfx, rep);
	initTimer(&timer, rep->fps);
    FlipScreen();

	updateTimer(&timer);
    while(!done) {
		if ((r = updateInputReplay(&game))) {
			if (r == PAUSED ) {
				done = pauseGame(gfx, &game, gfx->txt[txt_pause]);
			} else  {
				done = yesNoQuestion(gfx, &game, gfx->txt[txt_askquitrep]);
			}
			continueTimer(&timer);
		}
		
        updateTimer(&timer);
        updateGameReplay(&game, gfx, rep, timer.ms);
			
        updateScore(gfx, &game, timer.totalms);
        FlipScreen();
        
		if (++i == rep->nframes) done = ENDMATCH;
    }
	if (done == ENDMATCH) {
		r = yesNoQuestion(gfx, &game, gfx->txt[txt_askrepagain]);
	} else {
		r = FALSE;
	}
	
    freeGameReplay(&game);
	
	return r;
}

void freeGameReplay(game_t* game)
{
    free(game->heros);
}

void fputUint32(Uint32 data, FILE* fh)
{
	fputc(data >> 24,fh);
	fputc((data << 8) >> 24,fh);
	fputc((data << 16) >> 24,fh);
	fputc((data << 24) >> 24,fh);
}

int saveReplay(replay_t* rep, char* fname, char* comment)
{
	char* fullname = NULL;
	FILE* fh = NULL;
	int i;
	
	fullname = malloc(sizeof(char*)*(strlen(fname)+strlen(gblOps.repDir)+strlen(REPEXT)+2));
	sprintf(fullname, "%s/%s%s", gblOps.repDir, fname, REPEXT);
	
	if ((fh = fopen(fullname,"wb")) == NULL) {
		fprintf(stderr, "ERROR: Could not save replay to %s\n", fullname);
		free(fullname);
		return FALSE;
	}
	
	/* Header */
	fputUint32(REP_VERS, fh);
	fputs(comment, fh);
	fputc('\0',fh);
	fputUint32(rep->fps, fh);
	fputUint32(rep->nframes, fh);
	
	/* Body */
	for (i = 0; i < rep->bodysize; i++) {
		fputc(((char*)(rep->bst))[i], fh);
	}
	
	free(fullname);
	
	fclose(fh);
	
	return TRUE;
}

Uint32 fgetUint32(FILE* fh)
{
	Uint32 data = 0;
	data |= fgetc(fh) << 24;	
	data |= fgetc(fh) << 16;
	data |= fgetc(fh) << 8;
	data |= fgetc(fh);
	return data;
}

int loadReplay(data_t* gfx, char *file)
{
	FILE* fh = NULL;
	int version;
	int size;
	replay_t rep;
	
	size = getFileSize(file);
	
	if ((fh = fopen(file,"rb")) == NULL) {
		fprintf(stderr, "ERROR: Could not open replay %s\n", file);
		return FALSE;
	}
	
	version = fgetUint32(fh);
	if (version != REP_VERS) {
		fprintf(stderr, "ERROR: Replay %s is of a different version.\n", file);
		return FALSE;
	}
	
	while(fgetc(fh) != '\0');
	
	rep.fps = fgetUint32(fh);
	rep.nframes = fgetUint32(fh);
	rep.buf = rep.bst = malloc(size);
	rep.bufsize = size;
	
	for(rep.bodysize = 0; !feof(fh); rep.bodysize++) {
		((char*)(rep.buf))[ rep.bodysize ] = fgetc(fh);
	}
	
	fclose(fh);
	
	
	printf("Replay loaded: SIZE: %d FPS: %d NFRAMES: %d\n", size, rep.fps, rep.nframes);
	while(playReplay(gfx, &rep));
	
	freeReplay(&rep);
	
	return TRUE;
}

char* getReplayComment(char* file)
{
	FILE* fh = NULL;
	int version;
	char* comment = NULL;
	int i;
	
	if ((fh = fopen(file,"rb")) == NULL) {
		fprintf(stderr, "ERROR: Could not open replay %s\n", file);
		return NULL;
	}
	
	version = fgetUint32(fh);
	if (version != REP_VERS) {
		fprintf(stderr, "ERROR: Replay %s is of a different version.\n", file);
		return NULL;
	}
	
	i = 0;
	comment = malloc(sizeof(char)*MAX_CHAR);
	while ((comment[i++] = fgetc(fh)) != '\0');
	
	return comment;
}
