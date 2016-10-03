/*
 * Implementation of IO pin-control commands of nstBASIC.
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

#include "cmd_pinctl.h"

uint8_t pindir (void)
{
/*
        uint16_t a, b;
        // get pin number [0..7]
        a = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        // check range
        if (a < 0 || a > 7) {
                error_code = 0xC;
                return POST_CMD_WARM_RESET;
        }
        // check for comma
        if (*text_ptr != ',') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        // get direction [0/1]
        b = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        // create mask for altering direction
        a = 1 << a;
        switch (b) {
                case 0:
                        sec_data_bus_dir &= ~a;
                        break;
                case 1:
                        sec_data_bus_dir |= a;
                        break;
                default:
                        // direction can only be 1 or 0
                        error_code = 0x2;
                        return POST_CMD_WARM_RESET;
        }
        return POST_CMD_NEXT_LINE;
*/
        return 0;
}

uint8_t pindwrite (void)
{
/*
        uint16_t a, b;
        // get pin number [0..7]
        a = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        // check range
        if (a < 0 || a > 7) {
                error_code = 0xC;
                return POST_CMD_WARM_RESET;
        }
        // check for comma -- two parameters expected
        if (*text_ptr != ',') {
                error_code = 0x2;
                return POST_CMD_WARM_RESET;
        }
        text_ptr++;
        // get value [0/1]
        b = parse_expr_s1();
        if (error_code)
                return POST_CMD_WARM_RESET;
        // create mask for altering direction
        a = 1 << a;
        switch (b) {
                case 0:
                        sec_data_bus_out &= ~a;
                        break;
                case 1:
                        sec_data_bus_out |= a;
                        break;
                default:
                        // specified value can only be 1 or 0
                        error_code = 0x2;
                        return POST_CMD_WARM_RESET;
        }
        return POST_CMD_NEXT_LINE;
*/
        return 0;
}
