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

#ifndef GPU_BUFFER_H
#define GPU_BUFFER_H

#include <stdio.h>
#include <stdlib.h>

#include "thread_gpu.h"

void print_character (guchar chr);
void draw_printable (guchar chr);

void reset_buffer (void);
void clear_buffer (void);
void set_pen_colour (void);
void set_paper_colour(void);
void locate_cursor (void);
void put_pixel (void);
void put_logo (void);

const guchar logoimg[728];
const guchar fontdata[950];

#endif
