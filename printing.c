/*
 * String and message printing functions for nstBASIC.
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
 * @file printing.c
 * @brief Message and other data printing.
 *
 * The functions in this file can print messages, numbers, program listings or arbitrary (user
 * defined) strings on the screen, on some terminal through the serial port or on the EEPROM.
 * This flexibility is achieved with the use of relevant streams, defined in io.c. Each of
 * these streams is accompanied by functions that handle low-level IO cimmunication with
 * the corresponding device.
*/

#include "printing.h"

/// @cond CONST_MESSAGES

// general messages
const uint8_t msg_welcome[]   = "Welcome to nstBASIC v0.2\0";
const uint8_t msg_ram_bytes[] = " bytes RAM\0";
const uint8_t msg_rom_bytes[] = " bytes ROM\0";
const uint8_t msg_available[] = " bytes available\0";
const uint8_t msg_break[]     = "Break!\0";
const uint8_t msg_ok[]        = "OK\0";

// error messages
const uint8_t err_msgxl[] = "Left \0";
const uint8_t err_msgxr[] = "Right \0";
const uint8_t err_msg01[] = "Not yet implemented\0";
const uint8_t err_msg02[] = "Syntax error\0";
const uint8_t err_msg03[] = "Stack overflow\0";
const uint8_t err_msg04[] = "Unexpected character\0";
const uint8_t err_msg05[] = "parenthesis missing\0";
const uint8_t err_msg07[] = "Variable expected\0";
const uint8_t err_msg08[] = "Jump point not found\0";
const uint8_t err_msg09[] = "Invalid line number\0";
const uint8_t err_msg0A[] = "Operator expected\0";
const uint8_t err_msg0B[] = "Division by zero\0";
const uint8_t err_msg0C[] = "Invalid pin [0..7]\0";
const uint8_t err_msg0D[] = "Pin I/O error\0";
const uint8_t err_msg0E[] = "Unknown function\0";
const uint8_t err_msg0F[] = "Unknown command\0";
const uint8_t err_msg10[] = "Invalid coordinates\0";
const uint8_t err_msg11[] = "Invalid variable name\0";
const uint8_t err_msg12[] = "Expected byte [0..255]\0";
const uint8_t err_msg13[] = "Out of range\0";
const uint8_t err_msg14[] = "Expected color [0..127]\0";
const uint8_t err_msg15[] = "Expression expected!\0";

// keyboard connectivity messages
const uint8_t kb_fail_msg[26] = "Keyboard self-test failed\0";
const uint8_t kb_success_msg[32] = "Keyboard connected successfully\0";

/// @endcond

/** ***************************************************************************
 * @brief Print specified string
 *****************************************************************************/
void printstr (char *string, gint stream)
{
    uint8_t i = 0;
    while (string[i] != 0) {
        emu_putchar (string[i], stream);
        i++;
    }
}

/** ***************************************************************************
 * @brief Print specified integer number
 *****************************************************************************/
void printnum (int32_t number, gint stream)
{
        int digits = 0;
        if (number < 0) {
                number = -number;
                emu_putchar ('-', stream);
        }
        do {
                push_byte (number % 10 + '0');
                number = number / 10;
                digits++;
        } while (number > 0);

        while (digits > 0) {
                emu_putchar (pop_byte(), stream);
                digits--;
        }
}

/** ***************************************************************************
 * @brief Print selected message from FLASH (without new-line)
 *****************************************************************************/
void printmsg_noNL (const uint8_t *message, gint stream)
{
        while (*(message) != 0)
                emu_putchar (*(message++), stream);
}

/** ***************************************************************************
 * @brief Print selected message from FLASH (with new-line)
 *****************************************************************************/
void printmsg (const uint8_t *message, gint stream)
{
        printmsg_noNL (message, stream);
        newline (stream);
}

/** ***************************************************************************
 * @brief Print a program line
 *****************************************************************************/
void printline (uint8_t *line, gint stream)
{
        LINE_NUMBER line_num = *((LINE_NUMBER *)(line));

        line += sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH);

        // print line number and a space
        printnum (line_num, stream);
        emu_putchar (' ', stream);

        // print line content
        while (*line != LF) {
                emu_putchar (*line, stream);
                line++;
        }
        line++;
        newline (stream);
}

/** ***************************************************************************
 * @brief Print a NEW LINE character
 *****************************************************************************/
void newline (gint stream)
{
        emu_putchar (LF, stream);
//        emu_putchar (CR, active_stream);
}

/** ***************************************************************************
 * @brief Print a user defined string (enclosed in quotes)
 * @note The opening delimiter of the string is pointed to by @c text_ptr.
 *****************************************************************************/
uint8_t print_string (void)
{
        uint16_t i = 0;
        uint8_t delim = *text_ptr;
        // check for opening delimiter
        if (delim != '"' && delim != '\'')
                return 0;
        text_ptr++;
        // check for closing delimiter
        while (text_ptr[i] != delim) {
                if (text_ptr[i] == LF)
                        return 0;
                i++;
        }
        // print characters
        while (*text_ptr != delim) {
                emu_putchar (*text_ptr, active_stream);
                text_ptr++;
        }
        text_ptr++; // skip closing
        return 1;
}

/** ***************************************************************************
 * @brief Print a hash and the specified character for debug purposes
 *****************************************************************************/
void debug_print (uint8_t chr)
{
        emu_putchar ('#', active_stream);
        emu_putchar (chr, active_stream);
        emu_putchar (' ', active_stream);
}
