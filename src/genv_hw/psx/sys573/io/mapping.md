# System 573 IO Mapping

The System 573 has a bunch of memory-mapped I/O devices and whilst embedded design traditionally has a single register file, for GenV, the I/O registers have been split into their relevant driver files so all required aspects are covered in one space.

For the sake of those who want a more high level view of the I/O map (like you would get in a single register file), this document is for you.

## Base I/O (0x1f000000/0xbf000000) Mask: 0xFFFF

The base I/O address of all System 573 peripherals is `DEV0_BASE` from the PS1 common core's `registers.h` file.

KSEG0 `DEV0_BASE` = `0x1f000000`

KSEG1 `DEV0_BASE` = `0xbf000000`

The short register table is as follows:

| Address  | Mapped to:                                                                        |
| -------- | --------------------------------------------------------------------------------- |
| 0x000000 | Flash memory (Onboard 16/32MB or PCMCIA Linear Flash)                             |
| 0x400000 | [Konami I/O ASIC](#konami-io-multiplexer-asic-dev0_base--0x400000--mask-0xffff)   |
| 0x420000 | _Unused_                                                                          |
| 0x440000 | _Unused_                                                                          |
| 0x480000 | [IDE CS0](#ide-port-cs0-dev0_base--0x480000--mask-0xffff)                         |
| 0x4A0000 | _Unused_                                                                          |
| 0x4C0000 | [IDE CS1](#ide-port-cs1-dev0_base--0x4c0000--mask-0xffff)                         |
| 0x4E0000 | _Unused_                                                                          |
| 0x500000 | [Flash Bank Select / GPIO Direction](#flash-bank-dev0_base--0x500000-mask-0x00ff) |
| 0x520000 | [JVS IRDY Clear](#jvs-irdy-acknowledge-dev0_base--0x520000-mask-0x0000)           |
| 0x540000 | _Unused_                                                                          |
| 0x560000 | [IDE Reset Control](#ide-reset-control-dev0_base--0x560000-mask-0x0001)           |
| 0x5A0000 | _Unused_                                                                          |
| 0x5C0000 | [Watchdog Tick](#watchdog-tick-dev0_base--0x5c0000-mask-0x0000)                   |
| 0x5E0000 | _Unused_                                                                          |
| 0x600000 | [External Outputs](#ext-out-port-dev0_base--0x600000-mask-0x00ff)                 |
| 0x620000 | [MT48T58 RTC](#mt48t58-rtc-chip-dev0_base--0x620000-mask-0x00ff)                  |
| 0x640000 | Sys573 Expansion Port                                                             |
| 0x680000 | [JVS Write](#jvs-out-dev0_base--0x680000-mask-0xffff)                             |
| 0x6A0000 | [Security Cart Out](#security-cart-out-dev0_base--0x6a0000-mask-0x00ff)           |
| 0x6C0000 | _Unused_                                                                          |
| 0x6E0000 | _Unused_                                                                          |

## Konami I/O multiplexer ASIC (DEV0_BASE | 0x400000 ) Mask: 0xFFFF

This address maps to the Konami 056879 custom ASIC which selects between 6 16-bit inputs and has an integrated 16-bit latched output port.

The ports are as follows:

| Address | Direction | Mapped to:                                  |
| ------- | --------- | ------------------------------------------- |
| 0x00    | Out       | Output port (ADC, Counters, Muting Control) |
| 0x04    | In        | DIP, JVS Status, Cartridge In               |
| 0x06    | In        | Misc. Inputs                                |
| 0x08    | In        | JAMMA In                                    |
| 0x0A    | In        | JVS In                                      |
| 0x0C    | In        | JAMMA P1 Kick                               |
| 0x0E    | In        | JAMMA P2 Kick                               |

For details on this register, it's best to read through the [psx-spx writeup](https://psx-spx.consoledev.net/konamisystem573/#0x1f40000c-asic-register-6-jamma-controls-external-inputs) on the ASIC's bit mapping.

## IDE Port CS0 (DEV0_BASE | 0x480000 ) Mask: 0xFFFF

This register is directly connected to the IDE port on the 573 motherboard.

## IDE Port CS1 (DEV0_BASE | 0x4C0000 ) Mask: 0xFFFF

This register is directly connected to the IDE port on the 573 motherboard.

## Flash Bank (DEV0_BASE | 0x500000) Mask: 0x00FF

> In `storage/flash.cpp`

This register controls the current window of flash memory will be visible at `DEV0_BASE | 0x3fffff`. This is equivalent to the ROM chip of a PS1 PIO cartridge. It also controls the direciton of the GPIO pin of the Security Cart.

Bit 0 to Bit 5 controls the bank:

| Bank  | Mapped to:                           |
| ----- | ------------------------------------ |
| 0     | Internal flash 1 (chips`31M`, `27M`) |
| 1     | Internal flash 1 (chips`31L`, `27L`) |
| 2     | Internal flash 1 (chips`31J`, `27J`) |
| 3     | Internal flash 1 (chips`31H`, `27H`) |
| 4-15  | N/A                                  |
| 16-31 | PCMCIA Card Slot 1                   |
| 32-47 | PCMCIA Card Slot 2                   |
| 48-63 | N/A                                  |

Bit 6 sets the direction of the GPIO pin:

| State | GPIO Direction                                |
| ----- | :-------------------------------------------- |
| 0     | Input/Hi-Z                                    |
| 1     | Output (GPIO pin uses Cartridge D0 for state) |

As this register is critical to the operation of the flash storage device driver, this register and its current state is captured in the `storage/flash/flash.cpp` file. As a result, the cartridge functions that set the GPIO state are also stored in this file and declared as extern in the cartridge header at `cartridge/cartridge.hpp`

## JVS IRDY Acknowledge (DEV0_BASE | 0x520000) Mask: 0x0000

> This is a virtual register. Writing anything here will always clear the JVS port's IRDY flag

Declared in `jvs/jvs.cpp`, used only by the JVS driver.

## IDE Reset Control (DEV0_BASE | 0x560000) Mask: 0x0001

This register controls a latch which is connected to the IDE port's <span style="text-decoration:overline">RESET</span> line. _Note: This write is not inverted, so you must write bit 0 as high to release the <span style="text-decoration:overline">RESET</span> pin._

## Watchdog Tick (DEV0_BASE | 0x5C0000) Mask: 0x0000

> This is a virtual register. Writing anything here will always tick the watchdog

This register is used to tick the watchdog to prevent it from restarting the system. A word of caution: The watchdog is an analog part and so there's no fixed times in which you must tick by. In general, the longer you wait between ticks, the more sensitive the watchdog will be to delays. As a rough guideline, you should tick at least once a frame, but never longer than 20 frames.

## Ext-Out Port (DEV0_BASE | 0x600000) Mask: 0x00FF

This register is connected to an 8-bit latch which is connected to a port labelled EXT-OUT on the motherboard. It's a TTL output header and must not be directly connected to lamps, but rather through a relay board.

## MT48T58 RTC Chip (DEV0_BASE | 0x620000) Mask: 0x00FF

This register directly connects to the RTC chip of the 573, where the first `0x3FF0` bytes are battery backed SRAM, and the remaining bytes are for the clock control, current time and battery status.

## Expansion Port (DEV0_BASE | 0x640000) Mask: 0xFFFF

Any device(s) connected to the System 573's expansion header will show up at this address. There's a few expansion boards that could be connected to this port:

* [Analog I/O board (GX700-PWB(F))](https://psx-spx.consoledev.net/konamisystem573/#analog-io-board-gx700-pwbf)
* [Digital I/O board (GX894-PWB(B)A)](https://psx-spx.consoledev.net/konamisystem573/#digital-io-board-gx894-pwbba)
* [Alternate analog I/O board (GX700-PWB(K))](https://psx-spx.consoledev.net/konamisystem573/#alternate-analog-io-board-gx700-pwbk)
* [Fishing controller I/O board (GE765-PWB(B)A)](https://psx-spx.consoledev.net/konamisystem573/#fishing-controller-io-board-ge765-pwbba)
* [DDR Karaoke Mix I/O board (GX921-PWB(B))](https://psx-spx.consoledev.net/konamisystem573/#ddr-karaoke-mix-io-board-gx921-pwbb)
* [GunMania I/O board (PWB0000073070)](https://psx-spx.consoledev.net/konamisystem573/#gunmania-io-board-pwb0000073070)
* [Hypothetical debugging board](https://psx-spx.consoledev.net/konamisystem573/#hypothetical-debugging-board)

## JVS Out (DEV0_BASE | 0x680000) Mask: 0xFFFF

This register is used to send JVS data out by going through the H8 MCU to serialise the data. You should check the status of `JVSDRDY` before writing to this port or you will clobber unread data. _Note: The H8 expects 16-bit words, so you must send two JVS bytes for every write. Words are little endian format_

## Security Cart Out (DEV0_BASE | 0x6A0000) Mask: 0x00FF

This register is used to send data out through the 8-bit `OUT` port of the security cart connector. You should check the status of the cart's `DRDY` before writing to this port or you will clobber unread data. _Note: Writing to this port will automatically set the output ready flag._
