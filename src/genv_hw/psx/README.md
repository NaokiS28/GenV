# Sony PlayStation Core
This folder contains the abstraction interfaces for platforms based on the Sony PlayStation released in 1995. 
The Sony PlayStation (herein reffered to by the names of PS1 or PSX) has the following technical specs:
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

GenV Status:
<table>
	<tr>
		<td> CPU </td>
		<td> CPU COP0 is partially implemented, MDEC and GTE are not implemented. </td>
	</tr>
	<tr>
		<td> GPU </td>
		<td> GPU is partially implemented. DMA support is functional. 3D functions are not abstracted.</td>
	</tr>
	<tr>
		<td> SPU </td>
		<td> SPU is skeleton!</td>
	</tr>
	<tr>
		<td> CD-ROM </td>
		<td> CD-ROM is comletely unimplemented.</td>
	</tr>
	<tr>
		<td> Memory Cards </td>
		<td> Memory cards are partially implemented, but require advancements in the core storage management system.</td>
	</tr>
	<tr>
		<td> IO </td>
		<td> 
			Controllers are partially implemented, but require advancements in the core input management system.<p>
			SIO port is implemented and used as the primary debugging UART.<p>
			PIO based UART is not implemented.<p>
		</td>
	</tr>
</table>

## System 573
The KONAMI System 573 is an arcade board which is heavily based on the PS1, which uses the same base hardware with modifications:
* Doubles the EDO Work RAM to 4MB
* Doubles the GPU SGRAM to 2MB
* Replaces the PS1 2x CD-ROM drive with an IDE header with a IDE CD-ROM drive running in PIO mode.
* Adds 16MB of 16-Bit (arranged in a 8x 8-bit Hi/Lo config) JEDEC Flash in the EXP-1 region
* Adds memory mapped JAMMA input port and 8-Bit output port
* Adds Hitachi H8 MCU to handle JVS IO packet transfers
* Adds 2x memory mapped PCMCIA slots (only supports PCMCIA linear flash without mods)
* Adds a security cart port which has a buffered 8-bit in and out port, configurable GPIO, UART and handshaking flags.

GenV Status:
<table>
	<tr>
		<td> PS1 Core </td>
		<td> CPU & GPU & SPU are identical to retail units. See core PS1 implementation details. </td>
	</tr>
	<tr>
		<td> IDE CD-ROM </td>
		<td> IDE CD-ROM is comletely unimplemented.</td>
	</tr>
	<tr>
		<td> IDE HDD </td>
		<td> IDE HDD support is comletely unimplemented.</td>
	</tr>
	<tr>
		<td> IO </td>
		<td> 
			JAMMA is partially implemented, but require advancements in the core input management system.<p>
			JVS suport is skeleton.<p>
			Security cart is completely unimplemented.<p>
		</td>
	</tr>
</table>

## Notice of Non-Affiliation and Disclaimer
GenV and this core is not in any way affiliated, associated, authorized, endorsed by, or in any way officially connected with the following companies or any of its subsidiaries or its affiliates.
* Sony Computer Entertainment LLC
* Konami Group Corporation

The following names as well as related names, marks, emblems and images are registered trademarks of their respective owners.
* PlayStation, PS2, DualSense, DUALSHOCK, PlayStation Portal, PlayStation Shapes Logo, Play Has No Limits and PlayStation Productions are registered trademarks or trademarks of Sony Interactive Entertainment Inc.
* SONY and the SCE Diamond logo are registered trademarks or trademarks of Sony Group Corporation.
* KONAMI, KONAMI Digital Entertainment Co., Ltd, System 573, DDR, DanceDanceRevolution are registered trademarks of KOANMI Group Coroporation.