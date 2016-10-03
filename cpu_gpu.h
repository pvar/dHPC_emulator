/**
 * @file main.h
 * @brief Prototypes, macros, enumerators and global variables...
 */

#ifndef CPU_GPU_H
#define CPU_GPU_H

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

#include "io.h"
#include "interpreter.h"
#include "printing.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

gpointer CPU_GPU_thread_init (gpointer data);

// internal data handling
void uppercase (void);
void ignorespace (void);
void push_byte (uint8_t b);
uint8_t pop_byte (void);
int16_t str_to_num (uint8_t *strptr);
uint8_t *find_line (void);

uint8_t break_test (void);

uint16_t valid_filename_char (uint8_t c);
uint8_t * valid_filename (void);

// ------------------------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------------------------

#define cfg_auto_run       1 // 1st bit
#define cfg_run_after_load 2 // 2nd bit
#define cfg_from_sdcard    4 // 3rd bit
#define cfg_from_eeprom    8 // 4th bit

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

uint8_t sys_config;

guint to_apu;
guint from_apu;

G_LOCK_DEFINE (to_apu);
G_LOCK_DEFINE (from_apu);

#endif
