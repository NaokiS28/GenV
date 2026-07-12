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

#pragma once

#include <stdint.h>

#define COP0_GET(reg, output) \
    __asm__ volatile("mfc0 %0, $%1\n" : "=r"(output) : "i"(reg))
#define COP0_SET(reg, input) \
    __asm__ volatile("mtc0 %0, $%1\n" ::"r"(input), "i"(reg))

#define GTE_GET(reg, output) \
    __asm__ volatile("mfc2 %0, $%1\n" : "=r"(output) : "i"(reg))
#define GTE_SET(reg, input) \
    __asm__ volatile("mtc2 %0, $%1\n" ::"r"(input), "i"(reg))

#define GTE_GETC(reg, output) \
    __asm__ volatile("cfc2 %0, $%1\n" : "=r"(output) : "i"(reg))
#define GTE_SETC(reg, input) \
    __asm__ volatile("ctc2 %0, $%1\n" ::"r"(input), "i"(reg))

#define GTE_LOAD(reg, offset, ptr) \
    __asm__ volatile("lwc2 $%0, %1(%2)\n" ::"i"(reg), "i"(offset), "r"(ptr))
#define GTE_STORE(reg, offset, ptr) \
    __asm__ volatile("swc2 $%0, %1(%2)\n" ::"i"(reg), "i"(offset), "r"(ptr) : "memory")

/* Coprocessor 0 */

typedef enum
{
    COP0_BPC      = 3,  // Breakpoint program counter
    COP0_BDA      = 5,  // Breakpoint data address
    COP0_DCIC     = 7,  // Breakpoint control
    COP0_BADVADDR = 8,  // Bad virtual address
    COP0_BDAM     = 9,  // Breakpoint program counter bitmask
    COP0_BPCM     = 11, // Breakpoint data address bitmask
    COP0_SR       = 12, // Status register
    COP0_CAUSE    = 13, // Exception cause
    COP0_EPC      = 14, // Exception program counter
    COP0_PRID     = 15  // Processor identifier
} COP0Register;

typedef enum
{
    COP0_CAUSE_EXC_BITMASK = 31 << 2,
    COP0_CAUSE_EXC_INT     = 0 << 2,  // Interrupt
    COP0_CAUSE_EXC_AdEL    = 4 << 2,  // Load address error
    COP0_CAUSE_EXC_AdES    = 5 << 2,  // Store address error
    COP0_CAUSE_EXC_IBE     = 6 << 2,  // Instruction bus error
    COP0_CAUSE_EXC_DBE     = 7 << 2,  // Data bus error
    COP0_CAUSE_EXC_SYS     = 8 << 2,  // Syscall
    COP0_CAUSE_EXC_BP      = 9 << 2,  // Breakpoint or break instruction
    COP0_CAUSE_EXC_RI      = 10 << 2, // Reserved instruction
    COP0_CAUSE_EXC_CpU     = 11 << 2, // Coprocessor unusable
    COP0_CAUSE_EXC_Ov      = 12 << 2, // Arithmetic overflow
    COP0_CAUSE_Ip0         = 1 << 8,  // IRQ 0 pending (software interrupt)
    COP0_CAUSE_Ip1         = 1 << 9,  // IRQ 1 pending (software interrupt)
    COP0_CAUSE_Ip2         = 1 << 10, // IRQ 2 pending (hardware interrupt)
    COP0_CAUSE_CE_BITMASK  = 3 << 28,
    COP0_CAUSE_BD          = 1 << 30 // Exception occurred in delay slot
} COP0CauseFlag;

typedef enum
{
    COP0_SR_IEc = 1 << 0,  // Current interrupt enable
    COP0_SR_KUc = 1 << 1,  // Current privilege level
    COP0_SR_IEp = 1 << 2,  // Previous interrupt enable
    COP0_SR_KUp = 1 << 3,  // Previous privilege level
    COP0_SR_IEo = 1 << 4,  // Old interrupt enable
    COP0_SR_KUo = 1 << 5,  // Old privilege level
    COP0_SR_Im0 = 1 << 8,  // IRQ mask 0 (software interrupt)
    COP0_SR_Im1 = 1 << 9,  // IRQ mask 1 (software interrupt)
    COP0_SR_Im2 = 1 << 10, // IRQ mask 2 (hardware interrupt)
    COP0_SR_Isc = 1 << 16, // Isolate cache
    COP0_SR_BEV = 1 << 22, // Boot exception vector location
    COP0_SR_CU0 = 1 << 28, // Coprocessor 0 privilege level
    COP0_SR_CU2 = 1 << 30  // Coprocessor 2 enable
} COP0StatusFlag;

#define SETTER(reg, type)                        \
    static inline type cop0_get##reg(void)       \
    {                                            \
        type value;                              \
        COP0_GET(COP0_##reg, value);             \
        return value;                            \
    }                                            \
    static inline void cop0_set##reg(type value) \
    {                                            \
        COP0_SET(COP0_##reg, value);             \
    }

SETTER(BPC, void *)
SETTER(BDA, void *)
SETTER(DCIC, uint32_t)
SETTER(BDAM, uint32_t)
SETTER(BPCM, uint32_t)
SETTER(SR, uint32_t)

#undef SETTER

#define GETTER(reg, type)                  \
    static inline type cop0_get##reg(void) \
    {                                      \
        type value;                        \
        COP0_GET(COP0_##reg, value);       \
        return value;                      \
    }

GETTER(BADVADDR, void *)
GETTER(CAUSE, uint32_t)
GETTER(EPC, void *)

#undef GETTER