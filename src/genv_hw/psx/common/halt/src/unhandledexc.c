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
 *
 * If any exception other than an IRQ or syscall (such as a bus or alignment
 * error) occurs, the exception handler defined in system.s will call
 * _unhandledException() to safely halt the program. This is a very simple
 * implementation of it that prints the state of all registers (without using
 * printf() to avoid bloating the binary) then locks up.
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "ps1/sys.h"

#include "halt.h"

// True only if a word read at 'a' is safe: aligned and inside real RAM.
static int _isReadableWord(uint32_t a)
{
    if (a & 3) return 0;                             // must be 4-byte aligned
    if ((a & 0x1fffffff) < 0x00200000) return 1;     // 2 MB main RAM, any segment/mirror
    if (a >= 0x1f800000 && a < 0x1f800400) return 1; // scratchpad (optional, rare as a stack)
    return 0;
}

// #ifndef NDEBUG
static const char *const _causeNames[] = {
    "Load address error",
    "Store address error",
    "Instruction bus error",
    "Data bus error",
    "Syscall",
    "Break instruction",
    "Reserved instruction",
    "Coprocessor unusable",
    "Arithmetic overflow"};

static const char _registerNames[] = {
    "pc"
    "at"
    "v0"
    "v1"
    "a0"
    "a1"
    "a2"
    "a3"
    "t0"
    "t1"
    "t2"
    "t3"
    "t4"
    "t5"
    "t6"
    "t7"
    "s0"
    "s1"
    "s2"
    "s3"
    "s4"
    "s5"
    "s6"
    "s7"
    "t8"
    "t9"
    "gp"
    "sp"
    "fp"
    "ra"
    "hi"
    "lo"};

static const char _hexDigits[] = "0123456789ABCDEF";

typedef struct ExceptionString
{
    char *str;
    int size;
    int idx;
} ExceptionString;

static int _printString(ExceptionString *dest, const char *src)
{
    int avail = (dest->size - dest->idx) - 1;
    if (!avail) return 0;

    int inSize = strlen(src);
    int total  = (avail > inSize ? inSize : avail);
    strncat(dest->str, src, total);
    dest->idx += total;
    return dest->idx;
}

static int _printStringN(ExceptionString *dest, const char *src, int count)
{
    int avail = (dest->size - dest->idx) - 1;
    if (!avail) return 0;

    int total = (avail > count ? count : avail);
    strncat(dest->str, src, total);
    dest->idx += total;
    return dest->idx;
}

// Must supply hex_str with an array of atleast 9 bytes.
static int _printHexValue(ExceptionString *dest, uint32_t value)
{
    char str[9] = "";
    for (int i = 8; i; i--, value <<= 4)
        str[8 - i] = _hexDigits[value >> 28];

    return _printString(dest, str);
}

static ExceptionString t_detail = {
    .str  = genv_halt_text_detail,
    .size = GENV_HALT_TEXT_DETAIL_LEN};

static ExceptionString t_title = {
    .str  = genv_halt_text_title,
    .size = GENV_HALT_TEXT_TITLE_LEN};

static ExceptionString t_short = {
    .str  = genv_halt_text_short,
    .size = GENV_HALT_TEXT_SHORT_LEN};

static volatile int _in_handler = 0;
static volatile int _last_cause = 0;

void _genv_halt_screen();
void genv_halt_unhandled_exception(int cause, uint32_t badv)
{
    if (_in_handler)
    {
        // We faulted while already handling a crash. Don't re-run the dump -
        // it would append onto the half-built buffers and may fault again.
        // Stamp a marker and go straight to the screen with what we have.
        t_title.str[0] = '\0';
        t_title.idx    = 0;
        t_short.str[0] = '\0';
        t_short.idx    = 0;
        _printString(&t_title, "EXCEPTION: DOUBLE FAULT");
        _printString(&t_short, "Halt screen (or extension) faulted whilst handling a ");
        _printString(&t_short, _causeNames[cause - 4]);
        _printString(&t_short, " exception.");
        _genv_halt_screen(); // noreturn
    }
    _in_handler = 1;

    _printString(&t_title, "EXCEPTION: ");
    _last_cause = cause;
    if (cause >= 4 && cause <= 12)
        _printString(&t_title, _causeNames[cause - 4]);
    else
        _printString(&t_title, "Implausible cause");
    _printString(&t_title, "\r\n");

    if (cause <= 5)
    {
        _printString(&t_short, "AD: ");
        _printHexValue(&t_short, badv);
        _printString(&t_short, "\t ");

        _printString(&t_detail, "@ ");
        _printHexValue(&t_detail, badv);
        _printString(&t_detail, "\r\n");
    }

    _printString(&t_short, "PC:");
    _printHexValue(&t_short, currentThread->pc);
    _printString(&t_short, " \tSP:");
    _printHexValue(&t_short, currentThread->sp);

    // Detailed list
    _printString(&t_detail, "Register dump:\r\n");

    const char *name = _registerNames;
    uint32_t *reg    = (uint32_t *)&(currentThread->pc);

    for (int i = 31; i >= 0; i--)
    {
        _printString(&t_detail, "\t");
        _printStringN(&t_detail, name, 2);
        _printString(&t_detail, "=");
        _printHexValue(&t_detail, *(reg++));

        name += 2;

        if (!(i % 4))
        {
            _printString(&t_detail, "\r\n");
        }
    }

    _printString(&t_detail, "Stack dump:\r\n");

    uint32_t sp    = currentThread->sp & ~3u; // align, so a bad SP can't misalign the loads
    uint32_t *addr = (uint32_t *)sp - 5;
    uint32_t *end  = (uint32_t *)sp + 5;

    for (; addr <= end; addr++)
    {
        _printString(&t_detail, ((uint32_t)addr == sp) ? ">" : " ");
        _printString(&t_detail, "\t");
        _printHexValue(&t_detail, (uint32_t)addr);
        _printString(&t_detail, " \t");

        if (_isReadableWord((uint32_t)addr))
            _printHexValue(&t_detail, *addr); // verified in RAM, won't fault
        else
            _printString(&t_detail, "--------"); // out of range / would bus-error
        _printString(&t_detail, "\r\n");
    }

    _genv_halt_screen();
}