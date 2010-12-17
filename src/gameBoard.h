/*
 * GSquares: a turn based strategy game
 * Copyright (c) 2010-2011  Dibyendu Das
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef GAMEBOARD_H
#define	GAMEBOARD_H

#include "gsquares.h"

typedef enum {
    noExpose,
    firstExpose,
    secondExposeOnwards,
} ExposureType;

bool
gamePlay();

gboolean
drawing_area_expose_event_handler(GtkWidget *);

int
result_message(char *, char *, unsigned short, unsigned short);

void
show_about_dialogue();

#endif	/* GAMEBOARD_H */

