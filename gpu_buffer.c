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

#include "gpu_buffer.h"

/* ----------------------------------------------------------------
 * character matrix: 8 x 10
 * 10 bytes per character
 *
 * the table contains 95 characters (ASCII #32 to #126)
 * ---------------------------------------------------------------- */
const guchar fontdata[950] = {
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
        0, 0, 0, 60, 64, 64, 64, 64, 60, 0,                             // c
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

const guchar logoimg[728] = {
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

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void clear_buffer (void)
{
        gint pxl_ptr;
        guchar *pixel;

        /* fill frame buffer with paper colour */
        for (pxl_ptr = 0; pxl_ptr < FB_HEIGHT * FB_WIDTH; pxl_ptr++) {
                pixel = &dhpc->pixelbuffer[pxl_ptr * 3];
                *pixel     = colour_paper.red;
                *(pixel+1) = colour_paper.green;
                *(pixel+2) = colour_paper.blue;
        }
        /* display updated frame buffer */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);
        /* reset cursor position */
        cursor_x = 0;
        cursor_y = 0;
}


/** ***************************************************************************
 * @brief
 *****************************************************************************/

void put_pixel (guchar x, guchar y, guchar colour)
{
        guchar *pixel;
        struct rgb_triad pixel_col = color_converter(colour);

        /* draw pixel on fram buffer */
        pixel = &dhpc->pixelbuffer[(x + 2 * y * FB_WIDTH) * 3];
        *pixel     = pixel_col.red;
        *(pixel+1) = pixel_col.green;
        *(pixel+2) = pixel_col.blue;
        *(pixel+3) = pixel_col.red;
        *(pixel+4) = pixel_col.green;
        *(pixel+5) = pixel_col.blue;
        /* display updated frame buffer */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void put_logo (void)
{
        guchar image_line, image_data;
        guint image_offset, fb_offset;

        guchar pixel_ptr;
        guchar *pixel;
        struct rgb_triad pixel_col;

        for (image_line = 0; image_line < 14; image_line++) {

                fb_offset = (image_line + 100) * FB_WIDTH + 102;
                image_offset = image_line * 52;

                for (pixel_ptr = 0; pixel_ptr < 52; pixel_ptr++) {
                        // get colour from table and convert it to RGB values */
                        image_data = logoimg[image_offset + pixel_ptr];
                        pixel_col = color_converter(image_data);
                        // put pixel in frame buffer */
                        pixel = &dhpc->pixelbuffer[(fb_offset + pixel_ptr) * 6];
                        *pixel     = pixel_col.red;
                        *(pixel+1) = pixel_col.green;
                        *(pixel+2) = pixel_col.blue;
                        *(pixel+3) = pixel_col.red;
                        *(pixel+4) = pixel_col.green;
                        *(pixel+5) = pixel_col.blue;
                }
        }
        /* display updated frame buffer */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void scroll_buffer (void)
{
        gint chr_line,pxl_ptr, pxl_line;
        gint offset;
        gint line_offset;
        guchar *pixel1;
        guchar *pixel2;

        /* move character-lines up */
        line_offset = 20 * FB_WIDTH;
        for (chr_line = 0; chr_line < LINES_PER_FRAME - 1; chr_line++) {
                for (pxl_line = 0; pxl_line < 2 * PXL_LINES_PER_CHAR; pxl_line += 2) {
                        offset = (chr_line * 20 + pxl_line) * FB_WIDTH;
                        for (pxl_ptr = 0; pxl_ptr < FB_WIDTH; pxl_ptr++) {
                                pixel1 = &dhpc->pixelbuffer[(offset + pxl_ptr) * 3];
                                pixel2 = &dhpc->pixelbuffer[(offset + pxl_ptr + line_offset) * 3];
                                *pixel1     = *pixel2;
                                *(pixel1+1) = *(pixel2+1);
                                *(pixel1+2) = *(pixel2+2);
                        }
                }
        }
        /* clear last character-line */
        for (pxl_line = 0; pxl_line < 2 * PXL_LINES_PER_CHAR; pxl_line += 2) {
                offset = (FB_HEIGHT - 20 + pxl_line) * FB_WIDTH;
                for (pxl_ptr = 0; pxl_ptr < FB_WIDTH; pxl_ptr++) {
                        pixel1 = &dhpc->pixelbuffer[(offset + pxl_ptr) * 3];
                        *pixel1     = colour_paper.red;
                        *(pixel1+1) = colour_paper.green;
                        *(pixel1+2) = colour_paper.blue;
                }
        }
        /* display updated frame buffer */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void print_character (guchar chr)
{
        if ((chr > 31) && (chr <= 126)) {
                draw_printable (chr);
                return;
        }

        switch (chr) {
                case LF:
                case CR:
                        cursor_x = 0;
                        cursor_y++;
                        if (cursor_y > LINES_PER_FRAME - 1) {
                                cursor_y = LINES_PER_FRAME - 1;
                                scroll_buffer();
                        }
                        break;
                case TAB:
                        cursor_x += (4 - (cursor_x % 4));
                        break;
                case BS:
                        cursor_x--;
                        draw_printable (SPACE);
                        cursor_x--;
                        break;
                case HOME:
                        break;
                case END:
                        break;
                case AR_LFT:
                        break;
                case AR_RGT:
                        break;
        }
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void draw_printable (guchar chr)
{
        guchar font_line, font_data, pxl_ptr;
        guchar *pixel;
        guint pixel_col, pixel_row, fb_offset;


        if (cursor_y > 23) {
                g_print("WARNING:\n");
                g_print("Cannot print; Cursor-Y value is %i\n",cursor_y);
                return;
        }

        for (font_line = 0; font_line < PXL_LINES_PER_CHAR; font_line++) {
                pixel_col = cursor_x * 8;
                pixel_row = cursor_y * PXL_LINES_PER_CHAR + font_line;
                fb_offset = pixel_row * FB_WIDTH + pixel_col;

                font_data = fontdata[(chr - 32) * PXL_LINES_PER_CHAR + font_line];
                for (pxl_ptr = 0; pxl_ptr < 8; pxl_ptr++) {
                        pixel = &dhpc->pixelbuffer[(fb_offset + pxl_ptr) * 6];
                        if (font_data & 128) {
                                *pixel     = colour_pen.red;
                                *(pixel+1) = colour_pen.green;
                                *(pixel+2) = colour_pen.blue;
                                *(pixel+3) = colour_pen.red;
                                *(pixel+4) = colour_pen.green;
                                *(pixel+5) = colour_pen.blue;
                        } else {
                                *pixel     = colour_paper.red;
                                *(pixel+1) = colour_paper.green;
                                *(pixel+2) = colour_paper.blue;
                                *(pixel+3) = colour_paper.red;
                                *(pixel+4) = colour_paper.green;
                                *(pixel+5) = colour_paper.blue;
                        }
                        font_data = font_data << 1;
                }
        }
        /* display updated frame buffer */
        gtk_image_set_from_pixbuf (dhpc->screen, dhpc->framebuffer);

        cursor_x++;
        if (cursor_x == CHARS_PER_LINE) {
                cursor_x = 0;
                cursor_y++;
                if (cursor_y > LINES_PER_FRAME - 1) {
                        cursor_y = LINES_PER_FRAME - 1;
                        scroll_buffer();
                }
        }
}
