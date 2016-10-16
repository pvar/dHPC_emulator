/*
 * Fundamental components of nstBASIC.
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
 * @file thread_cpu.c
 * @brief Functions that would fit in no other file :)
 */

#include "thread_gpu.h"

/** ***************************************************************************
 * @brief
 *****************************************************************************/
gpointer GPU_thread_init (gpointer data)
{
        guint i;

        /* initialize local data */
        cpu_command = 0;
        for (i = 0; i < GPU_DATA_LENGTH; i++)
                cpu_data[i] = 0;

        /* initialize colours and frame buffer */
        init_video();

        /* main loop: get and process incoming data */
        while (1) {
                gboolean tmp;
                /* wait for new set of data*/
                G_LOCK (gpu_data);
                tmp = gpu_data.new_set;
                G_UNLOCK (gpu_data);
                while (tmp == FALSE) {
                        //g_usleep (2000);
                        g_usleep (250);
                        G_LOCK (gpu_data);
                        tmp = gpu_data.new_set;
                        G_UNLOCK (gpu_data);
                }

                /* get data to local variables */
                G_LOCK (gpu_data);
                cpu_command = gpu_data.type;
                for (i = 0; i < GPU_DATA_LENGTH; i++)
                        cpu_data[i] = gpu_data.data[i];
                gpu_data.received = TRUE;
                gpu_data.new_set = FALSE;
                G_UNLOCK (gpu_data);

                /* process received command */
                switch (cpu_command) {
                        case GPU_PRINT:
                                print_character(cpu_data[0]);
                                break;
                        case GPU_CLEAR:
                                clear_buffer();
                                break;
                        case GPU_PAPER:
                                colour_paper = color_converter(cpu_data[0]);
                                break;
                        case GPU_PEN:
                                colour_pen = color_converter(cpu_data[0]);
                                break;
                        case GPU_LOCATE:
                                if (cpu_data[0] < 0)
                                        cursor_y = 0;
                                else if (cpu_data[0] > LINES_PER_FRAME)
                                        cursor_y = LINES_PER_FRAME - 1;
                                else
                                        cursor_y = cpu_data[0];
                                if (cpu_data[1] < 0)
                                        cursor_x = 0;
                                else if (cpu_data[0] > CHARS_PER_LINE)
                                        cursor_x = CHARS_PER_LINE - 1;
                                else
                                        cursor_x = cpu_data[0];
                                break;
                        case GPU_RESET:
                                init_video();
                                break;
                        default:
                                break;
                }
        }

        return NULL;
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

void init_video() {
        colour_pen = color_converter(TEXT_COL_DEFAULT);
        colour_paper = color_converter(BACK_COL_DEFAULT);
        clear_buffer();
        put_logo();
}

/** ***************************************************************************
 * @brief
 *****************************************************************************/

struct rgb_triad color_converter (guchar colour)
{
        struct rgb_triad rgb_colour;
        guchar multiplier;
        guchar correction;

        if (colour & 128)
                multiplier = 2;
        else
                multiplier = 1;

        correction = (colour & 192) >> 5;

        rgb_colour.red =   ((colour & 3)  << 5) * multiplier + correction;
        rgb_colour.green = ((colour & 12) << 3) * multiplier + correction;
        rgb_colour.blue =  ((colour & 48) << 1) * multiplier + correction;
        //g_print("(mul: %i) red: %i, green: %i, blue: %i\n", multiplier, rgb_colour.red, rgb_colour.green, rgb_colour.blue);
        return rgb_colour;
}
