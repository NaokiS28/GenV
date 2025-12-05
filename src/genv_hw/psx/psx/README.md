# Sony PlayStation
The PlayStation core is the base core for all derrived platforms of the PS1.

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