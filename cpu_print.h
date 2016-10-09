/**
 * @file cpu_print.h
 * @brief Prototypes, macros, enumerators and global variables...
 * The three macros defined in this file determine the text colour for 'normal' and 'error'
 * messages, as well as the length of a text line on the screen. When porting nstBASIC to
 * other platforms, they should probably have to be be updated.
*/

#ifndef CPU_PRINT_H
#define CPU_PRINT_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include "thread_cpu.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

void printnum (int32_t num, gint stream);
void printmsg_noNL (const guchar *msg, gint stream);
void printmsg (const guchar *msg, gint stream);
void printline (guchar *line, gint stream);
void newline (gint stream);
guchar print_string (void);
void debug_print (guchar chr);

#endif
