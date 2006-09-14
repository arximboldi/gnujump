/*
 * SDLjump
 * (C) 2005 Juan Pedro Bolíar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * game.c
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

#include "sdljump.h"
#include "game.h"
#include "tools.h"
#include "records.h"
#include "replay.h"
#include "menu.h"

extern L_gblOptions gblOps;
extern SDL_Surface * screen;

int drawBg(JPB_surface* surf, int x, int y, int w, int h)
{
    SDL_Rect rect;
    rect.x = x;
    rect.h = h;
    rect.w = w;
    rect.y = y;
    JPB_PrintSurface(surf, &rect, &rect);
    return TRUE;
}

int drawFloor(data_t* gfx, int x, int y, int bw)
{
    SDL_Rect rect;
    int j;

    rect.x = x*BLOCKSIZE + gfx->gameX;
    rect.y = y + gfx->gameY;
    
    JPB_PrintSurface(gfx->floorL, NULL, &rect);
      
    for (j = 1; j < bw-1; j++) {
        rect.x = j*BLOCKSIZE + x*BLOCKSIZE + gfx->gameX;
        JPB_PrintSurface(gfx->floorC, NULL, &rect);
    }
    
    rect.x = j*BLOCKSIZE + x*BLOCKSIZE + gfx->gameX;
    JPB_PrintSurface(gfx->floorR, NULL, &rect);
    
    return TRUE;
}

void makeFloor(game_t* game, data_t* gfx, int y)
{
    int f,x1,x2,fvec;
    static int fpos = GRIDWIDTH/2;

    f = game->floorTop - y;

    if( f % 250 == 0 ) { /* If Floor % 50 = 0, you can relax */
        game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ] = 1;
        game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] = GRIDWIDTH -2;
        drawFloor(gfx, 1, y*BLOCKSIZE+game->scrollCount, GRIDWIDTH-2);
        return;
    }
    if( f % 5 == 0 ) {  /* Each five blocks, we create a floor */

        fvec = (rnd(2)*2-1) * ( rnd( (GRIDWIDTH-11)/4 ) + (GRIDWIDTH-11)/4 );
        fpos = ( fpos + fvec + GRIDWIDTH-11 ) % (GRIDWIDTH-11);
    
        x1 = ( fpos+5 - rnd( 3 ) - 2);
        x2 = ( fpos+5 + rnd( 3 ) + 2);

        game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ] = x1;
        game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] = x2;
        drawFloor(gfx, x1, y*BLOCKSIZE+game->scrollCount, x2-x1+1);
    } else {
        game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ] = GRIDWIDTH;
        game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] = -1;
    }
}

void hardScrollUp(game_t* game, data_t* gfx)
{
    int x,width,y;

    for( y = game->floorTop % 5 ; y < GRIDHEIGHT ; y += 5 ){
        x = game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ];
        width = game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] -x+1;

        drawBg(gfx->gameBg, x*BLOCKSIZE + gfx->gameX,
                            y*BLOCKSIZE + gfx->gameY , 
                            width*BLOCKSIZE, BLOCKSIZE);

        if( y < GRIDHEIGHT-1 )
            drawFloor(gfx, x,(y+1)*BLOCKSIZE,width);
    }
	
	scrollGrid(game, gfx);
    scrollHeros(game, BLOCKSIZE);
}

void softScrollUp(game_t* game, data_t* gfx, float scroll)
{
    int y,x,width;
    
    game->scrollCount += scroll;

    for( y = game->floorTop % 5 ; y < GRIDHEIGHT ; y += 5 ){
        x = game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ];
        width = game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] -x+1;

        drawBg(gfx->gameBg, x*BLOCKSIZE + gfx->gameX,
                            y*BLOCKSIZE+game->scrollCount-scroll-1 + gfx->gameY , 
                            width*BLOCKSIZE, BLOCKSIZE+1);

        if( y*BLOCKSIZE+game->scrollCount < (GRIDHEIGHT-1)*BLOCKSIZE)
            drawFloor(gfx,x, y*BLOCKSIZE+game->scrollCount, width);
    }
      
    if (game->scrollCount >= BLOCKSIZE) {
        game->scrollCount -= BLOCKSIZE;
		scrollGrid(game, gfx);
    }
      
    scrollHeros(game, scroll);
}

void scrollGrid(game_t* game, data_t* gfx)
{
    game->floorTop++;
    if( --game->mapIndex < 0 )
        game->mapIndex = GRIDHEIGHT - 1;

    makeFloor(game,gfx,0);
	
    if (gblOps.recReplay) game->replay.scrolls++;
}

void scrollHeros(game_t* game, float scroll)
{
    int i;
    for (i=0; i<game->numHeros; i++) {
        game->heros[i].y += scroll;
    }
}

void initGame(game_t* game, data_t* gfx, int numHeros)
{
    int i,j;
    
    /* Initialize the floors appearing on the screen */
    game->floorTop = GRIDHEIGHT - 4;
    game->mapIndex = 0;
	game->scrollCount = 0;
    for (i=0; i<GRIDHEIGHT; i++) {
        makeFloor(game,gfx,i);
    }
    game->numHeros = numHeros;
    game->heros = malloc(sizeof(hero_t)*numHeros);
    game->T_count = 0;
    game->T_speed = 200;
	game->deadHeros = 0;
    
    for (i = 0; i<numHeros; i++) {
    	for (j=0; j<HEROANIMS; j++) {
    		initializeSpriteCtlRot(&game->heros[i].sprite[j], gfx->heroSprite[i][j]);
    	}
    	
        initHeroKeys(&game->heros[i],i);
        game->heros[i].x = GRIDWIDTH*BLOCKSIZE/2 - BLOCKSIZE + isOdd(i)*(i+1)*20;
        game->heros[i].y = (GRIDHEIGHT - 4)*BLOCKSIZE 
        	-getFrameRot(&game->heros[i].sprite[H_STAND],0)->h;
        game->heros[i].vx = game->heros[i].vy = 0;
        game->heros[i].dir = 0;
        game->heros[i].jump = 0;
        game->heros[i].floor = 0;
        game->heros[i].id = game->heros[i].previd = H_STAND;
        game->heros[i].dead = FALSE;
        game->heros[i].prevLives = -1;
        if (numHeros > 1) game->heros[i].lives = gblOps.mpLives-1;
        else game->heros[i].lives = 0;
        game->heros[i].angle = game->heros[i].rotateto = 0;
    }
    
    if (gblOps.recReplay) initReplay(game);
}


void freeGame(game_t* game)
{
	if (gblOps.recReplay)
		freeReplay(&(game->replay));
    free(game->heros);
}

void updateScore(data_t* gfx, game_t* game)
{
    int i,j;
    int x,y;
    SDL_Rect rect;
    char score[MAX_CHAR];
    char prevscore[MAX_CHAR];
    
    for (i=0; i < game->numHeros; i++) {
        if (game->heros[i].floor != game->heros[i].prevFloor) {
            sprintf(score,"%d",game->heros[i].floor);
            sprintf(prevscore,"%d",game->heros[i].prevFloor);
            
            x = gfx->scoreX[i];
            y = gfx->scoreY[i];
            
            drawBg(gfx->gameBg, x-1, y-1, 
                  SFont_TextWidth(gfx->scorefont,prevscore)+2, 
                  SFont_TextHeight(gfx->scorefont)+2);
            
            SFont_Write(gfx->scorefont, x,y, score);
        }
        if (game->heros[i].prevLives != game->heros[i].lives) {
        	rect.x = x = gfx->livesX[i];
        	rect.y = y = gfx->livesY[i];
        	rect.w = gfx->livePic->w;
        	rect.h = gfx->livePic->h;
        	
        	if (gfx->liveAlign) {
        	drawBg(gfx->gameBg, x, y, 
                   gfx->livePic->w, 
                   gfx->livePic->h * (game->heros[i].lives+2));
			} else {
				drawBg(gfx->gameBg, x, y, 
                   gfx->livePic->w * (game->heros[i].lives+2), 
                   gfx->livePic->h);
			}
            
            for (j=0; j <= game->heros[i].lives;j++) {
            	JPB_PrintSurface(gfx->livePic, NULL, &rect);
            	if (gfx->liveAlign) {
					y += gfx->livePic->h;
					rect.y = y;
            	} else {
					x += gfx->livePic->w;
					rect.x = x;
            	}
            }

            game->heros[i].prevLives = game->heros[i].lives;
        }
    }
}

/*
 * Returns wether we want to play again or not.
 */
int playGame(data_t* gfx, int numHeros)
{
    L_timer timer;
	time_t start,end;
    game_t game;
	int done = FALSE;
	int r;
    
    Mix_PlayMusic(gfx->musgame, -1);
    
    drawBg(gfx->gameBg,0,0,gblOps.w,gblOps.h);
    initGame(&game, gfx, numHeros);
	initTimer(&timer, getFps());
    FlipScreen();
	
	start = time(NULL);
	updateTimer(&timer);
    while(!done) {
		if ((r = updateInput(&game))) {
			if (r == PAUSED ) {
				done = pauseGame(gfx, &game, gfx->txt[txt_pause]);
			} else  {
				done = yesNoQuestion(gfx, &game, gfx->txt[txt_askquit]);
			}
			continueTimer(&timer);
		}
		if ((SDL_GetAppState() & SDL_APPINPUTFOCUS) != SDL_APPINPUTFOCUS) {
			done = pauseGame(gfx, &game, gfx->txt[txt_pause]);
			continueTimer(&timer);
		}
        updateTimer(&timer);
        if (updateGame(&game,gfx, timer.ms))
			done = ENDMATCH;
        updateScore(gfx, &game);
        if (gblOps.recReplay) updateReplay(&game, timer.ms);
        FlipScreen(); /* Apply changes to the screen */
    }
	if (done == ENDMATCH) {
		end = time(NULL);
		if (gblOps.recReplay) endReplay(&(game.replay));
		r = endMatch(gfx, &game, difftime(end,start));
	} else {
		r = FALSE;
	}
	
    freeGame(&game);
	
	return r;
}

void drawAnimatedSquare(data_t* gfx, Uint32 color, Uint8 alpha, int x, int y, int w, int h, int time)
{
	fader_t fader;
	L_timer timer;
	int xc = 0;
	int ret = 0;
	
	Mix_PlayChannel(-1, gfx->gquestion, 0);
	
	x += w/2;
	initTimer(&timer, getFps());
	updateTimer(&timer);
	setFader(&fader, 1, w/2+1, time, 0);
	
	do {
		updateTimer(&timer);
		ret = updateFader(&fader, timer.ms);
		JPB_drawSquare(color, alpha, x+xc, y, fader.value-xc, h);
		JPB_drawSquare(color, alpha, x-xc, y, xc-fader.value, h);
		xc = fader.value;
		FlipScreen();
	} while(!ret);
	
}

int pauseGame(data_t* gfx, game_t* game,  char* text)
{
	int w = (GRIDWIDTH-2) *BLOCKSIZE;
	int h = BLOCKSIZE + SFont_AlignedHeight(gfx->textfont, w-2*BLOCKSIZE, 0, text);
	int x = gfx->gameX + BLOCKSIZE;
	int y = gfx->gameY + (GRIDHEIGHT*BLOCKSIZE-h)/2;
	int ret = FALSE; 
	int done = FALSE;
	SDL_Event event;
	

	drawAnimatedSquare(gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);
	SFont_WriteAligned(gfx->textfont, x + BLOCKSIZE, y + BLOCKSIZE/2, w - 2*BLOCKSIZE, 0, ACENTER, text);
	FlipScreen();
	
	while(!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.sym == KEY_QUIT) {
					ret = TRUE;
					done = TRUE;
				} else if (event.key.keysym.sym == SDLK_p || event.key.keysym.sym == SDLK_PAUSE) {
					ret = FALSE;
					done = TRUE;
				}
				else {
					ret = FALSE;
					done = TRUE;
					SDL_PushEvent(&event);
					break;
				}
			}
		}
	}
	
	recoverScr(gfx, game, x -gfx->gameX, y -gfx->gameY, w, h);
	
	if (ret) {
		return yesNoQuestion(gfx, game, gfx->txt[txt_askquit]);
	}
		
	return ret;
}

int endMatch(data_t* gfx, game_t* game, int time)
{
	int i;
	int r, fact, newrec = FALSE;
	records_t rec;
	
	if (game->numHeros > 1) fact = gblOps.mpLives; else fact = 1;
	
	for (i = 0; i < game->numHeros; i++) {
		if ((r = checkRecord(gblOps.records, game->heros[i].floor/fact))) {
			makeRecord(&rec, gblOps.pname[i], game->heros[i].floor/fact, time);
			addRecord(gblOps.records, &rec, r); 
			newrec = TRUE;
		}
	}
	if (gblOps.recReplay && yesNoQuestion(gfx, game, gfx->txt[txt_askreplay])) {
		saveReplayMenu(gfx, &(game->replay));
		drawBg(gfx->gameBg,0,0,gblOps.w,gblOps.h);
	}
	if (newrec) {
		Mix_PlayChannel(-1, gfx->grecord, 0);
		if (yesNoQuestion(gfx, game, gfx->txt[txt_newhsc])) {
			return TRUE;
		}
		drawRecords(gfx,gblOps.records);
		pressAnyKey();
		return FALSE;
	} else {
		return yesNoQuestion(gfx, game, gfx->txt[txt_gameover]);
	}
}

int yesNoQuestion(data_t* gfx, game_t* game, char* text)
{
	int w = (GRIDWIDTH-2) *BLOCKSIZE;
	int h = BLOCKSIZE + SFont_AlignedHeight(gfx->textfont, w-2*BLOCKSIZE, 0, text);
	int x = gfx->gameX + BLOCKSIZE;
	int y = gfx->gameY + (GRIDHEIGHT*BLOCKSIZE-h)/2;
	int ret = FALSE;
    int done = FALSE;
	SDL_Event event;
	
	drawAnimatedSquare(gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);
	
	x += BLOCKSIZE;
	y += BLOCKSIZE/2;
	w -= 2*BLOCKSIZE;
	
	SFont_WriteAligned(gfx->textfont, x, y, w, 0, ACENTER, text);
	FlipScreen();
	
	while(!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_KEYDOWN) {
				if( event.key.keysym.sym == KEY_QUIT ||
					event.key.keysym.sym == SDLK_n){
					ret = FALSE;
					done = TRUE;
				} else if ( event.key.keysym.sym == SDLK_y ) {
					ret = TRUE;
					done = TRUE;
				}
			}
		}
	}
	
	recoverScr( gfx, game, x-BLOCKSIZE-gfx->gameX, y-BLOCKSIZE/2-gfx->gameY,
				w+2*BLOCKSIZE, h);
	
	return ret;
}
	
void updateTimer(L_timer* time)
{
    Uint32 current_ticks;
    Uint32 target_ticks;
    Uint32 the_delay;
    Uint32 currms;
    
    if (time->rate > 0) { 
        time->framecount++;
    
        currms = current_ticks = SDL_GetTicks();
        time->ms = currms - time->mscount;
        time->mscount = currms;
        
        target_ticks = time->lastticks + 
        			   (Uint32) ((float) time->framecount * time->rateticks);
    
        if (current_ticks <= target_ticks) {
            the_delay = target_ticks - current_ticks;
            SDL_Delay(the_delay);
        } else {
            time->framecount = 0;
            time->lastticks = SDL_GetTicks();
        }
    } else { /* Dont limit FPS */
        currms = SDL_GetTicks();
        time->ms = currms - time->mscount;
        time->mscount = currms;
    }
}

void continueTimer(L_timer* time)
{
	time->mscount = SDL_GetTicks();
}

void initTimer(L_timer* time, int rate)
{
    time->framecount = 0;
    
    time->rate = rate;
    time->rateticks = (1000.0 / (float) time->rate);
    time->mscount = time->lastticks = SDL_GetTicks();
    time->ms = 0;
}

int updateGame(game_t* game, data_t* gfx, float ms)
{
    int i;       
    int done = TRUE;
	
    float fact = ms/DELAY;
    
    for (i=0; i<game->numHeros; i++) {
        undrawHero(i, gfx, game);   
    }
    
    if( game->T_speed < 5000 )
        game->T_speed += fact;
    else if( game->T_speed < 10000 )
        game->T_speed += fact/4;
    else if( game->T_speed < 15000 )
        game->T_speed += fact/8;
    
    
    game->T_count += game->T_speed * fact;
    
    if (gblOps.scrollMode == HARDSCROLL) {
        if( game->T_count > 20000 ){
            game->T_count -= 20000;
            hardScrollUp(game,gfx);
        }
    } else {
        softScrollUp(game,gfx,1 / (20000 / game->T_speed / BLOCKSIZE) * fact);
    }
    
    for (i=0; i<game->numHeros; i++) {
        if (game->heros[i].dead == FALSE) {
            if (updateHero(game, gfx, i, ms) == DEAD) {
            	if (game->heros[i].lives < 1) {
            		game->heros[i].dead = TRUE;
					game->deadHeros++;
				} else {
					reliveHero(game, i);
				}
				if (game->deadHeros == game->numHeros) {
					done = TRUE;
				}
				game->heros[i].lives--;
			}
            done= FALSE;	
        }
    }

    return done;
}

void reliveHero(game_t* game, int num)
{
    hero_t* hero = &game->heros[num];
        
    hero->y = MINSKY;
    hero->x = (GRIDWIDTH * BLOCKSIZE - getFrameRot(&hero->sprite[H_STAND], 0)->w) / 2;
    hero->id = H_STAND;
    hero->vx = 0;
    hero->vy = -2;
    hero->prevLives = hero->lives;
}

void playHeroSound(data_t* gfx, int sound, replay_t* rep)
{
	if (sound == S_JUMP) Mix_PlayChannel(-1, gfx->gjump, 0);
	if (sound == S_FALL) Mix_PlayChannel(-1, gfx->gfall, 0);
	if (sound == S_DIE) Mix_PlayChannel(-1, gfx->gdie, 0);
	if (gblOps.recReplay) rep->sounds |= sound;
}

int updateHero(game_t* game, data_t* gfx, int num, float ms)
{
    int acc = 0;
    int st = 0;
    int floor;
    hero_t* hero = &game->heros[num];
    float fact = ms/DELAY;
    
	st = updateHeroPosition(game, num, fact);
   	
    /* If we are too fast we should keep on rolling */
    if( hero->y < MINSKY ){
        if (gblOps.scrollMode == HARDSCROLL) {
            hardScrollUp(game,gfx);
            //hero->y += 16;
        } else {
            softScrollUp(game,gfx,MINSKY - hero->y);
            //hero->y += MINSKY - hero->y;
        }
        
        game->T_count = 0;
    } else if( hero->y >= GRIDHEIGHT*BLOCKSIZE ) {
		playHeroSound(gfx, S_DIE, &(game->replay));
        return DEAD;
    }
    
    if (hero->st == 0 && st > 0) playHeroSound(gfx, S_FALL, &(game->replay));
    
    /* If the player is standing in the floor... */
    if( st > 0 ){
        acc = 3;
        floor = (game->floorTop - hero->y / BLOCKSIZE) / FLOORGAP + 1;
        hero->rotateto = 0;
        
        if (gblOps.scrollMode == HARDSCROLL)
            hero->y = st*BLOCKSIZE;
        else
            hero->y = st*BLOCKSIZE + game->scrollCount+1;
            
        hero->vy = 0;
		if (hero->left.pressed || hero->right.pressed)
			hero->id = H_WALK;
		else
			hero->id = H_STAND;
			
        if( hero->up.pressed ){
            hero->jump = abs(hero->vx)/4 +7;
            hero->vy = -hero->jump/2 -12;
            hero->rotateto = 90+ 180*hero->dir;
            st = 0;
            playHeroSound(gfx, S_JUMP, &(game->replay));
        }
    }else{
        acc = 2;
        floor = 0;
        hero->id = H_JUMP;
    }
    
    /* If we are floating in the air */
    if( st == 0 ){
        if( hero->jump > 0 ){
            hero->vy = -hero->jump/2 -12;
            if( hero->up.pressed )
                hero->jump -= fact;
            else
                hero->jump = 0;
        } else {
            hero->vy += 2 *fact;
            if( hero->vy > 0 )
                hero->rotateto = 180;
            if( hero->vy > BLOCKSIZE )
                hero->vy = BLOCKSIZE;
        }
    }
    
    /* Set actions */
    if( hero->left.pressed){
        hero->vx -= acc *fact;
        if( hero->vx < -32 )
            hero->vx = -32;
        hero->dir = LEFT;
    }else if( hero->right.pressed){
        hero->vx += acc*fact;
        if( hero->vx > 32 )
            hero->vx = 32;
        hero->dir = RIGHT;
    }else if( st > 0 ){
        if( hero->vx < -2 )
            hero->vx += 3*fact;
        else if( hero->vx > 2 )
            hero->vx -= 3*fact;
        else
            hero->vx = 0;
    } 
    
    rotateHero(hero,ms);
    
	hero->st = st;
    if (hero->id != hero->previd) {
    	hero->previd = hero->id;
    	hero->sprite[hero->id].elpTime = hero->sprite[hero->id].frame = 0; 
    }
    
    animateSpriteRot(&hero->sprite[hero->id],ms);
    
    drawHero(gfx,hero);
    if (floor != 0) {
        hero->prevFloor = hero->floor;
        hero->floor = floor;
    }
    
    return TRUE;
}

/* Updates player position and returns wether it is standing or not */
int updateHeroPosition ( game_t* game, int num , float fact)
{
    float Dy, diff, Dycount;
    int st = 0, i;
    hero_t* hero = &game->heros[num];
    
    hero->x += hero->vx / 2 *fact;
    
    /* The blue bricks block your way */
    if( hero->x < BLOCKSIZE ) {
        hero->x = BLOCKSIZE ;
        hero->vx = -hero->vx/2;
    }else if( hero->x > GRIDWIDTH*BLOCKSIZE - 3*BLOCKSIZE ) {
        hero->x = GRIDWIDTH*BLOCKSIZE - 3*BLOCKSIZE ;
        hero->vx = -hero->vx/2;
    }
    
    Dy = Dycount = hero->vy *fact;

	/* Whe have to take into account that, at low FPS rates, whe might be
	   going too fast and missing floors */
	if (Dy > BLOCKSIZE) {
		for (i = 0; i <= Dy/BLOCKSIZE; i++) {
			if ((st = isStand(game, hero->x, hero->y))) {
				return st;
			}
			if (Dycount > BLOCKSIZE) diff = BLOCKSIZE;
			else diff = Dycount;
			Dycount -= diff;
			hero->y += diff;
		}
		return st;
	} else {
		hero->y += Dy;
		if (hero->vy < 0) return FALSE;
		else return isStand( game, hero->x, hero->y );
	}

}

/* Returns whether a hero being at (ix,iy) would be standing or not and the
   Y position of that floor */
int isStand( game_t* game, int ix, int iy)
{
    int y;

    if (iy + 2*BLOCKSIZE > (GRIDHEIGHT-1)*BLOCKSIZE)
        return FALSE;

    y = ( ( iy + 2*BLOCKSIZE - game->scrollCount) / BLOCKSIZE );
    if( y >= GRIDHEIGHT )
        return FALSE;

    if( game->floor_l[ ( y+game->mapIndex) % GRIDHEIGHT ] *BLOCKSIZE < (ix+24)
        && (ix-8) < game->floor_r[ ( y+game->mapIndex) % GRIDHEIGHT ] *BLOCKSIZE )
        return y-2;
    else
        return FALSE;
}


void rotateHero(hero_t* hero, float ms)
{
    float fact = ms/DELAY;
    if (gblOps.rotMode == ROTORIG) {
        hero->angle = hero->rotateto;
    } else if (gblOps.rotMode == ROTFULL) {
        if (hero->rotateto == 0) hero->angle = 0;
        else {
            hero->angspeed = abs(hero->vx)*bool2sign(hero->dir)
                           + abs(hero->vy)*bool2sign(hero->dir);
            hero->angle += hero->angspeed*fact;
        }
    }
}


void drawHero(data_t* gfx,hero_t* hero)
{
    SDL_Rect dest;
    dest.x = hero->x+ gfx->gameX +BLOCKSIZE -(getFrameRot(&hero->sprite[hero->id],0)->w -32)/2 ;
    dest.y = hero->y+ gfx->gameY +BLOCKSIZE -(getFrameRot(&hero->sprite[hero->id],0)->h -32)/2;
    printSpriteRot(&hero->sprite[hero->id],NULL,&dest,hero->dir,hero->angle);
}

#define MAX(a,b) ((a) > (b) ? (a) : (b))

void undrawHero(int heronum, data_t* gfx, game_t* game)
{
	int w = getFrameRot(&game->heros[heronum].sprite[game->heros[heronum].id],0)->w;
	int h = getFrameRot(&game->heros[heronum].sprite[game->heros[heronum].id],0)->h;
	int diag;
	
    diag = 2*MAX(w,h);
    recoverScr( gfx, game,
       	game->heros[heronum].x - (diag)/2 + BLOCKSIZE,
        game->heros[heronum].y - (diag)/2 + BLOCKSIZE, 
        diag, diag);
}


void recoverScr( data_t* gfx, game_t* game, int x, int y, int w, int h )
{
    SDL_Rect dest;
    int x0 = x;
    int y2,x2;
    
    y2 = (y+h-1) / BLOCKSIZE;
    y = y/BLOCKSIZE -2;
    
    for( ; y <= y2 ; y++ ){
        x2 = (x0+w-1) /BLOCKSIZE;
        x = x0/BLOCKSIZE -1;
        for (; x <= x2; x++) {
            dest.x = x*BLOCKSIZE + gfx->gameX;
            dest.y = y*BLOCKSIZE + gfx->gameY + game->scrollCount;
            drawBg(gfx->gameBg,dest.x,dest.y,BLOCKSIZE, BLOCKSIZE);
            
            if (y > GRIDHEIGHT-2 || x < 1 || x > GRIDWIDTH-1)
				continue;
			
            if (game->floor_l[(y+game->mapIndex)%GRIDHEIGHT] == x) {
                JPB_PrintSurface(gfx->floorL,NULL,&dest);
            } else if (x == game->floor_r[(y+game->mapIndex)%GRIDHEIGHT]) {
                JPB_PrintSurface(gfx->floorR,NULL,&dest);
            } else if (game->floor_l[(y+game->mapIndex)%GRIDHEIGHT] < x &&
                x < game->floor_r[(y+game->mapIndex)%GRIDHEIGHT])
            {
                JPB_PrintSurface(gfx->floorC,NULL,&dest);
            }
        }
    }
}

int isFloor(game_t* game, int x, int y)
{
    y -= game->scrollCount;
    if ( ( game->floor_r[(y+game->mapIndex)%GRIDHEIGHT] > x ) &&
         ( game->floor_l[(y+game->mapIndex)%GRIDHEIGHT] < x )  ) {
        return TRUE;
    } else {
        return FALSE;
    }
}

int updateInput(game_t* game)
{
    int done = 0;
    int i;
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
                for (i=0; i < game->numHeros; i++) {
                    markHeroKeys(&event,&game->heros[i]);
                }
                break;
            /* A key UP. */
            case SDL_KEYUP:
                for (i=0; i < game->numHeros; i++) {
                    unmarkHeroKeys(&event,&game->heros[i]);
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

void initHeroKeys(hero_t* hero, int num)
{
    hero->left.def = gblOps.keys[num][LEFTK];
    hero->left.pressed = FALSE;
    hero->right.def = gblOps.keys[num][RIGHTK];
    hero->right.pressed = FALSE;
    hero->up.def = gblOps.keys[num][JUMPK];
    hero->up.pressed = FALSE;
}

void markHeroKeys(SDL_Event* event, hero_t* hero) 
{
    if( event->key.keysym.sym == hero->left.def){
        hero->left.pressed = 1;
    } else
    if( event->key.keysym.sym == hero->right.def){
        hero->right.pressed = 1;
    } else
    if( event->key.keysym.sym == hero->up.def){
        hero->up.pressed = 1;
    }
}

void unmarkHeroKeys(SDL_Event* event, hero_t* hero)
{
    if( event->key.keysym.sym == hero->left.def){
        hero->left.pressed = 0;
    } else
    if( event->key.keysym.sym == hero->right.def){
        hero->right.pressed = 0;
    } else
    if( event->key.keysym.sym == hero->up.def){
        hero->up.pressed = 0;
    }
}

void drawRecords(data_t* gfx, records_t* rtab)
{
	int x, x1, x2, x3, x4, x5, y, w, h, i;
	char buf[128];
	
	x = gfx->gameX + 2*BLOCKSIZE;
	w = GRIDWIDTH*BLOCKSIZE - 4*BLOCKSIZE;
	h = BLOCKSIZE + 12*gfx->textfont->Surface->h 
	    + SFont_AlignedHeight(gfx->textfont, w-2*BLOCKSIZE,0,gfx->txt[txt_hscnote]);
	y = gfx->gameY + (GRIDHEIGHT*BLOCKSIZE - h)/2;
	
	JPB_PrintSurface(gfx->gameBg, NULL, NULL);
	drawAnimatedSquare(gfx, gfx->gcolor, gfx->galpha, x, y, w, h, MSGTIME);
	/*
	 x1:#  x2:Name                                x3:Floor x4:Mode x5:Time
	 ---------------------------------------------------------------------
	*/
	y += BLOCKSIZE;
	
	x1 = x + BLOCKSIZE;
	x2 = x1 + SFont_TextWidth(gfx->textfont, "#   ");
	sprintf(buf, "%s %s %s", gfx->txt[txt_floor], gfx->txt[txt_mode], gfx->txt[txt_time]);
	x3 = x1 + w - 2*BLOCKSIZE - SFont_TextWidth(gfx->textfont, buf);
	SFont_Write(gfx->textfont, x3, y, buf);
	
	sprintf(buf,"%s ", gfx->txt[txt_floor]);
	x4 = x3 + SFont_TextWidth(gfx->textfont, buf);
	sprintf(buf,"%s ", gfx->txt[txt_mode]);
	x5 = x4 + SFont_TextWidth(gfx->textfont, buf);
	
	sprintf(buf,"#  %s ", gfx->txt[txt_name]);
	SFont_Write(gfx->textfont, x1, y, buf);
	
	for (i = 0; i < MAX_RECORDS; i++) {
		y += gfx->textfont->Surface->h-1;
		
		sprintf(buf,"%d", i+1);
		SFont_Write(gfx->textfont, x1, y, buf);
		
		SFont_Write(gfx->textfont, x2, y, rtab[i].pname);
		SFont_FillWith(gfx->textfont, 
			x2 + SFont_TextWidth(gfx->textfont, rtab[i].pname) + 3,
		    y, x3 -6 -(x2 + SFont_TextWidth(gfx->textfont, rtab[i].pname)),'.');
		
		sprintf(buf,"%d", rtab[i].floor);
		SFont_Write(gfx->textfont, x3, y, buf);
		
		SFont_Write(gfx->textfont, x4, y, rtab[i].mode);
		
		sprintf(buf,"%d", rtab[i].time);
		SFont_Write(gfx->textfont, x5, y, buf);
	}
	y += gfx->textfont->Surface->h*2;
	SFont_WriteAligned(gfx->textfont, x1, y, w-2*BLOCKSIZE,
						0, ALEFT, gfx->txt[txt_hscnote]);
	
	FlipScreen();
}
