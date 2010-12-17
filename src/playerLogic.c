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


#include "gsquares.h"

void
initFrame(const int horizontalLines, const int verticalLines, const int startX,
        const int startY, const int lineLength, Frame *head) {
    Frame *temp = head, *_temp = NULL, *__temp = NULL;
    int tempX = startX, tempY = startY;
    for (int i = 0; i <= verticalLines; i++) {
        __temp = temp;
        for (int j = 0; j <= horizontalLines; j++) {
            if (j == 0)
                temp->left = NULL;
            temp->x = tempX + j*lineLength;
            temp->y = tempY;
            temp->horizontalLeft = temp->horizontalRight = temp->verticalDown =
                    temp->verticalUp = false;
            temp->boxFlag = (i != verticalLines) ? emptyBox : ownerNotApplicable;
            if (j == horizontalLines) {
                temp->right = NULL;
                temp->boxFlag = ownerNotApplicable;
                break;
            }
            _temp = (Frame *) calloc(1, sizeof (Frame));
            temp->right = _temp;
            _temp->left = temp;
            temp = _temp;
        }
        if (i == verticalLines)
            break;
        tempY += lineLength;
        __temp->down = (Frame *) calloc(1, sizeof (Frame));
        temp = __temp->down;
    }
    _temp = head;
    for (int i = 0; i < verticalLines; i++) {
        __temp = _temp->down;
        temp = _temp;
        for (int j = 0; j <= horizontalLines; j++) {
            if (i == 0)
                _temp->up = NULL;
            if (i == verticalLines - 1)
                __temp->down = NULL;
            _temp->down = __temp;
            __temp->up = _temp;
            _temp = _temp->right;
            __temp = __temp->right;
        }
        _temp = temp->down;
    }
    return;
}

void
updateFrame(const int horizontalLines, const int verticalLines, const int startX,
        const int startY, const int lineLength, Frame *head) {
    Frame *temp = head, *_temp = NULL;
    int tempX = startX, tempY = startY;
    for (int i = 0; i <= verticalLines; i++) {
        _temp = temp;
        for (int j = 0; j <= horizontalLines; j++) {
            temp->x = tempX + j*lineLength;
            temp->y = tempY;
            temp = temp->right;
        }
        tempY += lineLength;
        temp = _temp->down;
    }

    return;
}

Frame *
clickInBox(Frame *head, int x, int y, int lineLength) {
    int i = (x - head->x) / lineLength, j = (y - head->y) / lineLength;
    while (i != 0) {
        head = head->right;
        --i;
    }
    while (j != 0) {
        head = head->down;
        --j;
    }
    return (head->x == x && head->y == y) || (head->down == NULL) ? NULL : head;
}

bool
isSurrounded(Frame *point) {
    return (point->verticalUp && point->up->horizontalRight &&
            point->right->verticalUp && point->horizontalRight) ? true : false;
}

bool
findEndPoints(Frame *head, Frame **point1, Frame **point2, int x, int y,
        int lineLength, LineDirection *directionFlag) {
    int temp, _temp;
    *directionFlag = horizontal;
    *point1 = clickInBox(head, x, y, lineLength);
    if (!(*point1))
        return false;
    else {
        temp = x - (*point1)->x + (*point1)->down->y - y - lineLength;
        _temp = x - (*point1)->x - (*point1)->down->y + y;
        if (temp != 0 && _temp != 0) {
            if (temp < 0 && _temp < 0) {
                if (x - (*point1)->x <= lineLength / 4 && !(*point1)->verticalDown) {
                    *point2 = (*point1)->down;
                    (*point1)->verticalDown = (*point2)->verticalUp = true;
                    *directionFlag = vertical;
                } else return false;
            } else if (temp < 0 && _temp > 0) {
                if ((*point1)->down->y - y <= lineLength / 4 &&
                        !(*point1)->down->horizontalRight) {
                    *point1 = (*point1)->down;
                    *point2 = (*point1)->right;
                    (*point1)->horizontalRight = (*point2)->horizontalLeft = true;
                } else return false;
            } else if (temp > 0 && _temp < 0) {
                if (y - (*point1)->y <= lineLength / 4 && !(*point1)->horizontalRight) {
                    *point2 = (*point1)->right;
                    (*point1)->horizontalRight = (*point2)->horizontalLeft = true;
                } else return false;
            } else {
                if ((*point1)->right->x - x <= lineLength / 4 &&
                        !(*point1)->right->verticalDown) {
                    *point1 = (*point1)->right;
                    *point2 = (*point1)->down;
                    (*point1)->verticalDown = (*point2)->verticalUp = true;
                    *directionFlag = vertical;
                } else return false;
            }
        } else
            return false;
    }
    return true;
}

bool
isFrameFull(Frame *head) {
    Frame *temp;
    while (head->down) {
        temp = head;
        while (temp->right) {
            if (temp->boxFlag == emptyBox)
                return false;
            temp = temp->right;
        }
        head = head->down;
    }
    return true;
}

Frame **
findBoxToFill(Frame *point, LineDirection directionFlag) {
    Frame **frame = (Frame **) calloc(2, sizeof (Frame *));
    *frame = *(frame + 1) = NULL;
    if (directionFlag == horizontal) {
        if (!point->up)
            *(frame + 1) = isSurrounded(point->down) ? point : NULL;
        else if (!point->down)
            *frame = isSurrounded(point) ? point->up : NULL;
        else {
            *frame = isSurrounded(point) ? point->up : NULL;
            *(frame + 1) = isSurrounded(point->down) ? point : NULL;
        }
    } else {
        if (!point->left)
            *(frame + 1) = isSurrounded(point->down) ? point : NULL;
        else if (!point->right)
            *frame = isSurrounded(point->down->left) ? point->left : NULL;
        else {
            *frame = isSurrounded(point->down->left) ? point->left : NULL;
            *(frame + 1) = isSurrounded(point->down) ? point : NULL;
        }
    }
    return frame;
}

TurnResult
playerTurn(cairo_t *lineCr, cairo_t *boxCr, cairo_t *pointCr,
        Frame *head, int x, int y, int lineLength, int pointRadius,
        unsigned short *boxCount, BoxOwner boxOwner) {
    Frame *point1, *point2;
    LineDirection directionFlag;
    TurnResult result = invalidClick;

    if (!findEndPoints(head, &point1, &point2, x, y, lineLength, &directionFlag))
        return result;
    cairo_move_to(lineCr, point1->x, point1->y);
    cairo_line_to(lineCr, point2->x, point2->y);
    cairo_stroke(lineCr);
    result = lineDrawn;
    cairo_arc(pointCr, point1->x, point1->y, pointRadius, 0, 2 * M_PI);
    cairo_arc(pointCr, point2->x, point2->y, pointRadius, 0, 2 * M_PI);
    cairo_fill(pointCr);
    Frame **frame = findBoxToFill(point1, directionFlag);
    if ((*frame)) {
        cairo_rectangle(boxCr, (*frame)->x + pointRadius / sqrt(2), (*frame)->y +
                pointRadius / sqrt(2), lineLength - pointRadius * sqrt(2), lineLength -
                pointRadius * sqrt(2));
        cairo_fill(boxCr);
        (*frame)->boxFlag = boxOwner;
        (*boxCount)++;
        result = boxDrawn;
    }
    if ((*(frame + 1))) {
        cairo_rectangle(boxCr, (*(frame + 1))->x + pointRadius / sqrt(2),
                (*(frame + 1))->y + pointRadius / sqrt(2), lineLength - pointRadius *
                sqrt(2), lineLength - pointRadius * sqrt(2));
        cairo_fill(boxCr);
        (*(frame + 1))->boxFlag = boxOwner;
        (*boxCount)++;
        result = boxDrawn;
    }
    if (isFrameFull(head)) {
        result = frameIsFull;
        return result;
    }

    return result;
}
