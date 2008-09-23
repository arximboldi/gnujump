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

#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <sys/stat.h>

typedef struct fader
{
  float value;
  float target;
  float start;
  float time;
  float delta;
  int loop;
}
fader_t;

long getFileSize (char *fname);

int getFps ();

void setFader (fader_t * fader, int start, int target, int time, int loop);

int updateFader (fader_t * fader, int ms);

int rnd (int range);

void srnd (void);

void pressAnyKey ();

void findNextValue (FILE * tfile);

void skipValueStr (FILE * tfile);

int getValue_int (FILE * tfile, char *value);

float getValue_float (FILE * tfile, char *value);

void getValue_str (FILE * tfile, char *value, char *data, char *path);

char *getValue_charp (FILE * tfile, char *value);

int isOdd (int n);

void putValue_int (FILE * tfile, char *data, int value);

void putValue_str (FILE * tfile, char *data, char *value);

void putLine (FILE * tfile);

void putComment (FILE * tfile, char *comment);

int bool2sign (int n);

int checkExtension (char *file, char *ext);

int getDirList (char *folder, char ***tab);

int getFileList (char *folder, char ***tab);

int sumDirList (char ***a, int an, char **b, int bn);

int sumStringTabs (char ***a, int an, char **b, int bn);

int sumStringTabs_Cat (char ***a, int an, char **b, int bn, char *string);

#endif //_TOOLS_H_
