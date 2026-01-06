/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * halt.c - Created on 04-12-2025
 *
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stddef.h>
#include <stdio.h>

#include "halt.h"
#include "psx/common/halt/halt.h"
#include "psx/common/registers.hpp"
#include "psx/common/system/sys.h"
#include "psx/sys573/registers573.hpp"

extern HaltColor ColorBlue;

#define SYS573_HALT_X_POS 5
#define SYS573_HALT_Y_POS (240 - 5)

void sys573_halt_delay(HaltScreenFont *font)
{
    // Enable PIO/573 read/writing with delay slots. These are based on Konami's values
    // This needs to be done first else the RTC is inacessible
    BIU_DEV0_ADDR = 0x1F000000;
    BIU_DEV0_CTRL = 0x24173f47;
    SYS573_WATCHDOG = 0;

    size_t timer = 60 * 15; // 60Hz
    int lastSeconds = 0;

    puts("Rebooting in...");
    while (timer)
    {
        if (psx_waitForInterrupt(IRQ_VSYNC, 100))
        {
            int seconds = (timer / 60);

            if (lastSeconds != seconds)
            {
                lastSeconds = seconds;
                char str[32] = {0};
                snprintf(str, 32, "Rebooting in ... %d second%c", seconds, (seconds > 1 ? 's' : ' '));
                psx_gpu_rectangle(
                    ColorBlue,
                    SYS573_HALT_X_POS,
                    (SYS573_HALT_Y_POS - font->fontSize),
                    320 - 5,
                    font->fontSize);
                psx_gpu_drawText(
                    font,
                    str,
                    SYS573_HALT_X_POS, (SYS573_HALT_Y_POS - font->fontSize),
                    200, 20);

                if (seconds > 9)
                {
                    putchar('1');
                    putchar('0' + (seconds % 10));
                }
                else
                {
                    putchar('0' + seconds);
                }
                putchar(' ');
            }

            timer--;
        }

        SYS573_WATCHDOG = 0;
    }

    puts("\r\nRebooting now!\r\n");
    psx_gpu_rectangle(
        ColorBlue,
        SYS573_HALT_X_POS,
        (SYS573_HALT_Y_POS - font->fontSize),
        320 - 5,
        font->fontSize);
    psx_gpu_drawText(
        font,
        "Rebooting now!",
        SYS573_HALT_X_POS, (SYS573_HALT_Y_POS - font->fontSize),
        200, 20);
}