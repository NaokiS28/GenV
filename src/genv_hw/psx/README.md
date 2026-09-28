# Sony PlayStation Based Cores

This folder contains the abstraction interfaces for platforms based on the Sony PlayStation released in 1995.
The Sony PlayStation (herein reffered to by the names of PS1 or PS1 in subsequent documents) has the following technical specs:

* CPU: MIPS R3000A-compatible 32-bit RISC CPU MIPS R3051 with 5 KB L1 cache @ 33.8688 MHz
  * 2MB EDO Work RAM
  * Co-Processors:
  * Geometry Transformation Engine (GTE) - Performs high-speed matrix multiplications
  * Motion Decoder (MDEC) - Image and Video decoder
  * System Control Processor (COP0) - Controls memory management, system interrupts, exception handling, and breakpoints.
* GPU: Custom 32-Bit Sony GPU
  * 1MB SGRAM VRAM
  * 2KB Texture Cache
  * 64 byte FIFO buffer
  * Resolutions:
    * Progressive: 256×224 to 640×240 pixels
    * Interlaced: 256×448 to 640×480 pixels
  * Colors:
    * Maximum color depth of 16,777,216 colors (24-bit true color)
    * 57,344 (256×224) to 153,600 (640×240) colors on screen
    * Unlimited color lookup tables (CLUTs)
    * Mode 4: 4-bit CLUT (16 colors)
    * Mode 8: 8-bit CLUT (256 colors)
    * Mode 15: 15-bit direct (32,768 colors)
    * Mode 24: 24-bit (16,777,216 colors)
  * 1024×512 framebuffer, 8×8 and 16×16 sprite sizes
  * 256×256 maximum sprite size
* SPU: 16-Bit Sony designed SPU:
  * Supports ADPCM sources with up to 24 channels
  * Sampling rate of up to 44.1 kHz
  * 512KB RAM
* Storage:
  * CD-ROM drive:
    * 660 MB maximum storage capacity
    * 2×, with a maximum data throughput of 300 KB/s (double speed), 150 KB/s (normal)
    * 32 KB data buffer
    * XA Mode 2 compliant
    * CD-DA (CD-Digital Audio)
  * 2x Sony PlayStation Memory cards
    * SPI like serial interface, running at 250Kbps
    * 128KB storage, divided into 16 pages, 15 can be used by game saves
    * Custom PS1 Memory Card file system

## Platforms supported

* [PlayStation Hardware Core](common/) - Base driver.
* [Sony PlayStation](psx/) - Retail PS1 driver. Differences from the common core is wiring up the 2x CD-ROM drive and the SPU.
* [KONAMI System 573](sys573/) - Derrived from the common core. Differences include adding an IDE CD-ROM (optionally ATA devices), JVS, JAMMA and Analog IO support.
* [KONAMI GV999](gv999/) - Derrived from the common core. Differences include adding an SCSI CD-ROM (optionally a HDD), JAMMA and Intel Flash.

## Notice of Non-Affiliation and Disclaimer

GenV and this core is not in any way affiliated, associated, authorized, endorsed by, or in any way officially connected with the following companies or any of its subsidiaries or its affiliates.

* Sony Computer Entertainment LLC
* Konami Group Corporation

The following names as well as related names, marks, emblems and images are registered trademarks of their respective owners.

* PlayStation, PS2, DualSense, DUALSHOCK, PlayStation Portal, PlayStation Shapes Logo, Play Has No Limits and PlayStation Productions are registered trademarks or trademarks of Sony Interactive Entertainment Inc.
* SONY and the SCE Diamond logo are registered trademarks or trademarks of Sony Group Corporation.
* KONAMI, KONAMI Digital Entertainment Co., Ltd, System 573, DDR, DanceDanceRevolution are registered trademarks of KOANMI Group Coroporation.
