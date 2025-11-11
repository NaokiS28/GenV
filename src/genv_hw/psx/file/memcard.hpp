/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * memcard.hpp - Created on 09-11-2025
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

#include <stdint.h>
#include <stddef.h>

#include "common/util/templates.hpp"

#define DEF(type) static inline type __attribute__((always_inline))
#define structsize(count) (sizeof(*this) + count)

/**
 * @brief Computes the memory card checksum for a given data.
 *
 * @param data Pointer to the block of data
 * @param len Length of the data
 */
uint8_t psx_memcard_xor(void *data, size_t len);

typedef enum : uint16_t
{
    MEMCARD_REGION_JP = "BI"_c,
    MEMCARD_REGION_US = "BA"_c,
    MEMCARD_REGION_EU = "BE"_c
} MemCardRegion;

typedef enum : uint32_t
{
    MEMCARD_SONY_JP      = "SCPS"_c,
    MEMCARD_SONY_US      = "SCUS"_c,
    MEMCARD_SONY_EU      = "SCES"_c,
    MEMCARD_LICENSED_JP  = "SLPS"_c,
    MEMCARD_LICENSED_US  = "SLUS"_c,
    MEMCARD_LICENSED_EU  = "SLES"_c,
    MEMCARD_NOLICENSE_JP = "UGPS"_c,
    MEMCARD_NOLICENSE_US = "UGUS"_c,
    MEMCARD_NOLICENSE_EU = "UGES"_c
} MemCardLicense;

class MemCard_Header_Frame
{
public:
    uint16_t id;
    uint8_t checksum = 0;
    inline uint8_t calcSum() { return psx_memcard_xor(this, structsize(-2)); }
};

class MemCard_Directory_Frame
{
public:
    uint32_t allocState;
    uint32_t filesize;
    uint32_t nextFrame;
    char fileName[21];
    uint8_t checksum;
    inline uint8_t calcSum() { return psx_memcard_xor(this, structsize(-3)); }
};

class MemCard_BadSector_List
{
public:
    uint32_t offset;
    uint8_t checksum;
    inline uint8_t calcSum() { return psx_memcard_xor(this, structsize(-4)); }
};

typedef enum : uint8_t
{
    ICON_HAS_1_FRAME  = 0x11,
    ICON_HAS_2_FRAMES = 0x12,
    ICON_HAS_3_FRAMES = 0x13,
} MemCardIconFlags;

struct MemCard_Title_Frame
{
    uint16_t id;
    MemCardIconFlags iconFlag;
    uint8_t blockNumber;
    char title[64];
    uint8_t pocketStation[32];
    uint16_t iconCLUT[16];
};

struct MemCard_Icon_Frame
{
    uint8_t icon[128];
};

struct MemCard_Data_Frame
{
    uint8_t data[128];
};

/**
 * @brief Formats a string into a standard PS1 memory card
 * format. Use this for the title field within a title frame.
 *
 * @param out Pointer to the char array. Caller must create minimum of 21 byte array
 * @param titleID Game's 5 digit ID - This should avoid any existing game ID
 * @param filename Pointer to the filename - This can and should be different for different save files
 * @param region Game's region as described in MemCardRegion
 * @param license Game's 4 character license string as described in MemCardLicense
 * @param pocketexe Set to true if this file is a PocketStation executable
 */
void psx_memcard_format_filename(
    char **out,
    uint32_t titleID,
    const char *filename,
    MemCardRegion region,
    MemCardLicense license,
    bool pocketexe);