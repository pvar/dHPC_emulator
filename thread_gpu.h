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

#define CHAR_PER_LINE 32
#define TEXT_COL_DEFAULT 12
#define BACK_COL_DEFAULT 0
#define TEXT_COL_ERROR 3

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

struct rgb_triad {
        guchar red;
        guchar green;
        guchar blue;
};

guchar cursor_x, cursor_y;

struct rgb_triad colour_pen;
struct rgb_triad colour_paper;

guchar cpu_command;
guchar cpu_data[GPU_DATA_PACKET];

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gpointer GPU_thread_init (gpointer data);
void init_video (void);

struct rgb_triad color_converter (guchar colour);

#endif
