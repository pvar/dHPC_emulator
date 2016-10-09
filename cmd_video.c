/*
 * Implementation of graphics related commands of nstBASIC.
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

#include "cmd_video.h"

/* ----------------------------------------------------------------
 * character matrix: 8 x 10
 * 10 bytes per character
 *
 * the table contains 95 characters (ASCII #32 to #126)
 * ---------------------------------------------------------------- */

const uint8_t fontdata[950] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // SPACE
        0x00, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x10, 0x00, 0x00,     // !
        0x00, 0x24, 0x24, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // "
        0x00, 0x24, 0x24, 0x7E, 0x24, 0x7E, 0x24, 0x24, 0x00, 0x00,     // #
        0x00, 0x18, 0x3E, 0x40, 0x3C, 0x02, 0x7C, 0x18, 0x00, 0x00,     // $
        0x00, 0x00, 0x62, 0x64, 0x08, 0x10, 0x26, 0x46, 0x00, 0x00,     // %
        0x00, 0x30, 0x48, 0x30, 0x56, 0x88, 0x88, 0x76, 0x00, 0x00,     // &
        0x00, 0x10, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // '
        0x00, 0x10, 0x20, 0x40, 0x40, 0x40, 0x20, 0x10, 0x00, 0x00,     // (
        0x00, 0x20, 0x10, 0x08, 0x08, 0x08, 0x10, 0x20, 0x00, 0x00,     // )
        0x00, 0x00, 0x44, 0x38, 0xFE, 0x38, 0x44, 0x00, 0x00, 0x00,     // *
        0x00, 0x00, 0x10, 0x10, 0x7C, 0x10, 0x10, 0x00, 0x00, 0x00,     // +
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x20, 0x00,     // ,
        0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00,     // -
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00,     // .
        0x00, 0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00,     // /
        0, 24, 36, 36, 44, 52, 36, 36, 24, 0,                           // 0
        0, 8, 24, 40, 8, 8, 8, 8, 60, 0,                                // 1
        0, 24, 36, 4, 4, 8, 16, 32, 60, 0,                              // 2
        0, 56, 4, 4, 24, 4, 4, 4, 56, 0,                                // 3
        0, 32, 36, 36, 36, 60, 4, 4, 4, 0,                              // 4
        0, 56, 32, 32, 56, 4, 4, 36, 24, 0,                             // 5
        0, 24, 32, 32, 56, 36, 36, 36, 24, 0,                           // 6
        0, 60, 4, 4, 8, 8, 16, 16, 16, 0,                               // 7
        0, 24, 36, 36, 24, 36, 36, 36, 24, 0,                           // 8
        0, 24, 36, 36, 36, 28, 4, 4, 24, 0,                             // 9
        0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00,     // :
        0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x10, 0x10, 0x20, 0x00,     // ;
        0x00, 0x08, 0x10, 0x20, 0x40, 0x20, 0x10, 0x08, 0x00, 0x00,     // <
        0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00,     // =
        0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00,     // >
        0x00, 0x3C, 0x42, 0x02, 0x04, 0x08, 0x00, 0x08, 0x00, 0x00,     // ?
        0x00, 0x3C, 0x42, 0x5E, 0x52, 0x5E, 0x40, 0x3C, 0x00, 0x00,     // @
        0, 56, 68, 68, 68, 124, 68, 68, 68, 0,                          // A
        0, 120, 68, 68, 120, 68, 68, 68, 120, 0,                        // B
        0, 60, 64, 64, 64, 64, 64, 64, 60, 0,                           // C
        0, 120, 68, 68, 68, 68, 68, 68, 120, 0,
        0, 124, 64, 64, 112, 64, 64, 64, 124, 0,
        0, 124, 64, 64, 112, 64, 64, 64, 64, 0,
        0, 56, 64, 64, 64, 76, 68, 68, 56, 0,
        0, 68, 68, 68, 68, 124, 68, 68, 68, 0,
        0, 124, 16, 16, 16, 16, 16, 16, 124, 0,
        0, 124, 16, 16, 16, 16, 16, 16, 96, 0,
        0, 68, 68, 72, 112, 72, 68, 68, 68, 0,
        0, 64, 64, 64, 64, 64, 64, 64, 124, 0,
        0, 68, 68, 108, 84, 68, 68, 68, 68, 0,
        0, 68, 68, 100, 84, 76, 68, 68, 68, 0,
        0, 56, 68, 68, 68, 68, 68, 68, 56, 0,
        0, 120, 68, 68, 68, 120, 64, 64, 64, 0,
        0, 56, 68, 68, 68, 68, 84, 72, 52, 0,
        0, 120, 68, 68, 68, 120, 96, 88, 68, 0,
        0, 60, 64, 64, 56, 4, 4, 4, 120, 0,
        0, 124, 16, 16, 16, 16, 16, 16, 16, 0,
        0, 68, 68, 68, 68, 68, 68, 68, 56, 0,
        0, 68, 68, 68, 68, 68, 40, 40, 16, 0,
        0, 68, 68, 68, 68, 68, 84, 108, 68, 0,
        0, 68, 68, 40, 16, 16, 40, 68, 68, 0,                           // X
        0, 68, 68, 68, 40, 16, 16, 16, 16, 0,                           // Y
        0, 124, 4, 8, 16, 16, 32, 64, 124, 0,                           // Z
        0x00, 0x78, 0x40, 0x40, 0x40, 0x40, 0x40, 0x78, 0x00, 0x00,     // [
        0x00, 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00,     // \ .
        0x00, 0x78, 0x08, 0x08, 0x08, 0x08, 0x08, 0x78, 0x00, 0x00,     // ]
        0x00, 0x10, 0x28, 0x44, 0x82, 0x00, 0x00, 0x00, 0x00, 0x00,     // ^
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x00,     // _
        0x00, 0x10, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,     // `
        0, 0, 0, 56, 4, 60, 68, 68, 56, 0,                              // a
        0, 0, 64, 64, 120, 68, 68, 68, 56, 0,                           // b
        0, 0, 0, 0, 60, 64, 64, 64, 60, 0,                              // c
        0, 0, 4, 4, 60, 68, 68, 68, 56, 0,
        0, 0, 0, 56, 68, 68, 120, 64, 56, 0,
        0, 0, 60, 64, 64, 120, 64, 64, 64, 0,
        0, 0, 0, 56, 68, 68, 68, 60, 4, 56,
        0, 0, 64, 64, 120, 68, 68, 68, 68, 0,
        0, 0, 32, 0, 32, 32, 32, 32, 24, 0,
        0, 0, 8, 0, 8, 8, 8, 8, 8, 48,
        0, 0, 64, 68, 72, 112, 72, 68, 68, 0,
        0, 0, 96, 32, 32, 32, 32, 32, 28, 0,
        0, 0, 0, 104, 84, 68, 68, 68, 68, 0,
        0, 0, 0, 120, 68, 68, 68, 68, 68, 0,
        0, 0, 0, 56, 68, 68, 68, 68, 56, 0,
        0, 0, 0, 56, 68, 68, 68, 120, 64, 64,
        0, 0, 0, 56, 68, 68, 68, 60, 4, 4,
        0, 0, 0, 64, 88, 96, 64, 64, 64, 0,
        0, 0, 0, 56, 64, 48, 8, 8, 112, 0,
        0, 0, 32, 32, 32, 112, 32, 32, 24, 0,
        0, 0, 0, 68, 68, 68, 68, 68, 56, 0,
        0, 0, 0, 68, 68, 68, 68, 40, 16, 0,
        0, 0, 0, 68, 68, 68, 68, 84, 40, 0,
        0, 0, 0, 68, 40, 16, 40, 68, 68, 0,                             // x
        0, 0, 0, 68, 68, 68, 68, 60, 4, 56,                             // y
        0, 0, 0, 120, 8, 16, 32, 64, 120, 0,                            // z
        0x00, 0x0C, 0x10, 0x10, 0x60, 0x10, 0x10, 0x0C, 0x00, 0x00,     // {
        0x00, 0x10, 0x10, 0x10, 0x00, 0x10, 0x10, 0x10, 0x00, 0x00,     // |
        0x00, 0x30, 0x08, 0x08, 0x06, 0x08, 0x08, 0x30, 0x00, 0x00,     // }
        0x00, 0x32, 0x4C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00      // ~
};

const uint8_t logoimg[728] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 0, 0, 0, 0, 255, 255, 255, 255, 255, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 67, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 255, 255, 255, 255, 255, 255, 0, 0, 255, 255, 255, 255, 255, 255, 255, 0, 0,
        0, 0, 0, 0, 0, 0, 67, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 255, 255, 0, 255, 255, 255, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 67, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 255, 0, 0, 0, 0, 255, 255, 0, 255, 255, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 67, 67, 0, 0, 0, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 170, 170, 0, 0, 0, 0, 170, 170, 0, 170, 170, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 67, 67, 0, 0, 143, 143, 143, 143, 143, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 0, 112, 112, 112, 112, 112, 0, 0, 170, 170, 170, 170, 170, 170, 170, 170, 0, 170, 170, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 67, 67, 67, 67, 67, 67, 0, 143, 143, 0, 0, 0, 143, 143, 0, 76, 76, 0, 188, 188, 188, 188, 0, 0, 0, 0, 0, 0, 112, 112, 0, 170, 170, 170, 170, 170, 170, 170, 0, 0, 170, 170, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 67, 67, 0, 0, 0, 67, 67, 0, 143, 143, 0, 0, 0, 143, 143, 0, 76, 76, 0, 0, 188, 188, 0, 0, 0, 0, 0, 0, 0, 112, 112, 0, 170, 170, 0, 0, 0, 0, 0, 0, 0, 170, 170, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 67, 67, 0, 0, 0, 67, 67, 0, 143, 143, 143, 143, 143, 143, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 0, 112, 112, 112, 112, 112, 112, 0, 85, 85, 0, 0, 0, 0, 0, 0, 0, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 67, 67, 0, 0, 0, 67, 67, 0, 143, 143, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 112, 112, 0, 0, 0, 112, 112, 0, 85, 85, 0, 0, 0, 0, 0, 0, 0, 85, 85, 85, 0, 0, 0, 0, 0, 0, 0,
        0, 67, 67, 0, 0, 0, 67, 67, 0, 143, 143, 0, 0, 0, 0, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 112, 112, 0, 0, 0, 112, 112, 0, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 85, 85, 85, 85, 85, 85, 85, 0, 0,
        0, 0, 67, 67, 67, 67, 67, 0, 0, 0, 143, 143, 143, 143, 143, 0, 0, 0, 76, 76, 0, 0, 188, 188, 0, 0, 112, 112, 112, 112, 112, 0, 0, 85, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 85, 85, 85, 85, 85, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};


uint8_t reset_display (void)
{
//        putchar (vid_reset);
        printmsg (msg_welcome, active_stream);
        prog_end_ptr = program_space;
        return POST_CMD_PROMPT;
}

uint8_t clear_screen (void)
{
//        putchar (vid_clear);
        return POST_CMD_NEXT_STATEMENT;
}

uint8_t pen (void)
{
        uint16_t col;
        // get color value
        col = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (col < 0 || col > 127) {
                error_code = 0x14;
                return POST_CMD_WARM_RESET;
        }
        text_color ((uint8_t)col);
        return POST_CMD_NEXT_STATEMENT;
}

uint8_t paper (void)
{
        uint16_t col;
        // get color value
        col = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (col < 0 || col > 127) {
                error_code = 0x14;
                return POST_CMD_WARM_RESET;
        }
        paper_color ((uint8_t)col);
        return POST_CMD_NEXT_STATEMENT;
}

uint8_t locate (void)
{
        uint16_t line, column;
        // get target line
        line = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (line < 0 || line > 23) {
                error_code = 0x10;
                return POST_CMD_WARM_RESET;
        }
        // check for comma
        if (*text_ptr != ',') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        // get target line
        column = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (column < 0 || column > 31) {
                error_code = 0x10;
                return POST_CMD_WARM_RESET;
        }
        locate_cursor (line, column);
        return POST_CMD_NEXT_STATEMENT;
}

uint8_t print (void)
{
        // If we have an empty list then just put out a LF
        if (*text_ptr == ':') {
                newline (active_stream);
                text_ptr++;
        return POST_CMD_NEXT_STATEMENT;
        }
        if (*text_ptr == LF)
        return POST_CMD_NEXT_LINE;
        while (1) {
                ignorespace();
                if (print_string())
                        ;
                else if (*text_ptr == '"' || *text_ptr == '\'') {
                        error_code = 0x4;
                        return POST_CMD_WARM_RESET;
                } else {
                        uint16_t e;
                        error_code = 0;
                        e = parse_expr_s1();
                        if (error_code) {
                                return POST_CMD_WARM_RESET;
                        }
                        printnum (e, active_stream);
                }
                ignorespace();
                // skip comma and continue printing
                if (*text_ptr == ',')
                        text_ptr++;
                // stop printing without newline
                else if (text_ptr[0] == ';' && (text_ptr[1] == LF || text_ptr[1] == ':')) {
                        text_ptr++;
                        break;
                // stop printing with newline
                } else if (*text_ptr == LF || *text_ptr == ':') {
                        newline (active_stream);
                        break;
                // unexpected character...
                } else {
                        error_code = 0x2;
                        return POST_CMD_WARM_RESET;
                }
        }
        return POST_CMD_NEXT_STATEMENT;
}

uint8_t pset (void)
{
        uint16_t x, y, col;
        // get x-coordinate
        x = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (x < 0 || x > 255) {
                error_code = 0x10;
                return POST_CMD_WARM_RESET;
        }
        // check for comma
        if (*text_ptr != ',') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        // get y-coordinate
        y = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (y < 0 || y > 239) {
                error_code = 0x10;
                return POST_CMD_WARM_RESET;
        }
        // check for comma
        if (*text_ptr != ',') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        // get color
        col = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (col < 0 || col > 127) {
                error_code = 0x14;
                return POST_CMD_WARM_RESET;
        }
        put_pixel ((uint8_t)x, (uint8_t)y, (uint8_t)col);
        return POST_CMD_NEXT_STATEMENT;
}
