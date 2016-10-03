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

#ifndef CMD_OTHER_H
#define CMD_OTHER_H

#include <stdio.h>
#include <stdlib.h>
#include "interpreter.h"
#include "parser.h"

int8_t prog_run (void);
int8_t prog_end (void);
int8_t prog_new (void);
int8_t input (void);
int8_t assignment (void);
int8_t poke (void);
int8_t list (void);
int8_t mem (void);
int8_t randomize (void);
int8_t rndseed (void);

#endif
