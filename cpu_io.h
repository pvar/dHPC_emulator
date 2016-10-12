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

#include "thread_cpu.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------


void init_io (void);
void do_beep (void);

void uart_ansi_rst_clr (void);
void uart_ansi_move_cursor (guchar row, guchar col);

void send_to_apu (guchar cbyte);
void get_line (void);


// input / output functions
guint emu_putchar (guchar out_char, guchar stream);
guint emu_getchar (guchar stream);

guchar get_kbd (void);
guchar get_ser (void);

void put_gpu (guchar chr);
void put_apu (guchar chr);
void put_ser (guchar chr);

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
#define HOME            0xf2    // HOME
#define END             0xf3    // END
#define AR_LFT          0xf4    // ARROW LEFT
#define AR_RGT          0xf5    // ARROW RIGHT

// sudo streams for redirecting IO
#define STREAM_STD 1
#define STREAM_APU 2
#define STREAM_SER 3

// ------------------------------------------------------------------------------
// GLOBALS
// ------------------------------------------------------------------------------

gint active_stream;
guchar break_flow;

#endif
