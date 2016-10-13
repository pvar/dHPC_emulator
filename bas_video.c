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

#include "bas_video.h"

guchar vid_reset (void)
{
        putcmd_gpu (GPU_RESET, 0, NULL);
        misc_clear_program();
        printmsg (msg_welcome, active_stream);
        return POST_CMD_PROMPT;
}

guchar vid_clear (void)
{
        putcmd_gpu (GPU_CLEAR, 0, NULL);
        return POST_CMD_NEXT_STATEMENT;
}

guchar vid_set_pen_colour (void)
{
        uint16_t colour_value;
        guchar colour;
        // get color value
        colour_value = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        if (colour_value < 0 || colour_value > 127) {
                error_code = 0x14;
                return POST_CMD_WARM_RESET;
        }
        colour = (guchar)colour_value;
        putcmd_gpu (GPU_PEN, 1, &colour);
        return POST_CMD_NEXT_STATEMENT;
}

guchar vid_set_paper_colour(void)
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
        //paper_color ((uint8_t)col);
        //colour_paper = (uint8_t)col;
        return POST_CMD_NEXT_STATEMENT;
}

guchar vid_locate_cursor (void)
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
        // vid_locate_cursor (line, column);
        cursor_x = column;
        cursor_y = line;
        return POST_CMD_NEXT_STATEMENT;
}

guchar vid_put_pixel (void)
{
        gint x, y;
        guchar col;

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
        //vid_put_pixel ((guchar)x, (guchar)y, (guchar)col);
        return POST_CMD_NEXT_STATEMENT;
}
