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

enum {
    COL_PLAYER = 0,
    COL_BOX,
    NUM_COLS
};

typedef enum {
    RESULT = 0,
    NO_RESULT
} Game_State;

const int window_width = 300, window_height = 410;

char *winner, *looser;
GtkWidget *label, *image;
GdkPixbuf *happy, *sad, *tie;
unsigned short gamePoint;

void
change_selection(GtkTreeView *tree_view, gpointer data) {

    GtkTreeSelection *selection;
    GtkTreeModel *model;
    GtkTreeIter iter;
    char *value, markup_string[2000];
    selection = gtk_tree_view_get_selection(tree_view);

    if (gtk_tree_selection_get_selected(selection, &model, &iter)) {
        gtk_tree_model_get(model, &iter, COL_PLAYER, &value, -1);
        Game_State *state = (Game_State *) data;
        if (*state == NO_RESULT) {
            gtk_image_set_from_pixbuf(GTK_IMAGE(image), GDK_PIXBUF(tie));
            return;
        } else {
            if (!strcmp(value, winner)) {
                gtk_image_set_from_pixbuf(GTK_IMAGE(image), GDK_PIXBUF(happy));
                sprintf(markup_string, "<span font=\"%s\" foreground=\"#439850\" "
                        "weight=\"ultrabold\" style=\"oblique\">%s\n</span><span "
                        "font=\"%s\" foreground=\"#000000\" "
                        "weight=\"bold\">Point <i>%u</i></span>",
                        RESULT_FONT, winner, RESULT_FONT, gamePoint);
            } else {
                gtk_image_set_from_pixbuf(GTK_IMAGE(image), GDK_PIXBUF(sad));
                sprintf(markup_string, "<span font=\"%s\" foreground=\"#FF0000\" "
                        "weight=\"ultrabold\" style=\"oblique\">%s\n</span><span "
                        "font=\"%s\" foreground=\"#000000\" "
                        "weight=\"bold\">Point <i>%u</i></span>",
                        RESULT_FONT, looser, RESULT_FONT, 0);
            }
        }
        gtk_label_set_markup(GTK_LABEL(label), markup_string);
        gtk_widget_show(image);
        gtk_widget_show(label);

        g_free(value);
    }
    return;
}

gboolean
delete_event_handler() {
    return TRUE;
}

static GtkTreeModel *
create_and_fill_model(unsigned short player1Box, unsigned short player2Box) {
    GtkListStore *store;
    GtkTreeIter iter;
    unsigned short winnerBox = player1Box >= player2Box ? player1Box : player2Box;
    unsigned short looserBox = player1Box < player2Box ? player1Box : player2Box;

    gamePoint = (winnerBox - looserBox) * 2;

    store = gtk_list_store_new(NUM_COLS, G_TYPE_STRING, G_TYPE_UINT);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, COL_PLAYER, winner, COL_BOX, winnerBox, -1);

    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, COL_PLAYER, looser, COL_BOX, looserBox, -1);

    return GTK_TREE_MODEL(store);
}

static GtkWidget *
create_view_and_model(unsigned short player1Box, unsigned short player2Box) {
    GtkCellRenderer *renderer;
    GtkTreeModel *model;
    GtkWidget *view;
    Game_State game_state = player1Box == player2Box ? NO_RESULT : RESULT;

    view = gtk_tree_view_new();

    gtk_tree_view_set_hover_selection(GTK_TREE_VIEW(view), TRUE);
    gtk_tree_view_set_grid_lines(GTK_TREE_VIEW(view), GTK_TREE_VIEW_GRID_LINES_BOTH);

    g_signal_connect(GTK_TREE_VIEW(view), "cursor-changed",
            G_CALLBACK(change_selection), (gpointer) & game_state);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1,
            "Player", renderer, "text", COL_PLAYER, NULL);

    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(view), -1,
            "No of Box", renderer, "text", COL_BOX, NULL);

    model = create_and_fill_model(player1Box, player2Box);

    gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
    g_object_unref(model);

    return view;
}

int
result_message(char *player1, char *player2, unsigned short player1Box, unsigned
        short player2Box) {
    GtkWidget *window, *view, *vbox, *button, *event_box, *hbox;
    char markup_string[2000];

    if (player1Box >= player2Box) {
        winner = (char *) calloc(strlen(player1) + 1, sizeof (char));
        looser = (char *) calloc(strlen(player2) + 1, sizeof (char));
        strcpy(winner, player1);
        strcpy(looser, player2);
    } else {
        winner = (char *) calloc(strlen(player2) + 1, sizeof (char));
        looser = (char *) calloc(strlen(player1) + 1, sizeof (char));
        strcpy(winner, player2);
        strcpy(looser, player1);
    }

    happy = gdk_pixbuf_new_from_file(HAPPY_IMAGE_PATH, NULL);
    sad = gdk_pixbuf_new_from_file(SAD_IMAGE_PATH, NULL);
    tie = gdk_pixbuf_new_from_file(TIE_IMAGE_PATH, NULL);
    happy = gdk_pixbuf_scale_simple(happy, 140, 140, GDK_INTERP_BILINEAR);
    sad = gdk_pixbuf_scale_simple(sad, 140, 140, GDK_INTERP_BILINEAR);
    tie = gdk_pixbuf_scale_simple(tie, 220, 240, GDK_INTERP_BILINEAR);

    image = gtk_image_new();

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_widget_set_size_request(window, window_width, window_height);
    gtk_window_set_modal(GTK_WINDOW(window), TRUE);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_PATH, NULL);
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 48, 48, GDK_INTERP_BILINEAR);
    gtk_window_set_icon(GTK_WINDOW(window), pixbuf);
    gtk_window_set_title(GTK_WINDOW(window), "Result - GSquares");
    gtk_container_set_border_width(GTK_CONTAINER(window), 20);

    g_signal_connect(window, "delete-event", G_CALLBACK(delete_event_handler),
            NULL);

    event_box = gtk_event_box_new();
    label = gtk_label_new(NULL);
    view = create_view_and_model(player1Box, player2Box);

    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(label), TRUE);

    vbox = gtk_vbox_new(FALSE, 2);

    button = gtk_button_new_with_label("Close");
    gtk_widget_set_size_request(button, 80, 35);
    GdkColor *buttonColor = (GdkColor *) calloc(1, sizeof (GdkColor));
    gdk_color_parse("#F1F448", buttonColor);
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_modify_bg(button, GTK_STATE_NORMAL, buttonColor);
    hbox = gtk_hbox_new(FALSE, 10);
    gtk_box_pack_end(GTK_BOX(hbox), button, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox), view, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), label, FALSE, FALSE, 2);
    gtk_box_pack_start(GTK_BOX(vbox), event_box, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 2);

    gtk_container_add(GTK_CONTAINER(event_box), image);

    gtk_container_add(GTK_CONTAINER(window), vbox);

    Game_State state = player1Box == player2Box ? NO_RESULT : RESULT;
    if (state == NO_RESULT)
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), GDK_PIXBUF(tie));
    else {
        gtk_image_set_from_pixbuf(GTK_IMAGE(image), GDK_PIXBUF(happy));
        sprintf(markup_string, "<span font=\"%s\" foreground=\"#439850\" "
                "weight=\"ultrabold\" style=\"oblique\">%s\n</span><span "
                "font=\"%s\" foreground=\"#000000\" "
                "weight=\"bold\">Point <i>%u</i></span>",
                RESULT_FONT, winner, RESULT_FONT, abs(player1Box - player2Box) * 2);
        gtk_label_set_markup(GTK_LABEL(label), markup_string);
    }

    gtk_widget_show_all(window);
    gtk_main();

    gtk_widget_destroy(window);
    free(buttonColor);

    return 0;
}
