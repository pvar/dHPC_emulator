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

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gpointer GPU_thread_init (gpointer data);
void init_video (void);

// ------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------

#define CHAR_PER_LINE 32
#define TEXT_COL_DEFAULT 7
#define BACK_COL_DEFAULT 0
#define TEXT_COL_ERROR 3

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

guchar cursor_x, cursor_y;
guchar colour_pen, colour_paper;
/*
struct rgb_triad {
        guchar red;
        guchar green;
        guchar blue;
};
*/
#endif
