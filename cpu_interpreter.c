/*
 * Interpreter for nstBASIC.
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
 * @file cpu_interpreter.c
 * @brief Get user input, search for commands and execute them.
 *
 * The functions in this file form the core of the interprter, which processes user input
 * and/or programs. Every line entered by the user, that starts with a valid line-number,
 * is considered part of the currently active program and is stored at the appropriate
 * position in program misc_print_memory. If the newly entered line does not start with a line-number,
 * it is executed immediately.
 * @note Although the functions in this file perform some kind of parsing (syntactic analysis),
 * they are kept separately from the main parser functions. The interpreter functions determine
 * the state of the whole system, they run programs and call other functions, that execute single
 * commands. They do not evaluate expressions or check the syntax of given commands. In other
 * words, the interpreter could be seen as a manager/supervisor and the parser as one of those
 * who do the dirty work. :)
*/

#include "cpu_interpreter.h"

static guchar execution (void);
static void warm_reset (void);
static void insert_line (void);
static void remove_line (void);
static void move_line (void);
static void prep_line (void);
static void error_message (void);

static guchar *start;

/** ***************************************************************************
 * @brief Get current line number.
 *
 * This function scans current line and looks for a valid line number.
 * @note Scanning of current line begins at the character pointed by @c text_ptr.
 * @return The current line number.
 *****************************************************************************/
uint16_t get_linenumber (void)
{
        uint16_t num = 0;
        ignorespace();
        while (*text_ptr >= '0' && *text_ptr <= '9') {
                // check for overflow...
                if (num >= 0xFFFF / 10) {
                        num = 0xFFFF;
                        break;
                }
                num = num * 10 + *text_ptr - '0';
                text_ptr++;
        }
        return  num;
}

/** ***************************************************************************
 * @brief Initialization of language interpreter.
 *
 * This function initializes pointers used by the interpreter
 * and prints a welcome message along with the misc_print_memory size.
 *****************************************************************************/
void init_basic (void)
{
        prog_end_ptr = program_space;
        stack_ptr = program_space + MEMORY_SIZE;
        stack_limit = program_space + MEMORY_SIZE - STACK_SIZE;
        variables_ptr = stack_limit - 27 * VAR_SIZE;
}

/** ***************************************************************************
 * @brief The interpreter main loop.
 *
 * This function examines user input and determines if the last line entered
 * was a command or a program line. In the first case, the command is executed
 * immediately. In the second case, the line read is merged with the rest of
 * the program and into appropriate position.
 *****************************************************************************/
void interpreter (void)
{
    guchar exec_status;

    /* start interpreter with a warm-reset */
    exec_status = POST_CMD_WARM_RESET;

    while(1) {
        /* if have to --> perform warm-reset */
        if (exec_status == POST_CMD_WARM_RESET) {
            exec_status = POST_CMD_NOTHING;
            warm_reset();
        }

        while(1) {
            error_code = 0;
            /* check if autorun is enabled */
            if ((sys_config & cfg_auto_run) || (sys_config & cfg_run_after_load)) {
                sys_config &= ~cfg_auto_run;
                sys_config &= ~cfg_run_after_load;
                line_ptr = program_space;
                text_ptr = line_ptr + sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH);
                break;
            }

            get_line();
            uppercase();
            move_line();

            /* attempt to read line number */
            linenum = get_linenumber();
            ignorespace();

            /* chech if line number was found */
            if (linenum != 0) {

                /* invalid number --> ignore line & warm reset */
                if (linenum == 0xFFFF) {
                    error_code = 0x9;
                    break;
                }

                /* valid number --> merge with program */
                else {
                    /* embed line number and line length */
                    prep_line();
                    /* remove line with same line number */
                    start = find_line();
                    remove_line();
                    /* new line is empty --> get another one */
                    if (text_ptr[sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH)] == LF)
                        continue;
                    /* append new line to program */
                    insert_line();
                }
            }

            /* no line number --> execute it immediately */
            else {
                break_flow = 0;
                text_ptr = prog_end_ptr + sizeof (LINE_NUMBER);
                if (*text_ptr == LF)
                    continue;
                else
                    break;
            }
        }

        /* if no error --> start execution */
        if (error_code == 0)
            exec_status = execution();

        // do not combine these two checks!
        // should check for error again, after execution...

        /* if error --> print message */
        if (error_code)
            error_message();
    }
}

/** ***************************************************************************
 * @brief Execute specified line.
 *
 * This function executes the current line. A helper function (scantable())
 * looks for the presence of any command and the huge switch block executes
 * the said command. When a program is running, this function loops over
 * and keeps executing one line after the other.
 *
 * @note The current line begins at the character pointed by @c text_ptr.
 * When running a program, @c text_ptr is advanced automatically.
 * @return The post-execution status. See EXECUTION_STATUS enumerator.
 *****************************************************************************/
static guchar execution (void)
{
    guchar index;
    guchar cmd_status;

    while(1) {
        if (break_test()) {
            printmsg (msg_break, active_stream);
            return POST_CMD_WARM_RESET;
        }

        cmd_status = POST_CMD_NOTHING;
        error_code = 0;
        index = scantable (commands);

        switch (index) {
            case CMD_DELAY:
                cmd_status = POST_CMD_NEXT_LINE;
                break;
            case CMD_NEW:
                cmd_status = misc_clear_program();
                break;
            case CMD_BEEP:
                do_beep();
                cmd_status = POST_CMD_NEXT_LINE;
                break;
            case CMD_RUN:
                cmd_status = flow_run();
                break;
            case CMD_IF:
                cmd_status = misc_conditional();
                break;
            case CMD_GOTO:
                cmd_status = flow_goto();
                break;
            case CMD_MPLAY:
                cmd_status = snd_play();
                break;
            case CMD_MSTOP:
                cmd_status = snd_stop();
                break;
            case CMD_TEMPO:
                cmd_status = snd_tempo();
                break;
            case CMD_MUSIC:
                cmd_status = snd_music();
                break;
            case CMD_END:
            case CMD_STOP:
                cmd_status = flow_end();
                break;
            case CMD_LIST:
                cmd_status = misc_list();
                break;
            case CMD_MEM:
                cmd_status = misc_print_mem();
                break;
            case CMD_PEN:
                cmd_status = vid_set_pen_colour();
                break;
            case CMD_PAPER:
                cmd_status = vid_set_paper_colour();
                break;
            case CMD_NEXT:
                cmd_status = flow_next();
                if (error_code)
                    break;
                cmd_status = flow_return(CMD_NEXT);
                break;
            case CMD_LET:
                cmd_status = misc_assignment();
                break;
            case CMD_GOSUB:
                cmd_status = flow_gosub();
                break;
            case CMD_RETURN:
                cmd_status = flow_return(CMD_RETURN);
                break;
            case CMD_RANDOMIZE:
                cmd_status = prng_seed_refresh();
                break;
            case CMD_RNDSEED:
                cmd_status = prng_seed_define();
                break;
            case CMD_FOR:
                cmd_status = flow_forloop();
                break;
            case CMD_INPUT:
                cmd_status = misc_get_value();
                break;
            case CMD_POKE:
                cmd_status = misc_poke_mem();
                break;
            case CMD_PSET:
                cmd_status = vid_put_pixel();
                break;
            case CMD_RST:
                cmd_status = vid_reset();
                break;
            case CMD_PRINT:
            case CMD_QMARK:
                cmd_status = misc_print();
                break;
            case CMD_LOCATE:
                cmd_status = vid_locate_cursor();
                break;
            case CMD_CLS:
                cmd_status = vid_clear();
                break;
            case CMD_REM:
            case CMD_HASH:
            case CMD_QUOTE:
                cmd_status = POST_CMD_NEXT_LINE;
                break;
            case CMD_PINDIR:
                cmd_status = gpio_set_direction();
                break;
            case CMD_PINDWRITE:
                cmd_status = gpio_write_digital();
                break;
            case CMD_ASSIGNMENT:
                cmd_status = misc_assignment();
                break;
            default:
                error_code = 0xf;
                cmd_status = POST_CMD_WARM_RESET;
                break;
        }

        // check if should warm reset
        if (cmd_status == POST_CMD_WARM_RESET)
            return POST_CMD_WARM_RESET;

        // check if should return to prompt
        if (cmd_status == POST_CMD_PROMPT)
            return POST_CMD_PROMPT;

        // check if should restart interpretation
        if (cmd_status == POST_CMD_LOOP)
            continue;

        if (cmd_status == POST_CMD_NEXT_STATEMENT) {
            ignorespace();
            if (*text_ptr == ':') {
                text_ptr++;
                ignorespace();
                continue;
            } else
                cmd_status = POST_CMD_NEXT_LINE;
        }

        // check if should proceed to next line
        if (cmd_status == POST_CMD_NEXT_LINE) {
            // check if in direct mode (no line number)
            if (line_ptr == NULL)
                return POST_CMD_WARM_RESET;
            // proceed to next line
            line_ptr += line_ptr[sizeof (uint16_t)];
        }

        // warm reset if reached end of program
        if (line_ptr == prog_end_ptr)
            return POST_CMD_WARM_RESET;

        // if reached here, start execution of next line
        text_ptr = line_ptr + sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH);
    }
}

/** ***************************************************************************
 * @brief Perform a warm-reset.
 *
 * This function resets program-misc_print_memory pointer
 * and enables cursor and scrolling.
 *****************************************************************************/
static void warm_reset (void)
{
    // turn-on cursor
//    g_print("%c", vid_cursor_on);
    // turn-on scroll
//    g_print("%c", vid_scroll_on);
    // reset program-misc_print_memory pointer
    line_ptr = 0;
    stack_ptr = program_space + MEMORY_SIZE;
    printmsg (msg_ok, active_stream);
}

/** ***************************************************************************
 * @brief Insert new line in the program.
 *
 * This function calculates the space needed by the new line and
 * moves existing code accordingly, so that the new line will fit.
 *
 * @note The position at which the new line should be inserted,
 * is specified by the @c text_ptr pointer.
 *****************************************************************************/
static void insert_line (void)
{
        guchar *source, *dest, *new_end;
        uint16_t tomove, room_to_make;

        while (linelen > 0) {
                // determine misc_print_memory space to reserve
                room_to_make = text_ptr - prog_end_ptr;
                if (room_to_make > linelen)
                        room_to_make = linelen;
                new_end = prog_end_ptr + room_to_make;
                tomove = prog_end_ptr - start;

                // move existing code to make room
                source = prog_end_ptr;
                dest = new_end;
                while (tomove > 0) {
                        source--;
                        dest--;
                        *dest = *source;
                        tomove--;
                }

                // copy content of new line
                for (tomove = 0; tomove < room_to_make; tomove++) {
                        *start = *text_ptr;
                        text_ptr++;
                        start++;
                        linelen--;
                }
                prog_end_ptr = new_end;
        }
}

/** ***************************************************************************
 * @brief Remove line from program.
 *
 * This function removes a line from the programm and moves over
 * the remaining code.
 *
 * @note The line to be deleted is determined by the @c text_ptr pointer.
 *****************************************************************************/
static void remove_line (void)
{
    if (start != prog_end_ptr && * ((uint16_t *)start) == linenum) {
        // calculate the space taken by the line to be deleted
                guchar *dest, *from;
                uint16_t tomove;
                from = start + start[sizeof (LINE_NUMBER)];
                dest = start;
        // copy onver remaing code
                tomove = prog_end_ptr - from;
                while (tomove > 0) {
                        *dest = *from;
                        from++;
                        dest++;
                        tomove--;
                }
                prog_end_ptr = dest;
        }
}

/** ***************************************************************************
 * @brief Move line at the end of program misc_print_memory.
 *
 * This function moves the newly entered line to the end of program misc_print_memory.
 * It is executed whenever the user enters a new line and prior to the
 * interpretation / execution.
 *****************************************************************************/
static void move_line (void)
{
        /* find end of new line */
        text_ptr = prog_end_ptr + sizeof (LINE_NUMBER);
        while (*text_ptr != LF)
        text_ptr++;

        /* move line to the end of program_misc_print_memory */
        guchar *dest;
        dest = (guchar *)variables_ptr - 1;
        while (1) {
                *dest = *text_ptr;
                if (text_ptr == prog_end_ptr + sizeof (LINE_NUMBER))
            break;
                dest--;
                text_ptr--;
        }
        text_ptr = dest;
}

/** ***************************************************************************
 * @brief Prepare line for merging with the program.
 *
 * This function calculates the misc_print_memory space needed for the storing of line,
 * accounting fot the line number as well.
 *****************************************************************************/
static void prep_line (void)
{
    /* find length of line */
    linelen = 0;
    while (text_ptr[linelen] != LF)
        linelen++;
    /* increase to account for LF */
    linelen++;
    /* increase even more for line-header */
    linelen += sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH);
    /* move pointer to the beginning of line header */
    text_ptr -= sizeof (LINE_NUMBER) + sizeof (LINE_LENGTH);
    /* add line number and length*/
    * ((LINE_NUMBER *)text_ptr) = linenum;

    text_ptr[sizeof (LINE_NUMBER)] = linelen;
    //* ((LINE_LENGTH *)(text_ptr + sizeof(LINE_NUMBER))) = linelen;
}

/** ***************************************************************************
 * @brief Print appropriate error mesage.
 *
 * This function resets text and background color and prints an error message.
 *
 * @note The message is indicated by the global variable @c error_code.
 *****************************************************************************/
static void error_message (void)
{
        guchar colour;

        colour = TEXT_COL_ERROR;
        putcmd_gpu (GPU_PEN, 1, &colour);

        colour = BACK_COL_DEFAULT;
        putcmd_gpu (GPU_PAPER, 1, &colour);

        switch (error_code) {
        case 0x1:       // not yet implemented
            printmsg (err_msg01, active_stream);
            break;
        case 0x2:       // syntax error
            printmsg_noNL (err_msg02, active_stream);
            if (line_ptr != NULL) {
                guchar tmp = *text_ptr;
                if (*text_ptr != LF)
                    *text_ptr = '^';

                guchar *list = line_ptr;
                printline (list, active_stream);
                *text_ptr = tmp;
            }
            newline (active_stream);
            break;
        case 0x3:       // stack overflow
            printmsg (err_msg03, active_stream);
            break;
        case 0x4:       // unexpected character
            printmsg (err_msg04, active_stream);
            break;
        case 0x5:       // left parenthesis missing
            printmsg (err_msgxl, active_stream);
            printmsg (err_msg05, active_stream);
            break;
        case 0x6:       // right parenthesis missing
            printmsg (err_msgxr, active_stream);
            printmsg (err_msg05, active_stream);
            break;
        case 0x7:       // variable expected
            printmsg (err_msg07, active_stream);
            break;
        case 0x8:       // jump point not found
            printmsg (err_msg08, active_stream);
            break;
        case 0x9:       // invalid line number
            printmsg (err_msg09, active_stream);
            break;
        case 0xA:       // operator expected
            printmsg (err_msg0A, active_stream);
            break;
        case 0xB:       // division by zero
            printmsg (err_msg0B, active_stream);
            break;
        case 0xC:       // invalid pin
            printmsg (err_msg0C, active_stream);
            break;
        case 0xD:       // pin io error
            printmsg (err_msg0D, active_stream);
            break;
        case 0xE:       // unknown function
            printmsg (err_msg0E, active_stream);
            break;
        case 0xF:       // unknown command
            printmsg (err_msg0F, active_stream);
            break;
        case 0x10:      // invalid coordinates
            printmsg (err_msg10, active_stream);
            break;
        case 0x11:      // invalid variable name
            printmsg (err_msg11, active_stream);
            break;
        case 0x12:      // expected byte
            printmsg (err_msg12, active_stream);
            break;
        case 0x13:      // out of range
            printmsg (err_msg13, active_stream);
            break;
        case 0x14:      // expected color value
            printmsg (err_msg14, active_stream);
            break;
        case 0x15:      // expression expected
            printmsg (err_msg15, active_stream);
            break;
        }

        colour = TEXT_COL_DEFAULT;
        putcmd_gpu (GPU_PEN, 1, &colour);

        colour = BACK_COL_DEFAULT;
        putcmd_gpu (GPU_PAPER, 1, &colour);
}
