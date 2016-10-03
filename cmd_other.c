/*
 * Implementation of various (uncategorized) commands of nstBASIC.
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

#include "cmd_other.h"

int8_t input (void)
{
    uint8_t chr = 0;
    uint8_t cnt = 0;
    int16_t *var;
    uint8_t *input_buffer_ptr;

    // variable to store user value
    ignorespace();
    if (*text_ptr < 'A' || *text_ptr > 'Z') {
        error_code = 0x7;
        return POST_CMD_WARM_RESET;
    }
    var = (int16_t *)variables_ptr + *text_ptr - 'A';
    // check for proper statement termination
    text_ptr++;
    ignorespace();
    if (*text_ptr != LF && *text_ptr != ':') {
        error_code = 0x2;
        return POST_CMD_WARM_RESET;
    }
    // get user value (accept only digits)
    input_buffer_ptr = input_buffer;
    *input_buffer_ptr = 0;
    cnt = 0;

    while (chr != LF && chr != CR) {
        chr = fgetc (stdin);
        if (break_flow == 1)
            return POST_CMD_NEXT_STATEMENT;
        switch (chr) {
            case '-':
                // only accept minus sign as first character
                if (cnt == 0) {
                    g_print("%c", chr);
                    *input_buffer_ptr = chr;
                    cnt++;
                    input_buffer_ptr++;
                    *input_buffer_ptr = 0;
                }
                break;
            case LF:
            case CR:
                *input_buffer_ptr = LF;
                break;
            case BS:
                if (input_buffer_ptr <= input_buffer)
                    do_beep();
                else {
                    g_print("%c", BS);
                    input_buffer_ptr--;
                    cnt--;
                }
                break;
            default:
                if (chr >= '0' && chr <= '9') {
                    if (cnt < INPUT_BUFFER_SIZE) {
                        g_print("%c", chr);
                        *input_buffer_ptr = chr;
                        cnt++;
                        input_buffer_ptr++;
                        // mark last character with ASCII code 0
                        *input_buffer_ptr = 0;
                    } else
                        do_beep();
                }
        }
    }
    newline (active_stream);
    // parse and store user value
    *var = str_to_num (input_buffer);
        return POST_CMD_NEXT_STATEMENT;
}

int8_t assignment (void)
{
        int16_t value, *var;
        // check if invalid character (non-letter)
        if (*text_ptr < 'A' || *text_ptr > 'Z') {
                error_code = 0xf;
                return POST_CMD_WARM_RESET;
        }
        var = (int16_t *)variables_ptr + *text_ptr - 'A';
        text_ptr++;

        // check for invalid variable name (a second letter!)
        if (*text_ptr >= 'A' && *text_ptr <= 'Z') {
                text_ptr++;
                error_code = 0xf;
                return POST_CMD_WARM_RESET;
        }

        // check for missing assignment operator
        ignorespace();
        if (*text_ptr != '=') {
                error_code = 0xf;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        ignorespace();
        value = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        // check if at the end of the statement
        if (*text_ptr != LF && *text_ptr != ':') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        *var = value;
        return POST_CMD_NEXT_STATEMENT;
}

int8_t poke (void)
{
    int16_t value, address;
    // get the address
    address = parse_expr_s1();
    if (error_code) {
        return POST_CMD_WARM_RESET;
    }
    if (address > MEMORY_SIZE) {
        error_code = 0x13;
        return POST_CMD_WARM_RESET;
    }
    // check for comma
    ignorespace();
    if (*text_ptr != ',') {
        error_code = 0x2;
        return POST_CMD_WARM_RESET;
    }
    text_ptr++;
    // get the value to assign
    ignorespace();
    value = parse_expr_s1();
    if (error_code) {
        return POST_CMD_WARM_RESET;
    }
    if (value < 0 || value > 255) {
        error_code = 0x12;
        return POST_CMD_WARM_RESET;
    }
    // check for proper statement termination
    if (*text_ptr != LF && *text_ptr != ':') {
        error_code = 0x2;
        return POST_CMD_WARM_RESET;
    }
    // assign value to specified location in memory
    program_space[address] = value;
        return POST_CMD_NEXT_STATEMENT;
}

int8_t list (void)
{
        linenum = get_linenumber();

        if (text_ptr[0] != LF) {
                error_code = 0x4;
                return POST_CMD_WARM_RESET;
        }

        // find line
        uint8_t *line = find_line();
        LINE_LENGTH length = 0;
        while (line != prog_end_ptr) {
                printline (line, active_stream);
                length = (LINE_LENGTH) (*(line + sizeof (LINE_NUMBER)));
                line += length;
        }
        return POST_CMD_WARM_RESET;
}

int8_t mem (void)
{
        // SRAM size
        printnum (variables_ptr - prog_end_ptr, active_stream);
        printmsg (msg_ram_bytes, active_stream);
        return POST_CMD_NEXT_STATEMENT;
}

int8_t randomize (void)
{
        srand (2);
        return POST_CMD_NEXT_STATEMENT;
}

int8_t rndseed (void)
{
        uint16_t param;
        error_code = 0;
        // get seed for PRNG
        param = (uint16_t)parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        srand (param);
        return POST_CMD_NEXT_STATEMENT;
}

int8_t prog_run (void)
{
        // disable cursor
        //// g_print("%c", vid_cursor_off);
        // disable auto scroll
        //// g_print("%c", vid_scroll_off);
        line_ptr = program_space;
        return POST_CMD_EXEC_LINE;
}

int8_t prog_end (void)
{
        // should be at end of line
        if (text_ptr[0] != LF) {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        // set current line at the end of program
        line_ptr = prog_end_ptr;
        return POST_CMD_EXEC_LINE;
}

int8_t prog_new (void)
{
        if (text_ptr[0] != LF) {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        prog_end_ptr = program_space;
        return POST_CMD_PROMPT;
}
