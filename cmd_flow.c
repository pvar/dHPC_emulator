/*
 * Implementation of flow-control commands of nstBASIC. of nstBASIC.
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

#include "cmd_flow.h"

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
