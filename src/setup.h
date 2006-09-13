/*
 * SDLjump
 * (C) 2005 Juan Pedro Bol�ar Puente
 * 
 * This simple but addictive game is based on xjump. Thanks for its author for
 * making such a great game :-)
 * 
 * setup.h
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

#ifndef SETUP_H
#define SETUP_H

#include "sdljump.h"

void initGblOps(void);

void cleanGblOps(void);

int loadConfigFile(char* fname);

int writeConfigFile(char* fname);

char* getThemeComment(char* fname);

char* getLangComment(char* fname);

int loadGraphics(data_t* data, char* fname);

void freeGraphics(data_t* data);

void resetTheme(data_t* gfxdata);

void EngineInit(int mouse, int fullscreen, int w, int h, int bpp, int gl);

void SetVideoGl(int w, int h, int use_fullscreen,int bpp);

void SetVideoSw(int w, int h, int fullscreen,int bpp);

int loadLanguage(data_t* data, char* fname);

void freeLanguage(data_t* data);

#endif
