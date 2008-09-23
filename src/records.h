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

#ifndef _RECORDS_H
#define _RECORDS_H

#include "gnujump.h"

int loadRecords (char *fname, records_t * rec);

int writeRecords (char *fname, records_t * rec);

int addRecord (records_t * rtab, records_t * rec, int pos);

int checkRecord (records_t * rtab, int floor, int time);

void makeRecord (records_t * rec, char *name, int floor, int time);

void defaultRecords (records_t * rec);

void freeRecords (records_t * rec);

#endif /* _RECORDS_H */
