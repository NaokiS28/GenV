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
#include "psx/common/halt/src/halt_ext.h"
#include "psx/common/halt/src/ps1/registers.h"
#include "psx/common/halt/src/ps1/sys.h"

extern HaltColor ColorBlue;

void sysgv_halt_delay(HaltScreenFont *font, uint16_t x, uint16_t y, uint16_t w, uint16_t h)
{
    // Enable PIO/573 read/writing with delay slots. These are based on Konami's values
    // This needs to be done first else the RTC is inacessible
    // BIU_DEV0_ADDR = 0x1F000000;
    // BIU_DEV0_CTRL = 0x24173f47;
    // sysgv_WATCHDOG = 0;

    size_t timer    = 60 * 15; // 60Hz
    int lastSeconds = 0;

    puts("Rebooting in...");
    while (timer)
    {
        if (psx_waitForInterrupt(IRQ_VSYNC, 100))
        {
            int seconds = (timer / 60);

            if (lastSeconds != seconds)
            {
                lastSeconds  = seconds;
                char str[32] = {0};
                snprintf(str, 32, "Rebooting in ... %d second%c", seconds, (seconds > 1 ? 's' : ' '));
                genv_gpu_rectangle(
                    ColorBlue,
                    x,
                    (y - font->fontSize),
                    320 - 5,
                    font->fontSize);
                genv_gpu_drawText(
                    font,
                    str,
                    x, (y - font->fontSize),
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
    }

    puts("\r\nRebooting now!\r\n");
    genv_gpu_rectangle(
        ColorBlue,
        x,
        (y - font->fontSize),
        320 - 5,
        font->fontSize);
    genv_gpu_drawText(
        font,
        "Rebooting now!",
        x, (y - font->fontSize),
        200, 20);

    if (sysgv_extenstion.cb_exit)
        sysgv_extenstion.cb_exit();
}

void sysgv_tick_watchdog(void)
{
    *_ADDR16(DEV0_BASE | 0x780000) = 0;
}

HSDriver sysgv_watchdog_driver = {
    .id            = "GVWDT",
    .update_driver = &sysgv_tick_watchdog};

HSExtension sysgv_extenstion = {
    .show_halt = sysgv_halt_delay,
};