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

/* FLOW CONTROL */
uint8_t flow_goto (void);
uint8_t flow_forloop (void);
uint8_t flow_next (void);
uint8_t flow_gosub (void);
uint8_t flow_return (uint8_t cmd);
int8_t  flow_run (void);
int8_t  flow_end (void);

/* MISCELLANEOUS */
uint8_t misc_conditional (void);
uint8_t misc_print (void);
int8_t  misc_list (void);
int8_t  misc_assignment (void);
int8_t  misc_clear_program (void);
int8_t  misc_get_value (void);
int8_t  misc_print_mem (void);
int8_t  misc_poke_mem (void);

/* PRNG */
int8_t  prng_seed_refresh (void);
int8_t  prng_seed_define (void);

/* GPIO */
uint8_t gpio_write_digital (void);
uint8_t gpio_set_direction (void);

#endif
