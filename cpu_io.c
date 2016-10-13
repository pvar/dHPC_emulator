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
 * @file cpu_io.c
 * @brief Exchange data with sub-systems and peripherals.
 *
 * The functions in this file handle all IO transactions with EEPROM and serial port,
 * decode data received from keyboard and transmit commands or raw data to the VGA
 * controller and / or the sound synthesizer.
 * @note In order to port nstBASIC to some other platform, this file will probably
 * have to be rewritten from scratch!
 *
*/

#include "cpu_io.h"

/** ***************************************************************************
 * @brief Get a new line...
 *
 * see: https://git.gnome.org/browse/gtk+/plain/gdk/gdkkeysyms.h
 *****************************************************************************/
void get_line (void)
{
        text_ptr = prog_end_ptr + sizeof (LINE_NUMBER);
        guchar *maxpos = text_ptr;

        guint temp1, temp2;
        guint in_char;

        while (1) {
                in_char = emu_getchar (STREAM_STD);
                if ((in_char > 31) && (in_char < 127)) {
                        if (text_ptr == variables_ptr - 2)
                                do_beep();
                        else {
                                emu_putchar((guchar) in_char, active_stream);
                                text_ptr[0] = (guchar) in_char;
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
                                                ////g_print("%c", temp1 / CHARS_PER_LINE + 1);
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
                                                temp2 = temp1 / CHARS_PER_LINE - temp2 / CHARS_PER_LINE;
                                                g_print("%c", temp2 + 1);
                                                // calculate line-ending column
                                                temp2 = temp1 - (temp1 / CHARS_PER_LINE) * CHARS_PER_LINE;
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
                                case GDK_KEY_3270_Enter:
                                case GDK_KEY_ISO_Enter:
                                        text_ptr = maxpos;
                                        text_ptr[0] = LF; // LF for newline just like in Unix
                                        newline (active_stream);
                                        return;
                                case GDK_KEY_space:
                                case GDK_KEY_KP_Space:
                                        emu_putchar(SPACE, active_stream);
                                        text_ptr[0] = SPACE;
                                        text_ptr++;
                                        if (text_ptr > maxpos)
                                                maxpos = text_ptr;
                                        break;
                                case GDK_KEY_Tab:
                                case GDK_KEY_KP_Tab:
                                        emu_putchar(TAB, active_stream);
                                        text_ptr[0] = TAB;
                                        text_ptr++;
                                        if (text_ptr > maxpos)
                                                maxpos = text_ptr;
                                        break;
                                case GDK_KEY_BackSpace:
                                        if (text_ptr <= prog_end_ptr + 2)
                                                do_beep();
                                        else {
                                                emu_putchar(BS, active_stream);
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
        guint i;

        /* init keyboard buffer */
        G_LOCK (keyboard_data);
        keyboard_data.wp = 0;
        keyboard_data.cnt = 0;
        G_UNLOCK (keyboard_data);

        /* init struct for communication with GPU */
        G_LOCK (gpu_data);
        gpu_data.type = GPU_PRINT;
        for (i = 0; i < GPU_DATA_LENGTH; i++)
                gpu_data.data[i] = 0;
        gpu_data.received = TRUE;
        gpu_data.new_set = FALSE;
        G_UNLOCK (gpu_data);

        /* init struct for communication with APU */
        G_LOCK (apu_data);
        apu_data.type = APU_STOP;
        for (i = 0; i < APU_DATA_PACKET; i++)
                apu_data.data[i] = 0;
        apu_data.received = TRUE;
        apu_data.new_set = FALSE;
        G_UNLOCK (apu_data);

        active_stream = STREAM_STD;
}

/** ***************************************************************************
 * @brief Put character to selected stream.
 *****************************************************************************/
guint emu_putchar (guchar out_char, guchar stream)
{
        switch (stream) {
                case STREAM_STD:
                        putchr_gpu (out_char);
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
guint emu_getchar (guchar stream)
{
        switch (stream) {
                case STREAM_STD:
                        return getchr_kbd();
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
 * @brief Read character from keyboard buffer
 *****************************************************************************/

guint getchr_kbd (void) {
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
 * @brief Send character to GPU
 *****************************************************************************/

void putchr_gpu (guchar chr)
{
        gboolean tmp;

        /* wait for other threads to stop using gpu_data */
        G_LOCK (gpu_data);
        tmp = gpu_data.received;
        G_UNLOCK (gpu_data);
        while (tmp == FALSE) {
                g_usleep (2000);
                G_LOCK (gpu_data);
                tmp = gpu_data.received;
                G_UNLOCK (gpu_data);
        }

        /* load values to gup_data struct */
        G_LOCK (gpu_data);
        gpu_data.type = GPU_PRINT;
        gpu_data.data[0] = chr;
        gpu_data.received = FALSE; // it will be set to TRUE when read from gpu-thread
        gpu_data.new_set = TRUE;   // it will be set to FALSE when read from gpu-thread
        G_UNLOCK (gpu_data);
}

/** ***************************************************************************
 * @brief Send command to GPU
 *****************************************************************************/

void putcmd_gpu (guchar command, gint length, guchar *data)
{
        gboolean tmp;
        guint i;

        /* sanitize length */
        if (length > GPU_DATA_LENGTH)
                length = GPU_DATA_LENGTH;

        /* wait for other threads to stop using gpu_data */
        G_LOCK (gpu_data);
        tmp = gpu_data.received;
        G_UNLOCK (gpu_data);
        while (tmp == FALSE) {
                g_usleep (2000);
                G_LOCK (gpu_data);
                tmp = gpu_data.received;
                G_UNLOCK (gpu_data);
        }

        /* load values to gup_data struct */
        G_LOCK (gpu_data);
        gpu_data.type = command;
        for (i = 0; i < length; i++)
                gpu_data.data[i] = *(data + i);
        gpu_data.received = FALSE; // it will be set to TRUE when read from gpu-thread
        gpu_data.new_set = TRUE;   // it will be set to FALSE when read from gpu-thread
        G_UNLOCK (gpu_data);
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



/*
ISR (INT2_vect)
{
        // signal program-break
        break_flow = 1;
        // disable emergency break key
        EIMSK &= ~BREAK_INT;
}
*/
