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

#ifndef CMD_SCREEN_H
#define CMD_SCREEN_H

#include <stdio.h>
#include <stdlib.h>
#include "cpu_interpreter.h"
#include "cpu_parser.h"

uint8_t vid_reset (void);
uint8_t vid_clear (void);
uint8_t vid_set_pen_colour (void);
uint8_t vid_set_paper_colour(void);
uint8_t vid_locate_cursor (void);
uint8_t vid_put_pixel (void);
void vid_put_character (uint8_t chr);

const uint8_t logoimg[728];
const uint8_t fontdata[950];


#define CHAR_PER_LINE 32
#define TEXT_COL_DEFAULT 7
#define BACK_COL_DEFAULT 0
#define TEXT_COL_ERROR 3

uint8_t cursor_x, cursor_y;

#endif
