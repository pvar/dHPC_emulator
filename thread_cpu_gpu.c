/*
 * Fundamental components of nstBASIC.
 *
 * Copyright 2016, Panagiotis Varelas <varelaspanos@gmail.com>
 *
 * nstBASIC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * nstBASIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/gpl-3.0.html>.
 */

/**
 * @file main.c
 * @brief Functions that would fit in no other file :)
 */

#include "thread_cpu_gpu.h"

/** ***************************************************************************
 * @brief
 *****************************************************************************/
gpointer CPU_GPU_thread_init (gpointer data)
{
        init_io();
        init_video();
        init_basic();

        do_beep();

        /* print welcome message */
        printmsg (msg_welcome, active_stream);
        /* print (available) SRAM size */
        printnum (variables_ptr - prog_end_ptr, active_stream);
        printmsg (msg_ram_bytes, active_stream);
        newline (active_stream);

        interpreter();
        return NULL;
}

/** ***************************************************************************
 * @brief Push a byte in the STACK.
 *****************************************************************************/
void push_byte (guchar byte_to_push)
{
        stack_ptr--;
        *stack_ptr = byte_to_push;
}

/** ***************************************************************************
 * @brief Pop a byte from the STACK.
 *****************************************************************************/
guchar pop_byte (void)
{
        guchar byte_to_pop;
        byte_to_pop = *stack_ptr;
        stack_ptr++;
        return byte_to_pop;
}

/** ***************************************************************************
 *
 *****************************************************************************/
guchar *find_line (void)
{
        guchar *line = program_space;
        while (1) {
                if (line == prog_end_ptr)
                        return line;
                if ( ((uint16_t *)line)[0] >= linenum)
                        return line;
                // add line's lenght (this value is stored exactly after line number0
                line += line[ sizeof (LINE_NUMBER) ];
        }
}

/** ***************************************************************************
 * @brief Normalize user input.
 * Ignore spaces and transform code (not strings) to upper case.
 *****************************************************************************/
void ignorespace (void)
{
        while (*text_ptr == SPACE || *text_ptr == TAB)
                text_ptr++;
}

void uppercase (void)
{
        guchar *c = prog_end_ptr + sizeof (uint16_t);
        guchar quote = 0;
        while (*c != LF) {
                // are we in a quoted string?
                if (*c == quote) quote = 0;
                else if (*c == DQUOTE || *c == SQUOTE) quote = *c;
                else if (quote == 0 && *c >= 'a' && *c <= 'z') *c = *c + 'A' - 'a';
                c++;
        }
}

/** ***************************************************************************
 * @brief Check if BREAK button was pressed.
 *****************************************************************************/
guchar break_test (void)
{
        if (break_flow) {
                break_flow = 0;
                return 1;
        }
        return 0;
}

/** ***************************************************************************
 * @brief Transform string representation to numeric.
 *****************************************************************************/
int16_t str_to_num (guchar *strptr)
{
        guchar negative = 0;
        int16_t value = 0;
        // check for minus sign
        if (*strptr == '-') {
                strptr++;
                negative = 1;
        }
        // calculate value of given number
        while (*strptr >= '0' && *strptr <= '9') {
                value = 10 * value + (*strptr - '0');
                strptr++;
        }
        if (negative)
                return -value;
        else
                return value;
}
