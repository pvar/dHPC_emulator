/**
 * @file printing.h
 * @brief Prototypes, macros, enumerators and global variables...
 * The three macros defined in this file determine the text colour for 'normal' and 'error'
 * messages, as well as the length of a text line on the screen. When porting nstBASIC to
 * other platforms, they should probably have to be be updated.
*/

#ifndef PRINTING_H
#define PRINTING_H

// ------------------------------------------------------------------------------
// INCLUDES
// ------------------------------------------------------------------------------

#include "cpu_gpu.h"

// ------------------------------------------------------------------------------
// PROTOTYPES
// ------------------------------------------------------------------------------

void printnum (int32_t num, gint stream);
void printmsg_noNL (const uint8_t *msg, gint stream);
void printmsg (const uint8_t *msg, gint stream);
void printline (uint8_t *line, gint stream);
void newline (gint stream);
uint8_t print_string (void);
void debug_print (uint8_t chr);

#endif
