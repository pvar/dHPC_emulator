/*
 * Parser functions for nstBASIC.
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
 * @file parser.c
 * @brief Syntactic analysis and expression evaluation.
 *
 * The functions @c parse_expr_s# call each other recursivelly, until the given expression is
 * evaluated. The first of them handles relational operators, the second one handles additions
 * and sutractions, the third handles multiplications and divisions, while the fourth substitutes
 * variables with the respective values, calls other arithmetic functions, or could even start
 * the whole 'chain' of parsing from the beginning, in order to evaluate a sub-expression. The
 * representation of a mathematical expression in the form of a tree could help understand why
 * this works and how the operator precedence is honored. The rest functions parse the special
 * strings used for describing sequences of notes and in essence, complete melodies.
 */

#include "cpu_parser.h"

/// @cond BASIC_KEYWORDS

// functions which cannot be part of a larger expression  (return nothing / might print a value)
const uint8_t commands[208] = {
            'L', 'I', 'S', 'T' + 0x80,
            'N', 'E', 'W' + 0x80,
            'R', 'U', 'N' + 0x80,
            'N', 'E', 'X', 'T' + 0x80,
            'L', 'E', 'T' + 0x80,
            'I', 'F' + 0x80,
            'G', 'O', 'T', 'O' + 0x80,
            'M', 'P', 'L', 'A', 'Y' + 0x80,
            'M', 'S', 'T', 'O', 'P' + 0x80,
            'T', 'E', 'M', 'P', 'O' + 0x80,
            'M', 'U', 'S', 'I', 'C' + 0x80,
            'G', 'O', 'S', 'U', 'B' + 0x80,
            'R', 'E', 'T', 'U', 'R', 'N' + 0x80,
            'R', 'A', 'N', 'D', 'O', 'M', 'I', 'Z', 'E' + 0x80,
            'R', 'N', 'D', 'S', 'E', 'E', 'D' + 0x80,
            'R', 'S', 'T' + 0x80,
            'C', 'L', 'S' + 0x80,
            'R', 'E', 'M' + 0x80,
            'F', 'O', 'R' + 0x80,
            'I', 'N', 'P', 'U', 'T' + 0x80,
            'B', 'E', 'E', 'P' + 0x80,
            'P', 'R', 'I', 'N', 'T' + 0x80,
            'L', 'O', 'C', 'A', 'T', 'E' + 0x80,
            'P', 'O', 'K', 'E' + 0x80,
            'P', 'S', 'E', 'T' + 0x80,
            'S', 'T', 'O', 'P' + 0x80,
            'E', 'N', 'D' + 0x80,
            'M', 'E', 'M' + 0x80,
            'P', 'E', 'N' + 0x80,
            'P', 'A', 'P', 'E', 'R' + 0x80,
            '?' + 0x80,
            '#' + 0x80,
            '\'' + 0x80,
            'D', 'E', 'L', 'A', 'Y' + 0x80,
            'F', 'I', 'L', 'E', 'S' + 0x80,
            'C', 'F', 'O', 'R', 'M', 'A', 'T' + 0x80,
            'C', 'C', 'H', 'A', 'I', 'N' + 0x80,
            'C', 'S', 'A', 'V', 'E' + 0x80,
            'C', 'L', 'O', 'A', 'D' + 0x80,
            'P', 'I', 'N', 'D', 'I', 'R' + 0x80,
            'P', 'I', 'N', 'D', 'W', 'R', 'I', 'T', 'E' + 0x80,
            0
    };
// functions that must be part of a larger expression (return a value / print nothing)
const uint8_t functions[27] = {
            'P', 'E', 'E', 'K' + 0x80,
            'A', 'B', 'S' + 0x80,
            'R', 'N', 'D' + 0x80,
            'P', 'I', 'N', 'D', 'R', 'E', 'A', 'D' + 0x80,
            'P', 'I', 'N', 'A', 'R', 'E', 'A', 'D' + 0x80,
            0
    };
// relational operators
const uint8_t relop_table[12] = {
            '>', '=' + 0x80,
            '<', '>' + 0x80,
            '>' + 0x80,
            '=' + 0x80,
            '<', '=' + 0x80,
            '<' + 0x80,
            '!', '=' + 0x80,
            0
    };
// other keywords
const uint8_t to_tab[3] = {
            'T', 'O' + 0x80,
            0
    };
const uint8_t step_tab[5] = {
            'S', 'T', 'E', 'P' + 0x80,
            0
    };
const uint8_t highlow_tab[12] = {
            'H', 'I', 'G', 'H' + 0x80,
            'H', 'I' + 0x80,
            'L', 'O', 'W' + 0x80,
            'L', 'O' + 0x80,
            0
    };

/// @endcond

static int16_t parse_expr_s2 (void);
static int16_t parse_expr_s3 (void);
static int16_t parse_expr_s4 (void);
static uint8_t get_note (void);
static uint8_t get_effect (void);
static uint8_t get_duration (void);
static uint8_t get_octave (void);

/** ***************************************************************************
 * @brief Search for any string in specified table.
 *
 * This function scans current line and looks for any string contained in the
 * specified array.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @param table A pointer to an array of strings, located in FLASH.
 * @return The position (index) of the detected string in array.
 *****************************************************************************/
int8_t scantable (const uint8_t *table)
{
        int8_t position = 0;
        uint16_t pos = 0;

        while (1) {
                // check if at the end of table...
                if (* (table) == 0)
                        return position;

                // check for character match...
                if (text_ptr[pos] == * (table)) {
                        pos++;
                        table++;
                } else {
                        // check if this is the last character of a keyword (add 0x80)
                        if (text_ptr[pos] + 0x80 == *(table)) {
                                text_ptr += pos + 1; // points after the detected keyword
                                ignorespace();
                                return position;
                        }
                        // move to the end of this keyword
                        while ((*(table) & 0x80) == 0)
                                table++;
                        // move to first character of next keyword
                        table++;
                        // increase pointer
                        position++;
                        ignorespace();
                        pos = 0;
                }
        }
        return -1;
}

/** ***************************************************************************
 * @brief Search for an effect mark.
 *
 * This function examines current line and searches for a valid effect mark.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return A number indicating the defined effect.
 *****************************************************************************/
static uint8_t get_effect (void)
{
        // EFFECT                       RETURN VALUE
        //      none                            0
        //      bend up                         1
        //      bend down                       2
        //      vibrato                         3
        text_ptr++;
        switch (*text_ptr) {
        case '=':       // vibrato
            return 3;
            break;
        case '-':       // bend down
            return 2;
            break;
        case '+':       // bend up
            return 1;
            break;
        default:        // no effect
            text_ptr--;
            return 0;
            break;
        }
}

/** ***************************************************************************
 * @brief Search for channel number.
 *
 * This function examines current line and searches for sound-channel number.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 *****************************************************************************/
void parse_channel (void)
{
        text_ptr++;
        if (*text_ptr >= '1' && *text_ptr <= '4') {
                //send_to_apu (*text_ptr - '0');
                return;
        } else {
                error_code = 0x4;
                return;
        }
}

/** ***************************************************************************
 * @brief Search for notes.
 *
 * This function examines current line and searches for notes.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 *****************************************************************************/
void parse_notes (void)
{
        uint8_t tmp1, tmp2, tmp3, tmp4;
        uint8_t params, note;
        while (1) {
                //text_ptr++;
                tmp1 = get_octave();
                // check separately -- exit if no other notes
                if (tmp1 == 0)
                        return;
                tmp2 = get_note();
                tmp3 = get_duration();
                tmp4 = get_effect();
                // check separately -- exit on errors
                if ((tmp2 | tmp3 | tmp4) == 0)
                        return;
                if (tmp2 == 13)
                        note = 144;
                else
                        note = (24 * (tmp1 - 2)) + 2 * (tmp2 - 1);
                params = (tmp3 - 1) + (tmp4 * 64);
                g_print("%c", params);
                g_print("%c", note);
                //send_to_apu (params);
                //send_to_apu (note);
        }
}

/** ***************************************************************************
 * @brief Search for relational operators.
 *
 * This function examines current line and searches for any relational operator.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return The result from the evaluation of the whole expression.
 *****************************************************************************/
int16_t parse_expr_s1 (void)
{
        int16_t value1, value2;
        uint8_t index;

        value1 = parse_expr_s2();

        // check for error
        if (error_code)
                return value1;
        index = scantable (relop_table);
        if (index == RELOP_UNKNOWN)
                return value1;
        switch (index) {
                case RELOP_GE:
                        value2 = parse_expr_s2();
                        if (value1 >= value2)
                                return 1;
                        break;
                case RELOP_NE:
                case RELOP_NE_BANG:
                        value2 = parse_expr_s2();
                        if (value1 != value2)
                                return 1;
                        break;
                case RELOP_GT:
                        value2 = parse_expr_s2();
                        if (value1 > value2)
                                return 1;
                        break;
                case RELOP_EQ:
                        value2 = parse_expr_s2();
                        if (value1 == value2)
                                return 1;
                        break;
                case RELOP_LE:
                        value2 = parse_expr_s2();
                        if (value1 <= value2)
                                return 1;
                        break;
                case RELOP_LT:
                        value2 = parse_expr_s2();
                        if (value1 < value2)
                                return 1;
                        break;
        }
        return 0;
}

/** ***************************************************************************
 * @brief Search for subtraction or addition operators.
 *
 * This function examines current line and looks for a subtraction
 * or an addition operator.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return The result from the evaluation of the sub-expression.
 *****************************************************************************/
static int16_t parse_expr_s2 (void)
{
        int16_t value1, value2;

        if (*text_ptr == '-' || *text_ptr == '+')
                value1 = 0;
        else
                value1 = parse_expr_s3();
        while (1) {
                if (*text_ptr == '-') {
                        text_ptr++;
                        value2 = parse_expr_s3();
                        value1 -= value2;
                } else if (*text_ptr == '+') {
                        text_ptr++;
                        value2 = parse_expr_s3();
                        value1 += value2;
                } else
                        return value1;
        }
}

/** ***************************************************************************
 * @brief Search for multiplication or division operators.
 *
 * This function examines current line and looks for a multiplication
 * or a division operator.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return The result from the evaluation of the sub-expression.
 *****************************************************************************/
static int16_t parse_expr_s3 (void)
{
        int16_t value1, value2;

        value1 = parse_expr_s4();

        ignorespace();
        while (1) {
                if (*text_ptr == '*') {
                        text_ptr++;
                        value2 = parse_expr_s4();
                        value1 *= value2;
                } else if (*text_ptr == '/') {
                        text_ptr++;
                        value2 = parse_expr_s4();
                        if (value2 != 0)
                                value1 /= value2;
                        else
                                error_code = 0xB;
                } else
                        return value1;
        }
}

/** ***************************************************************************
 * @brief Search for numbers, variables or functions.
 *
 * This function examines current line and looks for string representations
 * of numbers, variables and functions.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return The result from the evaluation of the sub-expression.
 *****************************************************************************/
static int16_t parse_expr_s4 (void)
{
        uint8_t index;
        int16_t value1 = 0;
        int16_t value2 = 0;

        /////////////////////////////////////////////////////////////////////////// numbers
        ignorespace();
        // check for minus sign
        if (*text_ptr == '-') {
                text_ptr++;
                return -parse_expr_s4();
        }
        // leading zeros are not allowed
        if (*text_ptr == '0') {
                text_ptr++;
                return 0;
        }
        // calculate value of given number
        if (*text_ptr >= '1' && *text_ptr <= '9') {
                do {
                        value1 = value1 * 10 + *text_ptr - '0';
                        text_ptr++;
                } while (*text_ptr >= '0' && *text_ptr <= '9');
                return value1;
        }

        /////////////////////////////////////////////////////////////////////////// variables
        if (text_ptr[0] >= 'A' && text_ptr[0] <= 'Z') {
                // check next character -- variable names are single letters
                if (text_ptr[1] < 'A' || text_ptr[1] > 'Z') {
                        // return a pointer to the referenced variable
                        value2 = ((int16_t *)variables_ptr)[*text_ptr - 'A'];
                        text_ptr++;
                        return value2;
                }
                /////////////////////////////////////////////////////////////////////////// functions
                // search table with function names
                index = scantable (functions);
                if (index == FN_UNKNOWN) {
                        error_code = 0xE;
                        return 0;
                }
                // check for left parenthesis
                if (*text_ptr != '(') {
                        error_code = 0x5;
                        return 0;
                }
                text_ptr++;
                // get parameter
                value1 = parse_expr_s1();
                // check for right parenthesis
                if (*text_ptr != ')') {
                        error_code = 0x6;
                        return 0;
                }
                text_ptr++;
                switch (index) {
                //-----------------------------------------------------------------
                case FN_PEEK:
                        if (value1 > MEMORY_SIZE) {
                                error_code = 0x13;
                                return 0;
                        }
                        return program_space[value1];
                //-----------------------------------------------------------------
                case FN_ABS:
                        if (value1 < 0)
                return -value1;
                        return value1;
                //-----------------------------------------------------------------
                case FN_RND:
                        return rand() % value1;
                //-----------------------------------------------------------------
                case FN_PINDREAD:
                        /*
                        // expected a pin number (0..7)
                        if (value1 < 0 || value1 > 7) {
                                error_code = 0xC;
                                return 0;
                        }
                        // create bit mask for following checks
                        value1 = 1 << value1;
                        // selected pin should be configured as input
                        if (sec_data_bus_dir & value1) {
                                error_code = 0xD;
                                return 0;
                        }
                        // get the digital value
                        if (sec_data_bus_in & value1)
                                return 1;
                        return 0;
                        */
                        break;
                //-----------------------------------------------------------------
                case FN_PINAREAD:
                        /*
                        // expected a pin number (0..7)
                        if (value1 < 0 || value1 > 7) {
                                error_code = 0xC;
                                return 0;
                        }
                        // enable specified input channel
                        ADMUX = value1;
                        // create bit mask for following checks
                        value1 = 1 << value1;
                        // disable pull-up on selected pin
                        value2 = sec_data_bus_out;
                        sec_data_bus_out &= ~value1;
                        // selected pin should be configured as input
                        if (sec_data_bus_dir & value1) {
                                error_code = 0xD;
                                return 0;
                        }
                        // get the analog value
                        ADCSRA |= _BV (ADSC);
                        while (ADCSRA & _BV (ADSC));
                        // restore state of pull-up
                        sec_data_bus_out = value2;
                        return ADCW >> 1;
                        */
                        break;
                }
        }
// ------------------------------------------------------------------- expression in parenthesis
        if (*text_ptr == '(') {
                text_ptr++;
                value1 = parse_expr_s1();
                if (*text_ptr != ')') {
                        error_code = 0x6;
                        return 0;
                }
                text_ptr++;
                return value1;
        }
    /* SOME APPROPRIATE ERROR CODE */
    error_code = 0x15;
    return 0;
}

/** ***************************************************************************
 * @brief Get single note.
 *
 * This function examines current line and looks for the string representation
 * of a note.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return A number indicating the defined note.
 *****************************************************************************/
static uint8_t get_note (void)
{
        // NOTE         RETURN VALUE
        //      C                       01
        //      C#                      02
        //      D                       03
        //      Eb                      04
        //      E                       05
        //      F                       06
        //      F#                      07
        //      G                       08
        //      G#                      09
        //      A                       10
        //      Bb                      11
        //      B                       12
        //      PAUSE           13
        text_ptr++;
        switch (*text_ptr) {
        case 'C':
        case 'c':
            text_ptr++;
            if (*text_ptr == '#')
                return 2;
            else {
                text_ptr--;
                return 1;
            }
            break;
        case 'D':
        case 'd':
            return 3;
            break;
        case 'E':
        case 'e':
            text_ptr++;
            if (*text_ptr == 'B' || *text_ptr == 'b')
                return 4;
            else {
                text_ptr--;
                return 5;
            }
            break;
        case 'F':
        case 'f':
            text_ptr++;
            if (*text_ptr == '#')
                return 7;
            else {
                text_ptr--;
                return 6;
            }
            break;
        case 'G':
        case 'g':
            text_ptr++;
            if (*text_ptr == '#')
                return 9;
            else {
                text_ptr--;
                return 8;
            }
            break;
        case 'A':
        case 'a':
            return 10;
            break;
        case 'B':
        case 'b':
            text_ptr++;
            if (*text_ptr == 'B' || *text_ptr == 'b')
                return 11;
            else {
                text_ptr--;
                return 12;
            }
            break;
        case 'P':
        case 'p':
            return 13;
            break;
        default:
            error_code = 0x4;
            break;
        }
    return 0;
}

/** ***************************************************************************
 * @brief Get octave.
 *
 * This function examines current line and looks for the string representation
 * of an octave number.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return A number indicating the defined octave.
 *****************************************************************************/
static uint8_t get_octave (void)
{
        // OCTAVE               RETURN VALUE
        //      2                               2
        //      ...                             ...
        //      7                               7
        text_ptr++;
        if (*text_ptr >= '2' && *text_ptr <= '7')
                return *text_ptr - '0';
        else if (*text_ptr == '"' || *text_ptr == '\'') {
                text_ptr--;
                return 0;
        } else {
                error_code = 0x4;
                return 0;
        }
}

/** ***************************************************************************
 * @brief Get duration value.
 *
 * This function examines current line and looks for the string representation
 * of a note duration.
 *
 * @note Scanning begins at the character pointed to by @c text_ptr.
 * @return A number indicating the defined duration.
 *****************************************************************************/
static uint8_t get_duration (void)
{
        // DURATION                     RETURN VALUE
        //      1/32                            1
        //      1/16                            2
        //      1/16* (3/32)            3
        //      1/8                                     4
        //      1/8*  (3/16)            5
        //      1/4                                     6
        //      1/4*  (3/8)                     7
        //      1/2                                     8
        text_ptr++;
        if (*text_ptr >= '1' && *text_ptr <= '8')
                return *text_ptr - '0';

        error_code = 0x4;
        return 0;
}
