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

#define PLAYER_NAME_MAX_LENGTH 20

unsigned int xMin, xMax, yMin, yMax;

/*
 * Checks whether the click is within the
 * frame boundary
 */
bool
isClickInsideFrame(int x, int y) {
    return ((x < xMin || x > xMax) || (y < yMin || y > yMax)) ? false : true;
}

/*
 * The call back signal handlers for dataInitializer
 */
gboolean
window_delete(GtkWidget *widget, gpointer gdata) {
    return TRUE;
}

gboolean
parse_user_data(GtkWidget *widget, gpointer gdata) {
    Data *data = (Data *) gdata;


    if (!gtk_entry_get_text_length(GTK_ENTRY(data->player1Entry)) ||
            !gtk_entry_get_text_length(GTK_ENTRY(data->player2Entry))) {
        GdkColor *baseColor = (GdkColor *) calloc(1, sizeof (GdkColor)),
                *textColor = (GdkColor *) calloc(1, sizeof (GdkColor));
        gdk_color_parse(INVALID_BASE_COLOR, baseColor);
        gdk_color_parse(INVALID_TEXT_COLOR, textColor);
        if (!gtk_entry_get_text_length(GTK_ENTRY(data->player1Entry))) {
            gtk_widget_modify_base((data->player1Entry), GTK_STATE_NORMAL,
                    baseColor);
            gtk_widget_modify_text((data->player1Entry), GTK_STATE_NORMAL,
                    textColor);
            gtk_entry_set_text(GTK_ENTRY(data->player1Entry), "Invalid Name");
        }
        if (!gtk_entry_get_text_length(GTK_ENTRY(data->player2Entry))) {
            gtk_widget_modify_base((data->player2Entry), GTK_STATE_NORMAL,
                    baseColor);
            gtk_widget_modify_text((data->player2Entry), GTK_STATE_NORMAL,
                    textColor);
            gtk_entry_set_text(GTK_ENTRY(data->player2Entry), "Invalid Name");
        }
        free(baseColor);
        free(textColor);
        return FALSE;
    }
    strcpy(data->player1Name, (const char *) gtk_entry_get_text(GTK_ENTRY
            (data->player1Entry)));
    strcpy(data->player2Name, (const char *) gtk_entry_get_text(GTK_ENTRY
            (data->player2Entry)));

    if (!strcmp(data->player1Name, "Invalid Name") ||
            !strcmp(data->player2Name, "Invalid Name"))
        return FALSE;
    if (!strcmp(data->player1Name, data->player2Name)) {
        strcat(data->player1Name, " (Player 1)");
        strcat(data->player2Name, " (Player 2)");
    }
    data->horizontalLines = atoi((char *) gtk_combo_box_get_active_text
            (GTK_COMBO_BOX(data->comboBoxHorizontal)));
    data->verticalLines = atoi((char *) gtk_combo_box_get_active_text
            (GTK_COMBO_BOX(data->comboBoxVertical)));
    data->opponent = (GTK_TOGGLE_BUTTON(data->radioButtonComputer))->active ?
            COMPUTER : HUMAN;
    if (data->opponent == COMPUTER) {
        char *diff;
        diff = gtk_combo_box_get_active_text(GTK_COMBO_BOX(data->difficulty_combo));
        if (!strcmp(diff, "Easy"))
            data->difficulty = easy;
        else if (!strcmp(diff, "Medium"))
            data->difficulty = medium;
        else data->difficulty = hard;
    }
    gtk_main_quit();
    return TRUE;
}

gboolean
toggle_event(GtkWidget *widget, gpointer gdata) {
    Data *data = (Data *) gdata;
    GdkColor *textColor = (GdkColor *) calloc(1, sizeof (GdkColor)),
            *defaultTextColor = (GdkColor *) calloc(1, sizeof (GdkColor)),
            *baseColor = (GdkColor *) calloc(1, sizeof (GdkColor));
    gdk_color_parse(TEXT_ENTRY_NON_EDITABLE_TEXT_COLOR, textColor);
    gdk_color_parse(TEXT_ENTRY_DEFAULT_TEXT_COLOR, defaultTextColor);
    gdk_color_parse(TEXT_ENTRY_DEFAULT_BASE_COLOR, baseColor);

    if (GTK_TOGGLE_BUTTON(widget)->active) {
        gtk_combo_box_set_button_sensitivity(GTK_COMBO_BOX(data->difficulty_combo),
                GTK_SENSITIVITY_ON);
        strcpy(data->player2Name, (const char *) gtk_entry_get_text(GTK_ENTRY
                (data->player2Entry)));
        gtk_widget_modify_text(data->player2Entry, GTK_STATE_NORMAL,
                textColor);
        gtk_widget_modify_base(data->player2Entry, GTK_STATE_NORMAL,
                baseColor);
        gtk_entry_set_text(GTK_ENTRY(data->player2Entry), "computer");
        gtk_entry_set_editable(GTK_ENTRY(data->player2Entry), FALSE);
    } else {
        gtk_combo_box_set_button_sensitivity(GTK_COMBO_BOX(data->difficulty_combo),
                GTK_SENSITIVITY_OFF);
        gtk_widget_modify_text(data->player2Entry, GTK_STATE_NORMAL,
                defaultTextColor);
        if (strcmp(data->player2Name, "computer") &&
                strcmp(data->player2Name, "Invalid Name"))
            gtk_entry_set_text(GTK_ENTRY(data->player2Entry),
                data->player2Name);
        else
            gtk_entry_set_text(GTK_ENTRY(data->player2Entry), "");
        gtk_entry_set_editable(GTK_ENTRY(data->player2Entry), TRUE);
    }
    free(textColor);
    free(defaultTextColor);
    free(baseColor);
    return TRUE;
}

gboolean
focus_text_entry_event(GtkEntry *entry, gpointer gdata) {
    const gchar *text = gtk_entry_get_text(entry);

    if (!(strcmp((char *) text, "Invalid Name"))) {
        GdkColor *baseColor = (GdkColor *) calloc(1, sizeof (GdkColor));
        GdkColor *defaultTextColor = (GdkColor *) calloc(1, sizeof (GdkColor));
        gdk_color_parse(TEXT_ENTRY_DEFAULT_BASE_COLOR, baseColor);
        gdk_color_parse(TEXT_ENTRY_DEFAULT_TEXT_COLOR, defaultTextColor);
        gtk_widget_modify_base(GTK_WIDGET(entry), GTK_STATE_NORMAL, baseColor);
        gtk_widget_modify_text(GTK_WIDGET(entry), GTK_STATE_NORMAL,
                defaultTextColor);
        gtk_entry_set_text(entry, "");
        gtk_entry_set_editable(entry, TRUE);
        free(defaultTextColor);
        free(baseColor);
    }
    return TRUE;
}

gboolean
destroy(GtkWidget *widget, Data *data) {
    gtk_widget_destroy(data->window);
    free(data->player1Name);
    free(data->player2Name);
    g_slice_free(Data, data);

    exit(EXIT_SUCCESS);
}

bool
dataInitializer(char **player1, char **player2, const int *horizontalLines,
        const int *verticalLines, OPPONENT_TYPE *opponent, Difficulty *difficulty) {
    Data *data = g_slice_new0(Data);
    GtkWidget *vbox_root, *hbox_root, *vseparator, *hseparator,
            *quit_button, *apply_button, *label;
    PangoFontDescription *font = pango_font_description_from_string
            (TEXT_ENTRY_FONT);
    char buffer[3];

    data->player1Name = (char *) calloc(21, sizeof (char));
    data->player2Name = (char *) calloc(21, sizeof (char));

    data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    vbox_root = gtk_vbox_new(FALSE, 10);

    gtk_window_set_default_size(GTK_WINDOW(data->window), 600, 400);
    gtk_window_set_position(GTK_WINDOW(data->window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(data->window), FALSE);
    gtk_window_set_title(GTK_WINDOW(data->window), "New Game - GSquares");
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_PATH, NULL);
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 48, 48, GDK_INTERP_BILINEAR);
    gtk_window_set_icon(GTK_WINDOW(data->window), pixbuf);
    gtk_container_set_border_width(GTK_CONTAINER(data->window), 10);
    gtk_container_add(GTK_CONTAINER(data->window), vbox_root);

    data->comboBoxHorizontal = gtk_combo_box_new_text();
    data->comboBoxVertical = gtk_combo_box_new_text();
    data->difficulty_combo = gtk_combo_box_new_text();

    gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(data->comboBoxHorizontal), 4);
    gtk_combo_box_set_wrap_width(GTK_COMBO_BOX(data->comboBoxVertical), 4);

    for (int i = 1; i <= 20; i++) {
        sprintf(buffer, "%d", i);
        gtk_combo_box_append_text(GTK_COMBO_BOX(data->comboBoxHorizontal),
                buffer);
        gtk_combo_box_append_text(GTK_COMBO_BOX(data->comboBoxVertical),
                buffer);
    }
    gtk_combo_box_append_text(GTK_COMBO_BOX(data->difficulty_combo), "Easy");
    gtk_combo_box_append_text(GTK_COMBO_BOX(data->difficulty_combo), "Medium");
    gtk_combo_box_append_text(GTK_COMBO_BOX(data->difficulty_combo), "Hard");

    gtk_combo_box_set_active(GTK_COMBO_BOX(data->comboBoxHorizontal), 3);
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->comboBoxVertical), 3);
    gtk_combo_box_set_active(GTK_COMBO_BOX(data->difficulty_combo), 2);
    gtk_combo_box_set_button_sensitivity(GTK_COMBO_BOX(data->difficulty_combo),
            GTK_SENSITIVITY_OFF);

    GtkWidget * hbox = gtk_hbox_new(FALSE, 10);
    label = gtk_label_new("Number of Horizontal Line(s)    ");

    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 4);
    gtk_box_pack_start(GTK_BOX(hbox), data->comboBoxHorizontal, TRUE, FALSE, 4);

    gtk_box_pack_start(GTK_BOX(vbox_root), hbox, FALSE, FALSE, 4);

    hbox = gtk_hbox_new(FALSE, 10);
    label = gtk_label_new("Number of Vertical Line(s)       ");

    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 4);
    gtk_box_pack_start(GTK_BOX(hbox), data->comboBoxVertical, TRUE, FALSE, 4);

    gtk_box_pack_start(GTK_BOX(vbox_root), hbox, FALSE, FALSE, 4);

    hseparator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox_root), hseparator, FALSE, FALSE, 5);

    hbox_root = gtk_hbox_new(FALSE, 4);

    GtkWidget *vbox = gtk_vbox_new(TRUE, 4);
    label = gtk_label_new("Player 1 : ");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 5);

    label = gtk_label_new("Player 2 : ");
    gtk_label_set_justify(GTK_LABEL(label), GTK_JUSTIFY_LEFT);
    gtk_box_pack_start(GTK_BOX(vbox), label, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_root), vbox, FALSE, FALSE, 0);

    vbox = gtk_vbox_new(TRUE, 4);

    data->player1Entry = gtk_entry_new();
    data->player2Entry = gtk_entry_new();
    gtk_widget_modify_font(data->player1Entry, font);
    gtk_widget_modify_font(data->player2Entry, font);
    gtk_entry_set_alignment(GTK_ENTRY(data->player1Entry), 0.5);
    gtk_entry_set_alignment(GTK_ENTRY(data->player2Entry), 0.5);
    gtk_entry_set_max_length(GTK_ENTRY(data->player1Entry),
            PLAYER_NAME_MAX_LENGTH);
    gtk_entry_set_max_length(GTK_ENTRY(data->player2Entry),
            PLAYER_NAME_MAX_LENGTH);
    gtk_entry_set_text(GTK_ENTRY(data->player1Entry), getenv("USER"));
    gtk_box_pack_start(GTK_BOX(vbox), data->player1Entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), data->player2Entry, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_root), vbox, FALSE, FALSE, 0);

    vseparator = gtk_vseparator_new();
    gtk_box_pack_start(GTK_BOX(hbox_root), vseparator, FALSE, FALSE, 5);

    label = gtk_label_new("Opponent ");
    gtk_box_pack_start(GTK_BOX(hbox_root), label, FALSE, FALSE, 5);

    vbox = gtk_vbox_new(TRUE, 4);
    data->radioButtonHuman = gtk_radio_button_new_with_label(NULL, "Human");
    data->radioButtonComputer = gtk_radio_button_new_with_label_from_widget
            (GTK_RADIO_BUTTON(data->radioButtonHuman), "Computer");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(data->radioButtonHuman),
            TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), data->radioButtonHuman, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(vbox), data->radioButtonComputer, TRUE, TRUE, 5);
    gtk_box_pack_start(GTK_BOX(hbox_root), vbox, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(vbox_root), hbox_root, FALSE, FALSE, 5);

    hseparator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox_root), hseparator, FALSE, FALSE, 5);

    hbox = gtk_hbox_new(TRUE, 10);

    label = gtk_label_new("AI Difficulty ");
    gtk_box_pack_start(GTK_BOX(hbox), label, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(hbox), data->difficulty_combo, TRUE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(vbox_root), hbox, FALSE, FALSE, 5);

    hseparator = gtk_hseparator_new();
    gtk_box_pack_start(GTK_BOX(vbox_root), hseparator, FALSE, FALSE, 5);

    hbox = gtk_hbox_new(FALSE, 10);

    apply_button = gtk_button_new_with_label("Apply");
    quit_button = gtk_button_new_with_label("Quit");
    gtk_widget_set_size_request(apply_button, 80, 35);
    gtk_widget_set_size_request(quit_button, 80, 35);
    gtk_box_pack_end(GTK_BOX(hbox), quit_button, FALSE, TRUE, 10);
    gtk_box_pack_end(GTK_BOX(hbox), apply_button, FALSE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(vbox_root), hbox, FALSE, FALSE, 5);


    g_signal_connect(data->window, "delete-event",
            G_CALLBACK(window_delete), NULL);
    g_signal_connect(apply_button, "clicked",
            G_CALLBACK(parse_user_data), (gpointer) data);
    g_signal_connect(quit_button, "clicked",
            G_CALLBACK(destroy), (gpointer) data);
    g_signal_connect(data->radioButtonComputer, "toggled",
            G_CALLBACK(toggle_event), (gpointer) data);
    g_signal_connect(GTK_ENTRY(data->player1Entry), "focus-in-event",
            G_CALLBACK(focus_text_entry_event), NULL);
    g_signal_connect(GTK_ENTRY(data->player2Entry), "focus-in-event",
            G_CALLBACK(focus_text_entry_event), NULL);

    gtk_widget_show_all(data->window);
    gtk_main();

    memmove((void *) horizontalLines, (void *) &(data->horizontalLines), sizeof
            (int));
    memmove((void *) verticalLines, (void *) &(data->verticalLines), sizeof
            (int));
    *player1 = (char *) calloc(strlen(data->player1Name) + 1, sizeof (char));
    *player2 = (char *) calloc(strlen(data->player2Name) + 1, sizeof (char));
    strcpy(*player1, data->player1Name);
    strcpy(*player2, data->player2Name);
    *opponent = data->opponent;
    if (data->opponent == COMPUTER)
        *difficulty = data->difficulty;

    gtk_widget_destroy(data->window);

    pango_font_description_free(font); 
    free(data->player1Name);
    free(data->player2Name);
    g_slice_free(Data, data);
    //free(pixbuf);
    
    return true;
}

bool
setFrameAttribute(int drawingAreaWidth, int drawingAreaHeight,
        int horizontalLines, int verticalLines, const int *startX,
        const int *startY, const int *lineLength, const int *pointRadius) {
    double gap[20] = {4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8,
        8};
    int maxLines = horizontalLines > verticalLines ? horizontalLines :
            verticalLines;
    int temp = drawingAreaWidth > drawingAreaHeight ? (drawingAreaHeight -
            drawingAreaHeight / gap[maxLines - 1]) / verticalLines :
            (drawingAreaWidth - drawingAreaWidth / gap[maxLines - 1]) /
            horizontalLines;
    int maxLength = drawingAreaWidth > drawingAreaHeight ? horizontalLines :
            verticalLines;
    int check = drawingAreaWidth > drawingAreaHeight ? drawingAreaWidth -
            drawingAreaWidth / gap[maxLines - 1] : drawingAreaHeight -
            drawingAreaHeight / gap[maxLines - 1];
    while (temp * maxLength >= check)
        --temp;
    memmove((void *) lineLength, (void *) &temp, sizeof (int));
    temp = (drawingAreaHeight - (*lineLength) * verticalLines) / 2;
    memmove((void *) startY, (void *) &temp, sizeof (int));
    temp = (drawingAreaWidth - (*lineLength) * horizontalLines) / 2;
    memmove((void *) startX, (void *) &temp, sizeof (int));
    if (maxLines < 10)
        temp = (*lineLength) / 16;
    else if (maxLines >= 10 && maxLines <= 15)
        temp = (*lineLength) / 10;
    else
        temp = (*lineLength) / 8;
    memmove((void *) pointRadius, (void *) &temp, sizeof (int));
    xMin = *startX;
    xMax = *startX + (horizontalLines * (*lineLength));
    yMin = *startY;
    yMax = *startY + (verticalLines * (*lineLength));

    return true;
}
