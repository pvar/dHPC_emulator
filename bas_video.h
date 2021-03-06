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

#ifndef BAS_VIDEO_H
#define BAS_VIDEO_H

#include <stdio.h>
#include <stdlib.h>

#include "cpu_interpreter.h"
#include "cpu_parser.h"

guchar vid_reset (void);
guchar vid_clear (void);
guchar vid_set_pen_colour (void);
guchar vid_set_paper_colour (void);
guchar vid_locate_cursor (void);
guchar vid_put_pixel (void);


#endif
