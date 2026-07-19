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
		<td> Please check the <a href="../common/README.md">hardware core README</a> to see for implementation status. </td>
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
		<td> Flash </td>
		<td> Internal 16MiB flash support is comletely unimplemented.</td>
	</tr>
	<tr>
		<td> PCMCIA </td>
		<td> 
			<p>Warning: PCMCIA IO cards cannot be used without modification to the motherboard. Only memory mapped cards can be used.
			<p>PCMCIA support is comletely unimplemented.
		</td>
	</tr>
	<tr>
		<td> IO </td>
		<td> 
			<p>JAMMA is implemented for both players and arcade service panel.
			<p>Analog inputs on GX700 PCB supported (these appear under player 1, player 2 has only digital).
			<p>ADC083X driver supports 1, 2, 4 and 8 channel variants, but only integrated 4 channel ADC is currently supported.
			<p>JVS support is skeleton.
			<p>Security cart is completely unimplemented.
			<p>Expansion bus is completely unimplemented.
		</td>
	</tr>
</table>