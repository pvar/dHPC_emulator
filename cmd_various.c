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

#include "cmd_various.h"


uint8_t gotoline (void)
{
    linenum = parse_expr_s1();
    if (error_code || *text_ptr != LF) {
        error_code = 0x4;
        return POST_CMD_WARM_RESET;
    }
    line_ptr = find_line();
    return POST_CMD_EXEC_LINE;
}

uint8_t check (void)
{
    uint16_t value;
    value = parse_expr_s1();
    if (error_code || *text_ptr == LF) {
        error_code = 0x4;
        return POST_CMD_WARM_RESET;
    }
    if (value != 0)
        return POST_CMD_LOOP;
    else
        return POST_CMD_NEXT_LINE;
}

uint8_t loopfor (void)
{
        uint8_t index;
                uint8_t var;
                uint16_t initial, step, terminal;
                ignorespace();
                if (*text_ptr < 'A' || *text_ptr > 'Z') {
                        error_code = 0x7;
            return POST_CMD_WARM_RESET;
                }
                var = *text_ptr;
                text_ptr++;
                ignorespace();
                if (*text_ptr != '=') {
                        error_code = 0xA;
            return POST_CMD_WARM_RESET;
                }
                text_ptr++;
                ignorespace();
                error_code = 0;
                initial = parse_expr_s1();
                if (error_code) {
            return POST_CMD_WARM_RESET;
        }
                index = scantable (to_tab);
                if (index != 0) {
                        error_code = 0x2;
            return POST_CMD_WARM_RESET;
                }
                terminal = parse_expr_s1();
                if (error_code) {
            return POST_CMD_WARM_RESET;
        }
        index = scantable (step_tab);
                if (index == 0) {
                        step = parse_expr_s1();
                        if (error_code) {
                return POST_CMD_WARM_RESET;
            }
                } else step = 1;
                ignorespace();
                if (*text_ptr != LF && *text_ptr != ':') {
                        error_code = 0x2;
            return POST_CMD_WARM_RESET;
                }
                if (!error_code && *text_ptr == LF) {
                        struct stack_for_frame *f;
                        if (stack_ptr + sizeof (struct stack_for_frame) < stack_limit) {
                                error_code = 0x3;
                return POST_CMD_WARM_RESET;
                        }
                        stack_ptr -= sizeof (struct stack_for_frame);
                        f = (struct stack_for_frame *)stack_ptr;
                        ((uint16_t *)variables_ptr)[var - 'A'] = initial;
                        f->frame_type = STACK_FOR_FLAG;
                        f->for_var = var;
                        f->terminal = terminal;
                        f->step = step;
                        f->text_ptr = text_ptr;
                        f->line_ptr = line_ptr;
                        return POST_CMD_NEXT_STATEMENT;
                }
        error_code = 0x4;
    return POST_CMD_WARM_RESET;
}

uint8_t gosub (void)
{
        error_code = 0;
        linenum = parse_expr_s1();
        if (!error_code && *text_ptr == LF) {
                struct stack_gosub_frame *f;
                if (stack_ptr + sizeof (struct stack_gosub_frame) < stack_limit) {
                        error_code = 0x3;
            return POST_CMD_WARM_RESET;
                }
                stack_ptr -= sizeof (struct stack_gosub_frame);
                f = (struct stack_gosub_frame *)stack_ptr;
                f->frame_type = STACK_GOSUB_FLAG;
                f->text_ptr = text_ptr;
                f->line_ptr = line_ptr;
                line_ptr = find_line();
                return POST_CMD_EXEC_LINE;
        }
        error_code = 0x4;
    return POST_CMD_WARM_RESET;
}

uint8_t next (void)
{
        // find the variable name
        ignorespace();
        if (*text_ptr < 'A' || *text_ptr > 'Z') {
                error_code = 0x7;
        return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        ignorespace();
        if (*text_ptr != ':' && *text_ptr != LF) {
                error_code = 0x2;
        return POST_CMD_WARM_RESET;
        }
    return POST_CMD_NOTHING;
}

uint8_t gosub_return (uint8_t cmd)
{
    uint8_t *tmp_stack_ptr;

        // walk up the stack frames and find the frame we want -- if present
        tmp_stack_ptr = stack_ptr;
        while (tmp_stack_ptr < program_space + MEMORY_SIZE - 1) {
                switch (tmp_stack_ptr[0]) {
                case STACK_GOSUB_FLAG:
                        if (cmd == CMD_RETURN) {
                                struct stack_gosub_frame *f = (struct stack_gosub_frame *)tmp_stack_ptr;
                                line_ptr    = f->line_ptr;
                                text_ptr                  = f->text_ptr;
                                stack_ptr += sizeof (struct stack_gosub_frame);
                                return POST_CMD_NEXT_STATEMENT;
                        }
                        // This is not the loop you are looking for... go up in the stack
                        tmp_stack_ptr += sizeof (struct stack_gosub_frame);
                        break;
                case STACK_FOR_FLAG:
                        // Flag, Var, Final, Step
                        if (cmd == CMD_NEXT) {
                                struct stack_for_frame *f = (struct stack_for_frame *)tmp_stack_ptr;
                                // Is the variable we are looking for?
                                if (text_ptr[-1] == f->for_var) {
                                        uint16_t *varaddr = ((uint16_t *)variables_ptr) + text_ptr[-1] - 'A';
                                        *varaddr = *varaddr + f->step;
                                        // Use a different test depending on the sign of the step increment
                                        if ((f->step > 0 && *varaddr <= f->terminal) || (f->step < 0 && *varaddr >= f->terminal)) {
                                                // We have to loop so don't pop the stack
                                                text_ptr = f->text_ptr;
                                                line_ptr = f->line_ptr;
                                                return POST_CMD_NEXT_STATEMENT;
                                        }
                                        // We've run to the end of the loop. drop out of the loop, popping the stack
                                        stack_ptr = tmp_stack_ptr + sizeof (struct stack_for_frame);
                                        return POST_CMD_NEXT_STATEMENT;
                                }
                        }
                        // This is not the loop you are looking for... go up in the stack
                        tmp_stack_ptr += sizeof (struct stack_for_frame);
                        break;
                default:
                        //printf( "Stack is full!\n" );
            return POST_CMD_WARM_RESET;
                }
        }
        // cannot find the return point
        error_code = 0x8;
    return POST_CMD_WARM_RESET;
}




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


uint8_t pindir (void)
{
        return 0;
}

uint8_t pindwrite (void)
{
        return 0;
}
