#ifndef DHPC_EMU_H
#define DHPC_EMU_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

#include "thread_cpu.h"
#include "thread_gpu.h"
//#include "thread_apu.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gint get_current_path ( gchar **dir );
gint build_ui ( void );

// ------------------------------------------------------------------------------
// MACROS
// ------------------------------------------------------------------------------

#define GUIXML "dhpcemu.glade"

#define FB_WIDTH 512
#define FB_HEIGHT 480
#define GPU_DATA_LENGTH 8
#define APU_DATA_PACKET 4

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

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

struct packet_to_apu {
        guchar type;
        guchar data[APU_DATA_PACKET];
        gboolean received;
        gboolean new_set;
} apu_data;

G_LOCK_DEFINE (apu_data);

enum {
        GPU_PRINT = 0,
        GPU_LOCATE,
        GPU_PEN,
        GPU_PAPER,
        GPU_PSET,
        GPU_CLEAR,
        GPU_RESET
};

enum {
        APU_STOP = 0,
        APU_PLAY,
        APU_TEMPO,
        APU_INSERT,
        APU_CLEAR,
        APU_ENABLE,
        APU_DISABLE
};

#endif
