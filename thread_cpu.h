/**
 * @file thread_cpu.h
 * @brief Prototypes, macros, enumerators and global variables...
 */

#ifndef THREAD_CPU_H
#define THREAD_CPU_H

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
#include "cpu_io.h"
#include "cpu_interpreter.h"
#include "cpu_print.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gpointer CPU_thread_init (gpointer data);

// internal data handling
void uppercase (void);
void ignorespace (void);
void push_byte (guchar b);
guchar pop_byte (void);
int16_t str_to_num (guchar *strptr);
guchar *find_line (void);

guchar break_test (void);

uint16_t valid_filename_char (guchar c);
guchar * valid_filename (void);

// ------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------

#define KEYBOARD_BUFFER_SIZE    32
#define cfg_auto_run            1  // 1st bit
#define cfg_run_after_load      2  // 2nd bit
#define cfg_from_sdcard         4  // 3rd bit
#define cfg_from_eeprom         8  // 4th bit

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

struct input_buffer {
       guint buffer [KEYBOARD_BUFFER_SIZE];
       gint wp;
       gint cnt;
} keyboard_data;

GMutex *kbd_data_mutex;
GCond *kbd_data_cond;

guchar sys_config;

#endif
