/**
 * @file cpu_io.h
 * @brief Prototypes, macros, enumerators and global variables...
 */

#ifndef CPU_IO_H
#define CPU_IO_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "thread_cpu_gpu.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------


void init_io (void);
void do_beep (void);

void uart_ansi_rst_clr (void);
void uart_ansi_move_cursor (uint8_t row, uint8_t col);

void text_color (uint8_t color);
void paper_color (uint8_t color);
void locate_cursor (uint8_t line, uint8_t column);
void put_pixel (uint8_t x, uint8_t y, uint8_t color);

void send_to_apu (uint8_t cbyte);
void get_line (void);


// input / output functions
guint emu_putchar (guint out_char, uint8_t stream);
guint emu_getchar (uint8_t stream);

guint get_std (void);
guint get_gpu (void);
guint get_apu (void);
guint get_ser (void);

// ------------------------------------------------------------------------------
// MACROS
// ------------------------------------------------------------------------------

// ASCII special characters
#define LF              0x0A    // ENTER
#define CR              0x0D    // RETURN
#define TAB             0x09    // TAB
#define SPACE           0x20    // SPACE
#define BS              0x08    // BACKSPACE
#define ESC             0x1B    // ESC
#define DQUOTE          0x22    // DOUBLE QUOTE
#define SQUOTE          0x27    // SINGLE QUOTE

// other special characters (arbitrarily defined)
#define CTRL_L          0xf0    // CTRL+L
#define CTRL_G          0xf1    // CTRL+G

// sudo streams for redirecting IO
#define STREAM_STD 1
#define STREAM_APU 2
#define STREAM_SER 3

#define KEYBOARD_BUFFER_SIZE 32

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

struct input_buffer {
       guint buffer [KEYBOARD_BUFFER_SIZE];
       gint wp;
       gint cnt;
} keyboard_data;

G_LOCK_DEFINE (keyboard_data);

gint active_stream;
uint8_t break_flow;

#endif
