/*
 * ps1-bare-metal - (C) 2023 spicyjpeg
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef PSX_SIO_C
#define PSX_SIO_C
#include <stddef.h>
#include "serial.h"
#include "../registers.h"

/* Serial port stdin/stdout */
int sio1_init(int baud)
{
	SIO_CTRL(1) = SIO_CTRL_RESET;

	SIO_MODE(1) = 0 | SIO_MODE_BAUD_DIV1 | SIO_MODE_DATA_8 | SIO_MODE_STOP_1;
	SIO_BAUD(1) = F_CPU / baud;
	SIO_CTRL(1) = 0 | SIO_CTRL_TX_ENABLE | SIO_CTRL_RX_ENABLE | SIO_CTRL_RTS;
	return 0;
}

void sio1_write_byte(char ch)
{
	// The serial interface will buffer but not send any data if the CTS input
	// is not asserted, so we are going to abort if CTS is not set to avoid
	// waiting forever.
	while (
		(SIO_STAT(1) & (SIO_STAT_TX_NOT_FULL | SIO_STAT_CTS)) == SIO_STAT_CTS)
		__asm__ volatile("");

	if (SIO_STAT(1) & SIO_STAT_CTS)
		SIO_DATA(1) = ch;
}

int sio1_read_byte(void)
{
	while (!(SIO_STAT(1) & SIO_STAT_RX_NOT_EMPTY))
		__asm__ volatile("");

	return SIO_DATA(1);
	return 0;
}

int sio1_read(char *data, size_t number)
{
	int bytes;
	for(; number > 0; number--){
		*data = sio1_read_byte();
		data++;
		bytes++;
	}
	return bytes;
}

int sio1_write(const char *data, size_t number)
{
	int bytes;
	for(; number > 0; number--){
		sio1_write_byte(*data);
		data++;
		bytes++;
	}
	return bytes;
}

void sio1_flush()
{
}

#endif