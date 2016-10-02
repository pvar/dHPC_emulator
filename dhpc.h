#ifndef _DHPC_EMU_H
#define _DHPC_EMU_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>



gint get_current_path ( gchar **dir );
gint build_ui ( void );


#define GUIXML "dhpcemu.glade"

#define WIDTH 512
#define HEIGHT 480

typedef struct {
        GtkWidget       *window;
        GtkImage        *screen;
        GdkPixbuf       *framebuffer;
        guchar          *pixelbuffer;
        //GArray        *some_array;
        //GtkEntry      *some_entry;
        //GtkLabel      *some_label;
} dHPC;

dHPC *dhpc;

#endif
