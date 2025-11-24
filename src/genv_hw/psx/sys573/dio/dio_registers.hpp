/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * dio_registers.hpp - Created on 17-08-2025
 * 
 * GenV is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * GenV is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * GenV. If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include "../registers573.hpp"

/* System 573 digital I/O board */

typedef enum {
	SYS573D_CPLD_STAT_INIT = 1 << 12,
	SYS573D_CPLD_STAT_DONE = 1 << 13,
	SYS573D_CPLD_STAT_ID1  = 1 << 14,
	SYS573D_CPLD_STAT_ID2  = 1 << 15
} Sys573DCPLDStatusFlag;

typedef enum {
	SYS573D_CPLD_CTRL_INIT    = 1 << 12,
	SYS573D_CPLD_CTRL_DONE    = 1 << 13,
	SYS573D_CPLD_CTRL_PROGRAM = 1 << 14,
	SYS573D_CPLD_CTRL_UNKNOWN = 1 << 15
} Sys573DCPLDControlFlag;

typedef enum {
	SYS573D_FPGA_MP3_I2C_SDA = 1 << 12,
	SYS573D_FPGA_MP3_I2C_SCL = 1 << 13
} Sys573DFPGAMP3I2CFlag;

typedef enum {
	SYS573D_FPGA_DS_BUS_DS2433 = 1 <<  8,
	SYS573D_FPGA_DS_BUS_DS2401 = 1 << 12
} Sys573DFPGADSBusFlag;

#define SYS573D_FPGA_MAGIC _MMIO16(DEV0_BASE | 0x640080)

#define SYS573D_FPGA_NET_ID _MMIO16(DEV0_BASE | 0x640090)

#define SYS573D_FPGA_MP3_PTR_H     _MMIO16(DEV0_BASE | 0x6400a0)
#define SYS573D_FPGA_MP3_PTR_L     _MMIO16(DEV0_BASE | 0x6400a2)
#define SYS573D_FPGA_MP3_ENDPTR_H  _MMIO16(DEV0_BASE | 0x6400a4)
#define SYS573D_FPGA_MP3_ENDPTR_L  _MMIO16(DEV0_BASE | 0x6400a6)
#define SYS573D_FPGA_MP3_COUNTER   _MMIO16(DEV0_BASE | 0x6400a8)
#define SYS573D_FPGA_MP3_KEY1      _MMIO16(DEV0_BASE | 0x6400a8)
#define SYS573D_FPGA_MP3_FEED_STAT _MMIO16(DEV0_BASE | 0x6400aa)
#define SYS573D_FPGA_MP3_I2C       _MMIO16(DEV0_BASE | 0x6400ac)
#define SYS573D_FPGA_MP3_FEED_CTRL _MMIO16(DEV0_BASE | 0x6400ae)

#define SYS573D_FPGA_DRAM_WRPTR_H _MMIO16(DEV0_BASE | 0x6400b0)
#define SYS573D_FPGA_DRAM_WRPTR_L _MMIO16(DEV0_BASE | 0x6400b2)
#define SYS573D_FPGA_DRAM_DATA    _MMIO16(DEV0_BASE | 0x6400b4)
#define SYS573D_FPGA_DRAM_RDPTR_H _MMIO16(DEV0_BASE | 0x6400b6)
#define SYS573D_FPGA_DRAM_RDPTR_L _MMIO16(DEV0_BASE | 0x6400b8)

#define SYS573D_FPGA_NET_DATA      _MMIO16(DEV0_BASE | 0x6400c0)
#define SYS573D_FPGA_DAC_COUNTER_H _MMIO16(DEV0_BASE | 0x6400ca)
#define SYS573D_FPGA_DAC_COUNTER_L _MMIO16(DEV0_BASE | 0x6400cc)
#define SYS573D_FPGA_DAC_COUNTER_D _MMIO16(DEV0_BASE | 0x6400ce)

#define SYS573D_FPGA_LIGHTS_AH _MMIO16(DEV0_BASE | 0x6400e0)
#define SYS573D_FPGA_LIGHTS_AL _MMIO16(DEV0_BASE | 0x6400e2)
#define SYS573D_FPGA_LIGHTS_BH _MMIO16(DEV0_BASE | 0x6400e4)
#define SYS573D_FPGA_LIGHTS_D  _MMIO16(DEV0_BASE | 0x6400e6)
#define SYS573D_FPGA_INIT      _MMIO16(DEV0_BASE | 0x6400e8)
#define SYS573D_FPGA_MP3_KEY2  _MMIO16(DEV0_BASE | 0x6400ea)
#define SYS573D_FPGA_MP3_KEY3  _MMIO16(DEV0_BASE | 0x6400ec)
#define SYS573D_FPGA_DS_BUS    _MMIO16(DEV0_BASE | 0x6400ee)

#define SYS573D_CPLD_UNK_RESET _MMIO16(DEV0_BASE | 0x6400f4)
#define SYS573D_CPLD_STAT      _MMIO16(DEV0_BASE | 0x6400f6)
#define SYS573D_CPLD_CTRL      _MMIO16(DEV0_BASE | 0x6400f6)
#define SYS573D_CPLD_BITSTREAM _MMIO16(DEV0_BASE | 0x6400f8)
#define SYS573D_CPLD_LIGHTS_CL _MMIO16(DEV0_BASE | 0x6400fa)
#define SYS573D_CPLD_LIGHTS_CH _MMIO16(DEV0_BASE | 0x6400fc)
#define SYS573D_CPLD_LIGHTS_BL _MMIO16(DEV0_BASE | 0x6400fe)