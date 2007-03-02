/*
 * GNUjump
 * =======
 *
 * Copyright (C) 2005-2006, Juan Pedro Bolivar Puente
 *
 * GNUjump is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * GNUjump is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNUjump; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef SETUP_H
#define SETUP_H

#include "gnujump.h"

void initGblOps(void);

void cleanGblOps(void);

int loadConfigFile(char* fname);

int writeConfigFile(char* fname);

char* getThemeComment(char* fname);

char* getLangComment(char* fname);

void resetTheme(data_t* gfxdata);

void EngineInit();

void setWindow();

void SetVideoGl(int w, int h, int use_fullscreen,int bpp);

void SetVideoSw(int w, int h, int fullscreen,int bpp);

int loadGraphics(data_t* data, char* fname);

void freeGraphics(data_t* data);

void resetVolumes();

int loadSounds(data_t* data, char* fname);

void freeSounds(data_t* data);

#endif
