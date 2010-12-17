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


#ifndef INITIALIZER_H
#define	INITIALIZER_H

#include "gsquares.h"

typedef enum {
    HUMAN,
    COMPUTER
} OPPONENT_TYPE;

typedef enum {
    easy,
    medium,
    hard
} Difficulty;

typedef struct {
    GtkWidget *window, *comboBoxHorizontal, *comboBoxVertical,
            *radioButtonHuman, *radioButtonComputer, *player1Entry,
            *player2Entry, *difficulty_combo;
    int horizontalLines, verticalLines;
    char *player1Name, *player2Name;
    OPPONENT_TYPE opponent;
    Difficulty difficulty;
} Data;

bool
dataInitializer(char **, char **, const int *, const int *, OPPONENT_TYPE *, Difficulty *);

bool
setFrameAttribute(int, int, int, int, const int *, const int *, const int *,
        const int *);

bool
isClickInsideFrame(int, int);

#endif	/* INITIALIZER_H */

