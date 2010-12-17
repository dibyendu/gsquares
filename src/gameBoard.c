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

GtkWidget *window, *menu_bar, *vbox, *status_bar, *event_box;

static ExposureType expose = noExpose;
static bool frameInitializeFlag = false;
static gint status_context_id;
static gulong button_press_handler_id;

static unsigned short buttonPressCount = 0, BoxOfPlayer1 = 0,
        BoxOfPlayer2 = 0;

Frame *head = NULL, **computerPoints = NULL;
;

const int horizontalLines, verticalLines, startX, startY, lineLength,
pointRadius;
char *player1Name, *player2Name;
OPPONENT_TYPE opponent;
Difficulty difficulty;

static double fadingLineAlpha = 1;
const double delta = 0.04;
static bool timer = false;
const int fadeTimeInterval = 50;

void
cairo_set_source_rgba_from_string(cairo_t *cr, const char *colour,
        const double alpha) {
    int digit, buffer[6];

    for (int i = 0; i < 6; i++) {
        digit = (int) *(colour + i + 1);
        if (digit >= 48 && digit <= 57)
            buffer[i] = digit - 48;
        else if (digit >= 65 && digit <= 70)
            buffer[i] = digit - 55;
        else
            buffer[i] = digit - 87;
    }

    double red = (buffer[0] * 16 + buffer[1]) / 255;
    double green = (buffer[2] * 16 + buffer[3]) / 255;
    double blue = (buffer[4] * 16 + buffer[5]) / 255;

    cairo_set_source_rgba(cr, red, green, blue, alpha);

    return;
}

void
plotFrame(GtkWidget *widget, Frame *head, int horizontalLines,
        int verticalLines, int pointRadius) {
    Frame *temp = head, *_temp;
    const double dash[4] = {4, 3, 2, 1};
    cairo_t *pointCr = gdk_cairo_create(widget->window),
            *dashedLineCr = gdk_cairo_create(widget->window),
            *lineCr = gdk_cairo_create(widget->window),
            *player1BoxCr = gdk_cairo_create(widget->window),
            *player2BoxCr = gdk_cairo_create(widget->window),
            *fadingCr = gdk_cairo_create(widget->window);

    cairo_set_source_rgba_from_string(pointCr, POINT_COLOR, 1);
    cairo_set_source_rgba_from_string(dashedLineCr, DASHED_LINE_COLOR, 0.5);
    cairo_set_source_rgba_from_string(lineCr, LINE_COLOR, 1);
    cairo_set_source_rgba_from_string(player1BoxCr, PLAYER_1_BOX_COLOR, 1);
    cairo_set_source_rgba_from_string(player2BoxCr, PLAYER_2_BOX_COLOR, 1);

    cairo_set_line_width(dashedLineCr, pointRadius * 0.4);
    cairo_set_line_width(lineCr, pointRadius * sqrt(2));
    cairo_set_dash(dashedLineCr, dash, 4, 3);

    if (timer && computerPoints) {
        double lineWidth = pointRadius * sqrt(2);
        if (fadingLineAlpha < 0) {
            fadingLineAlpha = 0;
            timer = false;
        }
        cairo_set_source_rgba_from_string(fadingCr, FADE_LINE_COLOR, fadingLineAlpha);
        cairo_set_line_width(fadingCr, 3 * lineWidth);
        cairo_set_line_cap(fadingCr, CAIRO_LINE_CAP_ROUND);
        cairo_move_to(fadingCr, (*computerPoints)->x, (*computerPoints)->y);
        cairo_line_to(fadingCr, (*(computerPoints + 1))->x, (*(computerPoints + 1))->y);
        cairo_stroke(fadingCr);
        fadingLineAlpha -= delta;
        cairo_destroy(fadingCr);
    }

    for (int i = 0; i <= verticalLines; i++) {
        _temp = temp;
        for (int j = 0; j <= horizontalLines; j++) {
            if (temp->right) {
                if (!temp->horizontalRight) {
                    cairo_move_to(dashedLineCr, temp->x, temp->y);
                    cairo_line_to(dashedLineCr, temp->right->x, temp->y);
                    cairo_stroke(dashedLineCr);
                } else {
                    cairo_move_to(lineCr, temp->x, temp->y);
                    cairo_line_to(lineCr, temp->right->x, temp->y);
                    cairo_stroke(lineCr);
                }
            }
            cairo_arc(pointCr, temp->x, temp->y, pointRadius, 0, 2 * M_PI);
            cairo_fill(pointCr);
            temp = temp->right;
        }
        temp = _temp->down;
    }
    _temp = temp = head;
    for (int i = 0; i <= horizontalLines; i++) {
        temp = _temp;
        for (int j = 0; j < verticalLines; j++) {
            if (!temp->verticalDown) {
                cairo_move_to(dashedLineCr, temp->x, temp->y);
                cairo_line_to(dashedLineCr, temp->down->x, temp->down->y);
                cairo_stroke(dashedLineCr);
            } else {
                cairo_move_to(lineCr, temp->x, temp->y);
                cairo_line_to(lineCr, temp->down->x, temp->down->y);
                cairo_stroke(lineCr);
            }
            cairo_arc(pointCr, temp->x, temp->y, pointRadius, 0, 2 * M_PI);
            cairo_fill(pointCr);
            cairo_arc(pointCr, temp->down->x, temp->down->y, pointRadius, 0,
                    2 * M_PI);
            cairo_fill(pointCr);
            if (temp->boxFlag == firstPlayerBox) {
                cairo_rectangle(player1BoxCr, temp->x + pointRadius / sqrt(2),
                        temp->y + pointRadius / sqrt(2), lineLength - pointRadius *
                        sqrt(2), lineLength - pointRadius * sqrt(2));
                cairo_fill(player1BoxCr);
            }
            if (temp->boxFlag == secondPlayerBox) {
                cairo_rectangle(player2BoxCr, temp->x + pointRadius / sqrt(2),
                        temp->y + pointRadius / sqrt(2), lineLength - pointRadius *
                        sqrt(2), lineLength - pointRadius * sqrt(2));
                cairo_fill(player2BoxCr);
            }
            temp = temp->down;
        }
        _temp = _temp->right;
    }

    cairo_destroy(pointCr);
    cairo_destroy(dashedLineCr);
    cairo_destroy(lineCr);
    cairo_destroy(player1BoxCr);
    cairo_destroy(player2BoxCr);

    return;
}

gboolean
restart_game_menu_event_handler(GtkWidget *widget, gpointer data) {
    free(head);
    head = (Frame *) calloc(1, sizeof (Frame));
    frameInitializeFlag = false;
    buttonPressCount = BoxOfPlayer1 = BoxOfPlayer2 = 0;
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_context_id, player2Name);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_context_id, player1Name);
    gtk_widget_queue_draw(GTK_WIDGET(data));

    return FALSE;
}

gboolean
quit_menu_event_handler() {
    gtk_main_quit();
    return TRUE;
}

gboolean
about_menu_event_handler(GtkWidget *widget, gpointer data) {
    show_about_dialogue();
    return FALSE;
}

void
destroy_handler() {
    exit(EXIT_SUCCESS);
}

gboolean
drawing_area_redraw_on_allocate(GtkWidget *widget) {
    if (expose == firstExpose) {
        expose = secondExposeOnwards;
        gtk_widget_queue_draw(widget);
    }

    return FALSE;
}

GtkWidget *
gtk_create_window(const gchar *icon_file, const gchar *window_name, int width,
        int height, gboolean resizable) {
    GtkWidget *window;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    if (icon_file) {
        GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(icon_file, NULL);
        pixbuf = gdk_pixbuf_scale_simple(pixbuf, 48, 48, GDK_INTERP_BILINEAR);
        gtk_window_set_icon(GTK_WINDOW(window), pixbuf);
    }
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_window_set_title(GTK_WINDOW(window), window_name);
    gtk_window_set_default_size(GTK_WINDOW(window), width, height);
    gtk_window_set_resizable(GTK_WINDOW(window), resizable);

    return window;
}

GtkWidget *
gtk_create_menu_bar(GtkWidget *event_box) {
    GtkWidget *menu_bar, *menu, *menu_items, *action_menu, *help_menu;
    GdkColor color;

    menu_bar = gtk_menu_bar_new();

    menu = gtk_menu_new();

    menu_items = gtk_menu_item_new_with_label("Restart Game");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
    g_signal_connect(menu_items, "activate", G_CALLBACK(restart_game_menu_event_handler),
            (gpointer) event_box);
    menu_items = gtk_separator_menu_item_new();
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
    menu_items = gtk_menu_item_new_with_label("Quit");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
    g_signal_connect(menu_items, "activate", G_CALLBACK(quit_menu_event_handler),
            NULL);
    action_menu = gtk_menu_item_new_with_mnemonic("_Action");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(action_menu), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), action_menu);

    menu = gtk_menu_new();
    menu_items = gtk_menu_item_new_with_label("About");
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_items);
    g_signal_connect(menu_items, "activate", G_CALLBACK(about_menu_event_handler),
            (gpointer) event_box);
    help_menu = gtk_menu_item_new_with_mnemonic("_Help");
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(help_menu), menu);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help_menu);

    gdk_color_parse(MENU_BAR_COLOR, &color);
    gtk_widget_modify_bg(menu_bar, GTK_STATE_NORMAL, &color);

    return menu_bar;
}

GtkWidget *
gtk_create_status_bar(int *context_id, const gchar *context_str, char *msg1,
        char *msg2) {
    GtkWidget *status_bar = gtk_statusbar_new();
    *context_id = gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar),
            context_str);
    gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(status_bar), TRUE);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), *context_id, msg2);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), *context_id, msg1);

    return status_bar;
}

gboolean
fade_out_handler() {
    if (!window->window)
        return FALSE;
    if (!timer) {
        fadingLineAlpha = 1;
        return FALSE;
    }
    gtk_widget_queue_draw(event_box);

    return TRUE;
}

gboolean
drawing_area_expose_event_handler(GtkWidget *widget) {
    if (expose == noExpose) {
        expose = firstExpose;
        g_timeout_add(100, (GSourceFunc) drawing_area_redraw_on_allocate,
                widget);
    }
    if (expose == secondExposeOnwards) {
        setFrameAttribute(widget->allocation.width, widget->allocation.height,
                horizontalLines, verticalLines, &startX, &startY, &lineLength,
                &pointRadius);
        if (!frameInitializeFlag) {
            initFrame(horizontalLines, verticalLines, startX, startY, lineLength,
                    head);
            frameInitializeFlag = true;
        } else
            updateFrame(horizontalLines, verticalLines, startX, startY,
                lineLength, head);
        plotFrame(widget, head, horizontalLines, verticalLines, pointRadius);
    }

    return TRUE;
}

gboolean
button_press_event_handler(GtkWidget *widget, GdkEventButton *event,
        gpointer data) {
    cairo_t *pointCr = gdk_cairo_create(widget->window),
            *lineCr = gdk_cairo_create(widget->window),
            *player1BoxCr = gdk_cairo_create(widget->window),
            *player2BoxCr = gdk_cairo_create(widget->window);

    cairo_set_source_rgba_from_string(pointCr, POINT_COLOR, 1);
    cairo_set_source_rgba_from_string(lineCr, LINE_COLOR, 1);
    cairo_set_source_rgba_from_string(player1BoxCr, PLAYER_1_BOX_COLOR, 1);
    cairo_set_source_rgba_from_string(player2BoxCr, PLAYER_2_BOX_COLOR, 1);

    cairo_set_line_width(lineCr, pointRadius * 1.4);

    gint x = (gint) event->x, y = (gint) event->y;

    if (isClickInsideFrame(x, y)) {

        timer = true;
        fadingLineAlpha = -1;
        gtk_widget_queue_draw(event_box);
        timer = false;
        fadingLineAlpha = 1;

        buttonPressCount %= 2;
        TurnResult result;
        if (!buttonPressCount) {
            result = playerTurn(lineCr, player1BoxCr, pointCr, head, x,
                    y, lineLength, pointRadius, &BoxOfPlayer1, firstPlayerBox);
            if (result == lineDrawn) {
                gtk_statusbar_pop(GTK_STATUSBAR(data), status_context_id);
                if (opponent == HUMAN)
                    buttonPressCount++;
                else {
                    g_signal_handler_block(event_box, button_press_handler_id);
                    computerPoints = computerTurn(head, lineCr, player2BoxCr,
                            pointCr, lineLength, pointRadius, &BoxOfPlayer2,
                            secondPlayerBox, &event_box, difficulty);
                    g_signal_handler_unblock(event_box, button_press_handler_id);
                    if (!computerPoints) {
                        gtk_statusbar_push(GTK_STATUSBAR(data), status_context_id,
                                "Game Over");
                        timer = false;
                        result_message(player1Name, player2Name, BoxOfPlayer1, BoxOfPlayer2);
                    } else {
                        gtk_statusbar_push(GTK_STATUSBAR(data), status_context_id,
                                player1Name);
                        if ((*computerPoints)->x == (*(computerPoints + 1))->x)
                            (*computerPoints)->verticalDown =
                                (*(computerPoints + 1))->verticalUp = true;
                        else
                            (*computerPoints)->horizontalRight =
                                (*(computerPoints + 1))->horizontalLeft = true;
                        timer = true;
                        g_timeout_add(fadeTimeInterval, (GSourceFunc) fade_out_handler, NULL);
                    }
                }
            }
            if (result == frameIsFull) {
                gtk_statusbar_push(GTK_STATUSBAR(data), status_context_id,
                        "Game Over");
                result_message(player1Name, player2Name, BoxOfPlayer1, BoxOfPlayer2);
            }
        } else {
            result = playerTurn(lineCr, player2BoxCr, pointCr, head, x,
                    y, lineLength, pointRadius, &BoxOfPlayer2, secondPlayerBox);
            if (result == lineDrawn) {
                buttonPressCount++;
                gtk_statusbar_push(GTK_STATUSBAR(data), status_context_id,
                        player1Name);
            }
            if (result == frameIsFull) {
                gtk_statusbar_push(GTK_STATUSBAR(data), status_context_id,
                        "Game Over");
                result_message(player1Name, player2Name, BoxOfPlayer1, BoxOfPlayer2);
            }
        }
    }

    cairo_destroy(pointCr);
    cairo_destroy(lineCr);
    cairo_destroy(player1BoxCr);
    cairo_destroy(player2BoxCr);

    return TRUE;
}

bool
gamePlay() {

    dataInitializer(&player1Name, &player2Name, &horizontalLines, &verticalLines,
            &opponent, &difficulty);

    head = (Frame *) calloc(1, sizeof (Frame));

    window = gtk_create_window(ICON_PATH, "GSquares", 600, 600, TRUE);

    event_box = gtk_event_box_new();
    gtk_widget_set_app_paintable(GTK_WIDGET(event_box), TRUE);
    GdkColor color;
    gdk_color_parse(GAME_WINDOW_BG_COLOR, &color);
    gtk_widget_modify_bg(event_box, GTK_STATE_NORMAL, &color);

    status_bar = gtk_create_status_bar(&status_context_id, "Game Context",
            player1Name, player2Name);

    vbox = gtk_vbox_new(FALSE, 0);

    menu_bar = gtk_create_menu_bar(event_box);

    gtk_box_pack_start(GTK_BOX(vbox), menu_bar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), event_box, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(vbox), status_bar, FALSE, FALSE, 0);

    g_signal_connect(event_box, "expose-event",
            G_CALLBACK(drawing_area_expose_event_handler), NULL);
    g_signal_connect(window, "delete-event",
            G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(window, "destroy",
            G_CALLBACK(destroy_handler), NULL);
    button_press_handler_id = g_signal_connect(event_box, "button-press-event",
            G_CALLBACK(button_press_event_handler), (gpointer) status_bar);

    gtk_container_add(GTK_CONTAINER(window), vbox);
    gtk_widget_show_all(window);
    gtk_main();
    gtk_widget_destroy(window);

    return true;
}
