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

char *
file_to_string(const char *file) {
    FILE *fp;
    char ch, *str = NULL;
    long long char_count = 0;

    fp = fopen(file, "rb");
    do {
        str = (char *) realloc(str, sizeof (char) * (char_count + 1));
        ch = fgetc(fp);
        *(str + char_count) = ch;
        char_count++;
    } while (ch != EOF);
    fclose(fp);
    *(str + --char_count) = '\0';
    return str;
}

void
show_about_dialogue() {
    const gchar *author[] = {"Dibyendu Das <dibyendu.das.in@gmail.com>",
        NULL};
    char *license = file_to_string(LICENSE_FILE_PATH);
    GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(ICON_PATH, NULL);
    pixbuf = gdk_pixbuf_scale_simple(pixbuf, 48, 48, GDK_INTERP_BILINEAR);
    GtkWidget *about = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(about), "GSquares");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(about), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(about), "(c) 2010-2011 Dibyendu Das");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(about),
            "Take it in turns to draw lines.\nIf you complete a square,\nyou get another go.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(about), "https://sourceforge.net/projects/gsquare/");
    gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(about), license);
    gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(about), "Get GSquares");
    gtk_about_dialog_set_authors(GTK_ABOUT_DIALOG(about), author);
    gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(about), pixbuf);
    gtk_window_set_icon(GTK_WINDOW(about), pixbuf);
    gtk_dialog_run(GTK_DIALOG(about));
    gtk_widget_destroy(about);

    return;
}
