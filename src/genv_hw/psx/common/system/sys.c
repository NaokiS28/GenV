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

#include <stdatomic.h>
#include <stdbool.h>
#include <stdint.h>
#include "sys.h"
#include "psx/common/registers.hpp"

#define BIOS_ENTRY_POINT ((VoidFunction)0xbfc00000)
#define BIOS_API_TABLE ((VoidFunction *)0x80000200)
#define BIOS_BP_VECTOR ((uint32_t *)0x80000040)
#define BIOS_EXC_VECTOR ((uint32_t *)0x80000080)

/* Internal state */

static uint32_t _savedBreakpointVector[4];
static uint32_t _savedExceptionVector[4];
static Thread _mainThread;

ArgFunction interruptHandler = 0;
void *interruptHandlerArg    = 0;

Thread *currentThread = &_mainThread;
Thread *nextThread    = &_mainThread;

/* Exception handler setup */

static inline void _flushCache(void)
{
    // This is the only function that must always run from the BIOS ROM as it
    // temporarily disables main RAM.
    BIOS_API_TABLE[0x44]();
}

void _exceptionVector(void);

void psx_installExceptionHandler(void)
{
    // Clear all pending IRQ flags and prevent the interrupt controller from
    // generating further IRQs.
    IRQ_MASK = 0;
    IRQ_STAT = 0;
    DMA_DPCR = 0;
    DMA_DICR = DMA_DICR_CH_STAT_BITMASK;

    // Disable interrupts and the GTE at the COP0 side.
    cop0_setSR(COP0_SR_CU0);

    // Overwrite the default breakpoint and exception handlers placed into RAM
    // by the BIOS with a function that will jump to our custom handler.
    __builtin_memcpy(_savedBreakpointVector, BIOS_BP_VECTOR, 16);
    __builtin_memcpy(_savedExceptionVector, BIOS_EXC_VECTOR, 16);
    __builtin_memcpy(BIOS_BP_VECTOR, &_exceptionVector, 16);
    __builtin_memcpy(BIOS_EXC_VECTOR, &_exceptionVector, 16);
    _flushCache();

    DMA_DPCR = 0x0bbbbbbb;
    DMA_DICR = DMA_DICR_IRQ_ENABLE;

    // Ensure interrupt masking is set up properly and the GTE is enabled at the
    // COP0 side.
    cop0_setSR(COP0_SR_Im2 | COP0_SR_CU0 | COP0_SR_CU2);
}

void psx_uninstallExceptionHandler(void)
{
    // Disable interrupts at the COP0 side.
    cop0_setSR(COP0_SR_CU0 | COP0_SR_CU2);

    // Clear all pending IRQ flags and prevent the interrupt controller from
    // generating further IRQs.
    IRQ_MASK = 0;
    IRQ_STAT = 0;
    DMA_DPCR = 0;
    DMA_DICR = DMA_DICR_CH_STAT_BITMASK;

    // Restore the original BIOS breakpoint and exception handlers.
    __builtin_memcpy(BIOS_BP_VECTOR, _savedBreakpointVector, 16);
    __builtin_memcpy(BIOS_EXC_VECTOR, _savedExceptionVector, 16);
    _flushCache();
}

void psx_setInterruptHandler(ArgFunction func, void *arg)
{
    psx_disableInterrupts();

    interruptHandler    = func;
    interruptHandlerArg = arg;
    atomic_signal_fence(memory_order_release);
}

void psx_flushCache(void)
{
    bool enable = psx_disableInterrupts();

    _flushCache();
    if (enable)
        psx_enableInterrupts();
}

void psx_softReset(void)
{
    psx_disableInterrupts();
    BIOS_ENTRY_POINT();
    __builtin_unreachable();
}

/* IRQ acknowledgement */

bool psx_testInterrupt(IRQChannel irq, bool ack)
{
    if (irq == IRQ_SIO0)
    {
        // SIO0 ACK is not clock aligned and not double buffered.
        uint16_t a, b;
        a = IRQ_STAT;
        psx_delayMicrosecondsBusy(5);
        b = IRQ_STAT;
        if (a == b && a & (1 << irq))
        {
            if (ack) IRQ_STAT = ~(1 << irq);
            return true;
        }
    }
    else
    {
        if (IRQ_STAT & (1 << irq))
        {
            if (ack) IRQ_STAT = ~(1 << irq);
            return true;
        }
    }

    return false;
}

bool psx_waitForInterrupt(IRQChannel irq, int timeout)
{
    for (; timeout > 0; timeout -= 10)
    {
        if (psx_testInterrupt(irq, true))
            return true;

        if (irq == IRQ_SIO0)
            psx_delayMicrosecondsBusy(5); // psx_ackInt will wait 5 micros.
        else
            psx_delayMicrosecondsBusy(10);
    }

    return false;
}

bool psx_waitForDMATransfer(DMAChannel dma, int timeout)
{
    for (; timeout > 0; timeout -= 10)
    {
        if (!(DMA_CHCR(dma) & DMA_CHCR_ENABLE))
            return true;

        psx_delayMicrosecondsBusy(10);
    }

    return false;
}

/* Thread switching */

void psx_switchThread(Thread *thread)
{
    if (!thread)
        thread = &_mainThread;

    nextThread = thread;
    atomic_signal_fence(memory_order_release);
}