/*
 * Peripheral interfacing functions for nstBASIC.
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
 * @file io.c
 * @brief Exchange data with sub-systems and peripherals.
 *
 * The functions in this file handle all IO transactions with EEPROM and serial port,
 * decode data received from keyboard and transmit commands or raw data to the VGA
 * controller and / or the sound synthesizer.
 * @note In order to port nstBASIC to some other platform, this file will probably
 * have to be rewritten from scratch!
 *
*/

#include "io.h"

/** ***************************************************************************
 * @brief Get a new line...
 *
 * see: https://git.gnome.org/browse/gtk+/plain/gdk/gdkkeysyms.h
 *****************************************************************************/
void get_line (void)
{
        text_ptr = prog_end_ptr + sizeof (uint16_t);
        uint8_t *maxpos = text_ptr;

        uint8_t temp1, temp2;
        guint in_char;

        while (1) {
                in_char = emu_getchar (STREAM_STD);
                if ((in_char > 31) && (in_char < 127)) {
                        if (text_ptr == variables_ptr - 2)
                                do_beep();
                        else {
                                g_print("%c", (uint8_t) in_char);
                                text_ptr[0] = (uint8_t) in_char;
                                text_ptr++;
                                if (text_ptr > maxpos)
                                        maxpos = text_ptr;
                        }
                } else {
                        switch (in_char) {
                                case CTRL_G:
                                        do_beep();
                                        break;
                                case CTRL_L:
                                        break;
                                case GDK_KEY_Home:
                                        if (text_ptr > prog_end_ptr + 2) {
                                                ////g_print("%c", vid_tosol);
                                                // calculate lines to move the cursor up
                                                temp1 = text_ptr - prog_end_ptr - 2;
                                                ////g_print("%c", temp1 / MAXCPL + 1);
                                                text_ptr = prog_end_ptr + 2;
                                        }
                                        break;
                                case GDK_KEY_End:
                                        if (text_ptr < maxpos) {
                                                ////g_print("%c", vid_toeol);
                                                // chars to end of line
                                                temp1 = maxpos - prog_end_ptr - 2;
                                                // chars from start of line
                                                temp2 = text_ptr - prog_end_ptr - 2;
                                                // calculate line-ending row
                                                temp2 = temp1 / MAXCPL - temp2 / MAXCPL;
                                                g_print("%c", temp2 + 1);
                                                // calculate line-ending column
                                                temp2 = temp1 - (temp1 / MAXCPL) * MAXCPL;
                                                g_print("%c", temp2);
                                                text_ptr = maxpos;
                                        }
                                        break;
                                case GDK_KEY_Left:
                                case GDK_KEY_KP_Left:
                                        if (text_ptr <= prog_end_ptr + 2)
                                                do_beep();
                                        else {
                                                //// g_print("%c", vid_tolft);
                                                text_ptr--;
                                        }
                                        break;
                                case GDK_KEY_Right:
                                case GDK_KEY_KP_Right:
                                        if (text_ptr < maxpos) {
                                                text_ptr++;
                                                //// g_print("%c", vid_torgt);
                                        } else do_beep();
                                        break;
                                case GDK_KEY_Up:
                                case GDK_KEY_KP_Up:
                                case GDK_KEY_Down:
                                case GDK_KEY_KP_Down:
                                        break;
                                case GDK_KEY_Return:
                                case GDK_KEY_KP_Enter:
                                        text_ptr = maxpos;
                                        text_ptr[0] = LF; // LF for newline just like in Unix
                                        newline (active_stream);
                                        return;
                                case GDK_KEY_BackSpace:
                                        if (text_ptr <= prog_end_ptr + 2)
                                                do_beep();
                                        else {
                                                g_print("%c", BS);
                                                text_ptr--;
                                        }
                                        break;
                        }
                }
        }
}


/** ***************************************************************************
 * @brief Microcontroiller peripheral initialization.
 *
 * This function configures the pins of the CPU (actually the microcontroller).
 * It prepares the ADC, initializes the keyboard and prints some messages.
 *****************************************************************************/
void init_io (void)
{
        G_LOCK (keyboard_data);
        keyboard_data.wp = 0;
        keyboard_data.cnt = 0;
        G_UNLOCK (keyboard_data);

        active_stream = STREAM_STD;
}

/** ***************************************************************************
 * @brief Put character top selected stream.
 *****************************************************************************/
guint emu_putchar (guint out_char, uint8_t stream)
{
        switch (stream) {
                case STREAM_STD:
                        g_print("%c", out_char);
                        break;
                case STREAM_APU:
                        g_print("%c", out_char);
                        break;
                case STREAM_SER:
                        g_print("%c", out_char);
                        break;
                default:
                        return 0;
        }
        return 1;
}

/** ***************************************************************************
 * @brief Get character from selected stream.
 *****************************************************************************/
guint emu_getchar (uint8_t stream)
{
        switch (stream) {
                case STREAM_STD:
                        return get_std();
                case STREAM_APU:
                        break;
                case STREAM_SER:
                        break;
                default:
                        return 0;
        }
        return 1;
}

/** ***************************************************************************
 * @brief Get character from keyboard buffer
 *****************************************************************************/

guint get_std (void) {
        static gint rp;
        gint input_char = -1;
        guint tmp;

        while (input_char == -1) {
                G_LOCK (keyboard_data);
                tmp = keyboard_data.cnt;
                G_UNLOCK (keyboard_data);
                while (tmp == 0) {
                        g_usleep (16000);
                        G_LOCK (keyboard_data);
                        tmp = keyboard_data.cnt;
                        G_UNLOCK (keyboard_data);
                }

                G_LOCK (keyboard_data);
                /* get character from buffer */
                input_char = keyboard_data.buffer[rp];
                G_UNLOCK (keyboard_data);
                /* update read pointer */
                rp++;
                if (rp == KEYBOARD_BUFFER_SIZE)
                        rp = 0;
                /* update character count */
                keyboard_data.cnt--;
        }
        return input_char;
}



























/** ***************************************************************************
 * @brief BEEP or flash the screen.
 *
 * This function makes a sound (beep) or flashes an LED, to inform user about
 * an error (usually when the keyboard buffer is full).
 *****************************************************************************/
void do_beep (void)
{
        // make beep or flash screen
}

/** ***************************************************************************
 * @brief Change on-screen text colour.
 *
 * This function sends to the graphics subsystem the necessary control byte,
 * along with the selected "text" colour.
 *****************************************************************************/
void text_color (uint8_t color)
{
//        g_print("%c", vid_color);
//        g_print("%c", color);
}

/** ***************************************************************************
 * @brief Change on-screen background colour.
 *
 * This function sends to the graphics subsystem the necessary control byte,
 * along with the selected background colour -- paper colour, anyone?
 *****************************************************************************/
void paper_color (uint8_t color)
{
//        g_print("%c", vid_paper);
//        g_print("%c", color);
}

/** ***************************************************************************
 * @brief Move cursor to arbitrary location.
 *
 * This function sends to the graphics subsystem the necessary control byte,
 * along with the new location of the cursor.
 *****************************************************************************/
void locate_cursor (uint8_t line, uint8_t column)
{
//        g_print("%c", vid_locate);
//        g_print("%c", line);
//        g_print("%c", column);
}

/** ***************************************************************************
 * @brief Draw a pixel on the screen.
 *
 * This function sends to the graphics subsystem the necessary control byte,
 * along with data describing the new pixel (position and colour).
 *****************************************************************************/
void put_pixel (uint8_t x, uint8_t y, uint8_t color)
{
//        g_print("%c", vid_pixel);
//        g_print("%c", x);
//        g_print("%c", y);
//        g_print("%c", color);
}



/*
ISR (INT2_vect)
{
        // signal program-break
        break_flow = 1;
        // disable emergency break key
        EIMSK &= ~BREAK_INT;
}
*/
