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

typedef struct _PointList {
    Frame *point;
    struct _PointList *next;
} PointList;

typedef enum {
    TOP = 1,
    RIGHT,
    BOTTOM,
    LEFT
} BoxSide;

const gdouble waitInterval = 500;

static int boxChainCount;

/*
 * Returns the list with top left vertices
 * of the boxes that are filled with 'n' sides.
 * Otherwise returns NULL.
 *
 * (For n = 3 the function returns a list of only one point)
 */
PointList *
find_boxes_with_n_filled_sides(Frame *head, int n) {
    PointList *list = (PointList *) calloc(1, sizeof (PointList));
    PointList *tempList = list;
    Frame *temp = head, *_temp;
    int pointCount = 0;
    while (temp->down) {
        _temp = temp;
        while (temp->right) {
            int sideCount = 0;
            if (temp->horizontalRight)
                sideCount++;
            if (temp->verticalDown)
                sideCount++;
            if (temp->right->verticalDown)
                sideCount++;
            if (temp->down->horizontalRight)
                sideCount++;
            if (sideCount == n) {
                tempList->point = temp;
                pointCount++;
                if (n == 3)
                    return tempList;
                tempList->next = (PointList *) calloc(1, sizeof (PointList));
                tempList = tempList->next;
            }
            temp = temp->right;
        }
        temp = _temp->down;
    }

    tempList = list;

    if (!pointCount)
        list = NULL;
    else {
        for (int i = 1; i < pointCount; i++)
            tempList = tempList->next;
        tempList->next = NULL;
    }

    return list;
}

/*
 * Returns the total number of filled sides of the box
 * whose upper left vertex is pointed to by the argument.
 */
int
find_side_count_of_box(Frame *point) {
    return ((int) (point->horizontalRight + point->verticalDown +
            point->down->horizontalRight + point->right->verticalDown));
}

/*
 * Generates a random number between 1 and randMax
 */
int
genRand(int randMax) {
    if (randMax == 1)
        return 1;
    time_t rawTime;
    struct tm *timeInfo;
    time(&rawTime);
    timeInfo = localtime(&rawTime);
    char buffer[2000];
    strftime(buffer, 2000, "%s", timeInfo);

    return (((long long) ((double) sqrt(atoll(buffer)) * (double) sqrt(rand()) * 10)) % randMax) + 1;
}

/*
 * Returns the side [TOP, RIGHT, BOTTOM or LEFT (as specified by
 * the BoxSide enum)] of the box whose upper left vertex is pointed
 * to by the argument if the non diagonal adjacent box (if exists)
 * has total number of sides less than 2.
 *
 * Otherwise returns -1.
 */
int
find_suitable_side_of_box(Frame *point) {
    BoxSide side[4];
    int count = 0;
    if (!point->horizontalRight) {
        if (!point->up)
            side[count++] = TOP;
        else {
            if (find_side_count_of_box(point->up) < 2)
                side[count++] = TOP;
        }
    }

    if (!point->right->verticalDown) {
        if (point->right->right) {
            if (find_side_count_of_box(point->right) < 2)
                side[count++] = RIGHT;
        } else
            side[count++] = RIGHT;
    }

    if (!point->down->horizontalRight) {
        if (point->down->down) {
            if (find_side_count_of_box(point->down) < 2)
                side[count++] = BOTTOM;
        } else
            side[count++] = BOTTOM;
    }

    if (!point->verticalDown) {
        if (!point->left)
            side[count++] = LEFT;
        else {
            if (find_side_count_of_box(point->left) < 2)
                side[count++] = LEFT;
        }
    }

    if (!count)
        return -1;

    return side[genRand(count) - 1];
}

void
drawBox(cairo_t *lineCr, cairo_t *boxCr, cairo_t *pointCr, Frame *point,
        int lineLength, int pointRadius, BoxOwner boxOwner,
        unsigned short **boxCount) {
    LineDirection direction;
    Frame **frame, *nextPoint;
    if (!point->horizontalRight)
        direction = horizontal;
    else if (!point->verticalDown)
        direction = vertical;
    else if (!point->right->verticalDown) {
        point = point->right;
        direction = vertical;
    } else {
        direction = horizontal;
        point = point->down;
    }
    if (direction == horizontal) {
        nextPoint = point->right;
        point->horizontalRight = nextPoint->horizontalLeft = true;
    } else {
        nextPoint = point->down;
        point->verticalDown = nextPoint->verticalUp = true;
    }
    cairo_move_to(lineCr, point->x, point->y);
    cairo_line_to(lineCr, nextPoint->x, nextPoint->y);
    cairo_stroke(lineCr);
    cairo_arc(pointCr, point->x, point->y, pointRadius, 0, 2 * M_PI);
    cairo_fill(pointCr);
    cairo_arc(pointCr, nextPoint->x, nextPoint->y, pointRadius, 0, 2 * M_PI);
    cairo_fill(pointCr);
    frame = findBoxToFill(point, direction);
    if ((*frame)) {
        cairo_rectangle(boxCr, (*frame)->x + pointRadius / sqrt(2), (*frame)->y +
                pointRadius / sqrt(2), lineLength - pointRadius * sqrt(2), lineLength -
                pointRadius * sqrt(2));
        cairo_fill(boxCr);
        (*frame)->boxFlag = boxOwner;
        (**boxCount)++;
    }
    if ((*(frame + 1))) {
        cairo_rectangle(boxCr, (*(frame + 1))->x + pointRadius / sqrt(2),
                (*(frame + 1))->y + pointRadius / sqrt(2), lineLength - pointRadius *
                sqrt(2), lineLength - pointRadius * sqrt(2));
        cairo_fill(boxCr);
        (*(frame + 1))->boxFlag = boxOwner;
        (**boxCount)++;
    }

    return;
}

/*
 * Returns number of points in the linked list
 */
int
find_points_in_list(PointList *head) {
    int count = 0;
    while (head) {
        count++;
        head = head->next;
    }

    return count;
}

/*
 * Deletes the index-th (starts from 1) element & returns the head
 */
PointList *
delete_indexed_point_from_list(PointList *list, int index) {
    PointList *temp = list;
    if (index == 1) {
        list = temp->next;
        free(temp);
        return list;
    }
    for (int i = 1; i < index - 1; i++)
        temp = temp->next;
    PointList *_temp = temp->next;
    temp->next = _temp->next;
    free(_temp);

    return list;
}

/*
 * Returns the indexed point from the list
 * (index is between 1 and number of points in list)
 */
PointList *
find_indexed_point(PointList *list, int index) {
    for (int i = 1; i < index; i++)
        list = list->next;
    return list;
}

void
delay(gdouble miliSeconds) {
    GTimer *timer;
    gdouble timeElasped = 0;
    timer = g_timer_new();
    do {
        timeElasped = g_timer_elapsed(timer, NULL);
    } while (timeElasped * 1000 < miliSeconds);
    g_timer_stop(timer);
    g_timer_destroy(timer);

    return;
}

Frame *
easy_search(Frame *head) {
    PointList *list, *tempList;
    Frame *retPoint = NULL;
    list = find_boxes_with_n_filled_sides(head, 2);
    if (list) {
        int index = genRand(find_points_in_list(list));
        tempList = find_indexed_point(list, index);
        retPoint = tempList->point;
        free(list);
    }

    return retPoint;
}

/*
 * Returns the upper left vertex of the starting box
 * of the chain of non diagonal adjacent boxes all
 * having number of sides equals to 2.
 */
Frame *
medium_search(Frame *head) {
    int minBox = 0;
    Frame *returnPoint = NULL;
    Frame *temp = head;
    while (temp->down) {
        head = temp;
        while (temp->right) {
            if (find_side_count_of_box(temp) == 2) {
                Frame *seed = temp;
                int loopCount = 1;
                int boxCount = 1;
                /*
                 * horizontal right pass
                 */
                while (!temp->right->verticalDown && temp->right->right) {
                    if (find_side_count_of_box(temp->right) == 2) {
                        temp = temp->right;
                        loopCount++;
                        boxCount++;
                    } else break;
                }
                temp = seed;
                /*
                 * horizontal left pass
                 */
                while (!temp->verticalDown && temp->left) {
                    if (find_side_count_of_box(temp->left) == 2) {
                        temp = temp->left;
                        loopCount++;
                        boxCount++;
                    } else break;
                }
                Frame *_temp;
                for (int i = 0; i < loopCount; i++) {
                    _temp = temp;
                    /*
                     * vertical up pass
                     */
                    while (!temp->horizontalRight && temp->up) {
                        if (find_side_count_of_box(temp->up) == 2) {
                            temp = temp->up;
                            boxCount++;
                        } else break;
                    }
                    temp = _temp;
                    /*
                     * vertical down pass
                     */
                    while (!temp->down->horizontalRight && temp->down->down) {
                        if (find_side_count_of_box(temp->down) == 2) {
                            temp = temp->down;
                            boxCount++;
                        } else break;
                    }
                    temp = _temp->right;
                }
                if (!minBox) {
                    minBox = boxCount;
                    returnPoint = seed;
                } else {
                    if (minBox > boxCount) {
                        minBox = boxCount;
                        returnPoint = seed;
                    }
                }
                temp = seed;
            }
            temp = temp->right;
        }
        temp = head->down;
    }
    return returnPoint;
}

void
make_all_boxes_not_visited(Frame *head) {
    Frame *temp = head;
    while (temp->down) {
        head = temp;
        while (temp->right) {
            temp->visit = not_visited;
            temp = temp->right;
        }
        temp = head->down;
    }

    return;
}

void
chain_counter(Frame *point) {
    point->visit = visited;
    boxChainCount++;
    /*
     * vertical up pass
     */
    if (!point->horizontalRight && point->up)
        if (find_side_count_of_box(point->up) == 2 && point->up->visit != visited)
            chain_counter(point->up);
    /*
     * horizontal right pass
     */
    if (!point->right->verticalDown && point->right->right)
        if (find_side_count_of_box(point->right) == 2 && point->right->visit != visited)
            chain_counter(point->right);
    /*
     * vertical down pass
     */
    if (!point->down->horizontalRight && point->down->down)
        if (find_side_count_of_box(point->down) == 2 && point->down->visit != visited)
            chain_counter(point->down);
    /*
     * horizontal left pass
     */
    if (!point->verticalDown && point->left)
        if (find_side_count_of_box(point->left) == 2 && point->left->visit != visited)
            chain_counter(point->left);

    return;
}

Frame *
advanced_search(Frame *head) {
    int minBox = 0;
    Frame *temp, *_temp, *returnPoint = NULL;
    temp = head;
    while (temp->down) {
        _temp = temp;
        while (temp->right) {
            if (find_side_count_of_box(temp) == 2) {
                boxChainCount = 0;
                make_all_boxes_not_visited(head);
                chain_counter(temp);
                if (!minBox) {
                    minBox = boxChainCount;
                    returnPoint = temp;
                } else {
                    if (minBox > boxChainCount) {
                        minBox = boxChainCount;
                        returnPoint = temp;
                    }
                }
            }
            temp = temp->right;
        }
        temp = _temp->down;
    }
    return returnPoint;
}

Frame **
computerTurn(Frame *head, cairo_t *lineCr, cairo_t *boxCr, cairo_t *pointCr,
        int lineLength, int pointRadius, unsigned short *boxCount, BoxOwner boxOwner,
        GtkWidget **event_box, Difficulty difficulty) {
    PointList *list, *tempList;
    Frame **returnPoints = (Frame **) calloc(2, sizeof (Frame *));

    delay(waitInterval);
    list = find_boxes_with_n_filled_sides(head, 3);
    while (list) {
        drawBox(lineCr, boxCr, pointCr, list->point, lineLength, pointRadius,
                boxOwner, &boxCount);
        drawing_area_expose_event_handler(*event_box);
        delay(waitInterval);
        free(list);
        list = find_boxes_with_n_filled_sides(head, 3);
    }

    if (isFrameFull(head))
        return NULL;

    list = find_boxes_with_n_filled_sides(head, 0);
    while (list) {
        int index = genRand(find_points_in_list(list));
        tempList = find_indexed_point(list, index);
        switch (find_suitable_side_of_box(tempList->point)) {
            case TOP:
                tempList->point->horizontalRight = tempList->point->right->horizontalLeft = true;
                *returnPoints = tempList->point;
                *(returnPoints + 1) = tempList->point->right;
                return returnPoints;
            case RIGHT:
                tempList->point->right->verticalDown = tempList->point->right->down->verticalUp = true;
                *returnPoints = tempList->point->right;
                *(returnPoints + 1) = tempList->point->right->down;
                return returnPoints;
            case BOTTOM:
                tempList->point->down->horizontalRight = tempList->point->down->right->horizontalLeft = true;
                *returnPoints = tempList->point->down;
                *(returnPoints + 1) = tempList->point->down->right;
                return returnPoints;
            case LEFT:
                tempList->point->verticalDown = tempList->point->down->verticalUp = true;
                *returnPoints = tempList->point;
                *(returnPoints + 1) = tempList->point->down;
                return returnPoints;
            default:
                break;
        }
        list = delete_indexed_point_from_list(list, index);
    }
    if (list)
        free(list);

    list = find_boxes_with_n_filled_sides(head, 1);
    while (list) {
        int index = genRand(find_points_in_list(list));
        tempList = find_indexed_point(list, index);
        switch (find_suitable_side_of_box(tempList->point)) {
            case TOP:
                tempList->point->horizontalRight = tempList->point->right->horizontalLeft = true;
                *returnPoints = tempList->point;
                *(returnPoints + 1) = tempList->point->right;
                return returnPoints;
            case RIGHT:
                tempList->point->right->verticalDown = tempList->point->right->down->verticalUp = true;
                *returnPoints = tempList->point->right;
                *(returnPoints + 1) = tempList->point->right->down;
                return returnPoints;
            case BOTTOM:
                tempList->point->down->horizontalRight = tempList->point->down->right->horizontalLeft = true;
                *returnPoints = tempList->point->down;
                *(returnPoints + 1) = tempList->point->down->right;
                return returnPoints;
            case LEFT:
                tempList->point->verticalDown = tempList->point->down->verticalUp = true;
                *returnPoints = tempList->point;
                *(returnPoints + 1) = tempList->point->down;
                return returnPoints;
            default:
                break;
        }
        list = delete_indexed_point_from_list(list, index);
    }
    if (list)
        free(list);

    if (difficulty == easy)
        *returnPoints = easy_search(head);
    else if (difficulty == medium)
        *returnPoints = medium_search(head);
    else
        *returnPoints = advanced_search(head);

    if (!(*returnPoints)->horizontalRight)
        *(returnPoints + 1) = (*returnPoints)->right;
    else if (!(*returnPoints)->verticalDown)
        *(returnPoints + 1) = (*returnPoints)->down;
    else if (!(*returnPoints)->right->verticalDown) {
        *returnPoints = (*returnPoints)->right;
        *(returnPoints + 1) = (*returnPoints)->down;
    } else {
        *returnPoints = (*returnPoints)->down;
        *(returnPoints + 1) = (*returnPoints)->right;
    }
    return returnPoints;
}
