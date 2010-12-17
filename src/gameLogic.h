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


#ifndef GAMELOGIC_H
#define	GAMELOGIC_H

#include "gsquares.h"

typedef enum {
    emptyBox,
    firstPlayerBox,
    secondPlayerBox,
    ownerNotApplicable
} BoxOwner;

typedef enum {
    visited,
    not_visited,
    visit_flag_not_applicable
} BoxVisitFlag;

typedef struct _Frame {
    unsigned int x, y;
    struct _Frame *left, *right, *up, *down;
    bool horizontalRight, horizontalLeft, verticalUp, verticalDown;
    /*
     * The following 2 flags are applicable
     * for upper left corner of a box
     */
    BoxOwner boxFlag;
    BoxVisitFlag visit;
} Frame;

typedef enum {
    horizontal,
    vertical
} LineDirection;

typedef enum {
    invalidClick,
    lineDrawn,
    boxDrawn,
    frameIsFull
} TurnResult;

/*
 * Initialize the Frame (pointed to by
 * the head pointer argument) with user
 * given values
 */
void
initFrame(const int, const int, const int, const int, const int, Frame *);

/*
 * updates the coordinates of the frame according to the new
 * drawing area resolution with all the line flags & box flags unaffected
 */
void
updateFrame(const int, const int, const int, const int, const int, Frame *);

/*
 * This function returns the upper left vertex or NULL
 * of the square in which pointer has been clicked
 * (if at all been clicked inside Frame)
 */
Frame *
clickInBox(Frame *, int, int, int);

/*
 * Finds the 2 end points to draw a line for
 * a valid click & sets the point1 , point2 &
 * lineDirectionFlag (horizontal or vertical)
 * accordingly
 *
 * Returns "true" on success & "false" otherwise
 * (e.g: click near already deawn line or boundary
 * condition violated)
 */
bool
findEndPoints(Frame *, Frame **point1, Frame **point2, int, int, int, LineDirection *direction);

/*
 * Returns the upper left corners of the
 * possible boxes otherwise NULL
 */
Frame **
findBoxToFill(Frame *, LineDirection);

/*
 * Checks whether the box whose bottom left
 * corner is represented by argument
 * is surrounded by lines
 */
bool
isSurrounded(Frame *);

/*
 * Checks whether the Frame is filled
 * completely with occupied boxes
 */
bool
isFrameFull(Frame *);

/*
 * Takes the click coordinate (for
 * human players). Draw line (if click is valid)
 * & boxes (if there is scope).
 *
 * Returns the result of the particular turn as
 * specified by the "TurnResult" enum.
 */
TurnResult
playerTurn(cairo_t *, cairo_t *, cairo_t *, Frame *, int, int, int,
        int, unsigned short *, BoxOwner);

Frame **
computerTurn(Frame *, cairo_t *, cairo_t *, cairo_t *, int, int,
        unsigned short *, BoxOwner, GtkWidget **, Difficulty);

#endif	/* GAMELOGIC_H */

