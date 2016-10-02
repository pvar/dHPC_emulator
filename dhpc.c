/*
 * Fundamental components of dhpc emulator -- dhpc_emu.
 *
 * Copyright 2016, Panagiotis Varelas <varelaspanos@gmail.com>
 *
 * dhpc_emu is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * dhpc_emu is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

#include "dhpc.h"

void key_press_event (GtkWidget *widget, GdkEventKey *event);


gint main (int argc, char *argv[])
{
        /* allocate memory for dhpc struct */
        dhpc = g_slice_new(dHPC);

        /* initialize GTK+ libraries */
        gtk_init(&argc, &argv);

        /* build UI from XML file */
        if(build_ui() == FALSE)
                return 1;

        /* display main window */
        gtk_widget_show(dhpc->window);

        /* enter the GTK+ mainloop */
        gtk_main();

        /* free memory allocated to dhpc struct */
        g_slice_free(dHPC, dhpc);

        return 0;
}

gint build_ui (void)
{
        gchar *app_path;
        gchar *xml_path;
        GtkBuilder *appgui;
        GError *error = NULL;

        /* get application path */
        get_current_path(&app_path);
        if((strlen(app_path) < 0 ))
                return FALSE;

        /* construct path to GUI XML */
        xml_path = g_strdup_printf ("%s/%s", app_path, GUIXML);

        /* use GtkBuilder to build interface from XML */
        appgui = gtk_builder_new ();
        if(gtk_builder_add_from_file (appgui, (const gchar *)xml_path, &error) == 0) {
                g_print("GTKBUILDER ERROR: %s\n", error->message);
                g_error_free(error);
                return FALSE;
        }

        /* free path strings */
        g_free(app_path);
        g_free(xml_path);

        /* initialize dhpc-struct members */
        dhpc->window = GTK_WIDGET(gtk_builder_get_object(appgui, "dhpc_window"));
        dhpc->screen = GTK_IMAGE(gtk_builder_get_object(appgui, "dhpc_screen"));

        /* initialize pixel and frame buffer */
        dhpc->framebuffer = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE, 8, WIDTH, HEIGHT);
        dhpc->pixelbuffer = gdk_pixbuf_get_pixels(dhpc->framebuffer);

        /* pixel manipulation example: a white dot in the middle of the screen */
        guchar *pixel = &dhpc->pixelbuffer[(240 * WIDTH + 256) * 3];
        *pixel     = 250;
        *(pixel+1) = 250;
        *(pixel+2) = 250;

        /* display frame buffer on screen */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);

        /* connect destroy event to built-in gtk_main_quit() */
        g_signal_connect(dhpc->window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
        g_signal_connect (dhpc->window, "key_press_event", G_CALLBACK (key_press_event), NULL);
        return TRUE;
}

gint get_current_path (gchar **dir)
{
        gchar full_dir[512];
        gchar proc_lnk[512];
        pid_t pid = getpid();

        sprintf (proc_lnk, "/proc/%d/exe", pid);
        readlink (proc_lnk, full_dir, sizeof(full_dir));

        gchar *full_path_with_bin = g_strdup_printf("%s", full_dir);
        const gchar *app_name = "dhpcemu";
        gchar *bin_pos = g_strrstr(full_path_with_bin, app_name);
        gchar *clear_path = g_strndup(full_path_with_bin, (gsize)bin_pos - (gsize)full_path_with_bin);

        if((strlen(clear_path) > 0)) {
                *dir = g_strdup_printf("%s",clear_path);
                free(clear_path);
                return 0;
        } else {
                perror("Unable to use getcwd()");
                free(clear_path);
                free(full_path_with_bin);
                return -1;
        }
}

void key_press_event (GtkWidget *widget, GdkEventKey *event)
{
        g_print ("%i  ", event->keyval);

}




/* Example of GMutex with convenience macros

G_LOCK_DEFINE (current_number);
int give_me_next_number (void)
{
        static int current_number = 0;
        int ret_val;

        G_LOCK (current_number);
        ret_val = current_number = calc_next_number (current_number);
        G_UNLOCK (current_number);

        return ret_val;
}
*/
