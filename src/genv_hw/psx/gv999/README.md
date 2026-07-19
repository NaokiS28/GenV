# Konami GV999

The KONAMI GV999 (otherwise called GV System or Baby Phoenix) is an arcade board which is heavily based on the PS1, which uses the same base hardware with modifications:

* ZV variants use a V1 PS1 GPU - This is known to have different commands to the normal V2 GPU.
* Replaces the PS1 2x CD-ROM drive with an SCSI header with a SCSI CD-ROM drive controlled byy a dedicated Fast SCSI controller.
* Adds memory mapped JAMMA input port and SPI EEPROM.
* External headers support players 3 and 4.
* Intel 28F400 Flash fitted to some motherboards.

GenV Status:

<table>
	<tr>
		<td> PS1 Core </td>
		<td> Please check the <a href="../common/README.md">hardware core README</a> to see for implementation status. </td>
	</tr>
	<tr>
		<td> SCSI CD-ROM </td>
		<td> SCSI CD-ROM (or HDD) support is comletely unimplemented.</td>
	</tr>
<tr>
		<td> NVRAM </td>
		<td> NVRAM (EEPROM) support is comletely unimplemented.</td>
	</tr>
<tr>
		<td> Flash </td>
		<td> Flash support is comletely unimplemented.</td>
	</tr>
	<tr>
		<td> IO </td>
		<td> 
			<p>JAMMA is implemented for all players and arcade service panel.
		</td>
	</tr>
</table>
