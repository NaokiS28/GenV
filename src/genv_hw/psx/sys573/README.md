# System 573
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