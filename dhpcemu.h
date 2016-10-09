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
#define KEYBOARD_BUFFER_SIZE 32

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


struct packet_to_gpu {
        guchar type;
        guchar data[12];
} gpu_data;

struct packet_to_apu {
        guchar type;
        guchar data[4];
} apu_data;

struct input_buffer {
       guint buffer [KEYBOARD_BUFFER_SIZE];
       gint wp;
       gint cnt;
} keyboard_data;

G_LOCK_DEFINE (keyboard_data);
G_LOCK_DEFINE (gpu_data);
G_LOCK_DEFINE (apu_data);

enum {
        GPU_PRINT = 0,
        GPU_PEN,
        GPU_PAPER,
        GPU_CLEAR,
        GPU_RESET
};

enum {
        APU_PRINT = 0,
        APU_PEN,
        APU_PAPER,
        APU_CLEAR,
        APU_RESET
};
#endif
