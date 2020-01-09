/**
 * @file thread_cpu.h
 * @brief Prototypes, macros, enumerators and global variables...
 */

#ifndef THREAD_GPU_H
#define THREAD_GPU_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <gtk/gtk.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <termios.h>

#include "dhpcemu.h"
#include "gpu_buffer.h"

// ------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------

#define GPU_DATA_LENGTH    8
#define CHARS_PER_LINE     32
#define LINES_PER_FRAME    24
#define TEXT_COL_DEFAULT   76
#define TEXT_COL_ERROR     67
#define BACK_COL_DEFAULT   0
#define PXL_LINES_PER_CHAR 10

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

struct rgb_triad {
        guchar red;
        guchar green;
        guchar blue;
};

struct packet_to_gpu {
        guchar type;
        guchar data[GPU_DATA_LENGTH];
        gboolean received;
        gboolean new_set;
} gpu_data;

GMutex *gpu_data_mutex;
GCond *gpu_data_cond;

guchar cursor_x, cursor_y;

struct rgb_triad colour_pen;
struct rgb_triad colour_paper;

guchar cpu_command;
guchar cpu_data[GPU_DATA_LENGTH];

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gpointer GPU_thread_init (gpointer data);
void init_video (void);

struct rgb_triad color_converter (guchar colour);

#endif
