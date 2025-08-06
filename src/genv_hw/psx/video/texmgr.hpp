/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * texman.hpp - Created on 04-08-2025
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

#include "common/util/rect.h"

#include "gpucmd.h"
#include "psxtex.hpp"
#include "gpudef.hpp"

namespace System::PSX::GPU
{
    class TextureManager
    {

    public:
        enum : bool
        {
            TMGR_FREE,
            TMGR_INUSE
        };

        enum : int
        {
            TMGR_OKAY = 0,
            TMGR_MALLOC_FAIL = -1,
            TMGR_INVALID_OBJECT = -2,
            TMGR_OUT_OF_BOUNDS = -3,
            TMGR_COLLISION = -4,
            TMGR_NO_FREE_SPACE = -5,
        };

        struct TexPageEntry
        {
            uint8_t page = 0, tileX = 0, tileY = 0;
        };

        struct VRAMEntry
        {
            uint8_t x = 0, y = 0;
        };

    private:
        // A simple POD to describe one texture page's allocation state:
        struct PageAllocState
        {
            // Bitmaps for tracking free/used tiles and CLUT slots:
            uint32_t tileBitmap[TILES_PER_ROW];
            uint32_t clutBitmap[MAX_CLUT_LINES_PER_PAGE];
        } *_pages;

        uint8_t _vramSize = 0;
        uint8_t _vramPageRows = 0;
        RectWH _frameBufferBox; // In pages.

        // Finds a free space in the texture page where the requested block can go, or returns no space.
        int findFreeBlock(TexPageEntry &v, uint8_t wTiles, uint8_t hTiles) const;

        // Finds a free space in the texture page where the requested CLUT can go, or returns no space.
        // A CLUT can go anywhere within reason in the VRAM, but for the sake of simplicity, for now
        // findFreeCLUT will only return TMGR_OKAY if it can allocate space on the same texture page.
        // X and Y are PIXEL values, not tile values.
        // TODO: Allow CLUT to exist independantly of texture
        int findFreeCLUT(uint8_t page, GP0ColorDepth bitdepth, uint8_t &clutLine, uint8_t &width) const;

        int processLargeBlock(uint8_t state,                    // State to write (ignored on dry run)
                              uint8_t pageStart,               // Starting texture page
                              uint8_t xTile, uint8_t yTile,     // Origin of tile block
                              uint16_t wTiles, uint16_t hTiles, // W/H of tile block
                              bool dryRun);                     // Does not modify anything if true

    public:
        // vram_size in MiB
        TextureManager(uint8_t vram_size)
        {
            _vramSize = vram_size;
            _vramPageRows = (2 * vram_size);
        }

        ~TextureManager()
        {
            if (_pages)
                delete[] _pages;
        }

        // Takes in a VRAM coord and returns a TexPage entry
        inline constexpr uint8_t pxToTile(uint16_t px) const
        {
            return (px % PAGE_PIXELS) / MIN_TILE_SIZE;
        }
        inline constexpr uint8_t tileToPx(uint8_t tile) const
        {
            return MIN_TILE_SIZE * tile;
        }
        inline constexpr uint8_t tileXToPx(uint8_t page, uint8_t tile) const
        {
            return tileToPx(tile) + (PAGE_PIXELS * (page % PAGE_GRID_COLS));
        }
        inline constexpr uint8_t tileYToPx(uint8_t page, uint8_t tile) const
        {
            return tileToPx(tile) + (PAGE_PIXELS * (page / PAGE_GRID_COLS));
        }

        inline constexpr TexPageEntry vramToTPage(VRAMEntry v) const { return vramToTPage(v.x, v.y); }
        inline constexpr TexPageEntry vramToTPage(int width, int yPx) const
        {
            TexPageEntry t;
            t.page = (PAGE_GRID_COLS * (yPx / PAGE_PIXELS)) +
                     (width / PAGE_PIXELS);
            t.tileX = (width % PAGE_PIXELS);
            t.tileY = (yPx % PAGE_PIXELS);
            return t;
        }

        inline constexpr VRAMEntry tpageToVRAM(TexPageEntry t) const { return tpageToVRAM(t.page, t.tileX, t.tileY); }
        inline constexpr VRAMEntry tpageToVRAM(uint8_t page, uint8_t tileX, uint8_t tileY) const
        {
            VRAMEntry v;
            v.x = tileXToPx(page, tileX);
            v.y = tileYToPx(page, tileY);
            return v;
        }

        // Returns VRAM coords for CLUT in a page.
        // Clamps width to align to 16, 32, 48 etc
        // Also clamps yPx to maximum CLUT lines per page.
        inline constexpr VRAMEntry clutToVRAM(uint8_t p, uint8_t width, uint8_t clutLine) const
        {
            VRAMEntry v;
            v.x = (COLORS_4BPP * (width / COLORS_4BPP)) + (PAGE_PIXELS * (p % PAGE_GRID_COLS));
            v.y = (255 - (clutLine % MAX_CLUT_LINES_PER_PAGE)) + (PAGE_PIXELS * (p / PAGE_GRID_COLS));
            return v;
        }

        // Allocates and clears the allocation maps.
        int init();

        // Checks if vram area in XY of page is in use. Checks per tile row for tiles.
        // false if it's free, true if in used
        bool testTile(const uint8_t page, const uint8_t tileX, const uint8_t tileY) const;

        // Checks if vram area in XY of page is in use. Checks per line for CLUTs
        // false if it's free, true if in used
        inline bool testCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t width) const
        {
            uint8_t line = (PAGE_PIXELS - 1) - clutLine;
            return (((_pages[page].tileBitmap[pxToTile(line)] & (1 << pxToTile(width))) != 0) ||
                   (_pages[page].clutBitmap[clutLine] & (1 << (width / 2))) != 0);
        }

        void writeTile(const bool state, const uint8_t page, const uint8_t tileX, const uint8_t tileY);

        // Marks a given vram area in XY of page as in use.
        // No safety check is performed, prefer to use testTile prior or markBlock/markLargeBlock.
        inline void setTile(const uint8_t page, const uint8_t tileX, const uint8_t tileY)
        {
            _pages[page].tileBitmap[tileY] |= (1 << tileX);
        }
        inline void clearTile(const uint8_t page, const uint8_t tileX, const uint8_t tileY)
        {
            _pages[page].tileBitmap[tileY] &= ~(1 << tileX);
        }
        inline void setCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t width)
        {
            _pages[page].clutBitmap[clutLine] |= (1 << (width / 16));
        }
        inline void clearCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t width)
        {
            _pages[page].clutBitmap[clutLine] &= ~(1 << (width / 16));
        }

        // Marks bits within a page as `state`. Constrains to within the page.
        inline int markBlock(
            const bool state,
            const uint8_t page,
            uint8_t xTiles, uint8_t yTiles,
            const uint8_t wTiles = 1, const uint8_t hTiles = 1)
        {
            if ((0xFF - xTiles) - wTiles || (0xFF - yTiles) - hTiles || xTiles > TILES_PER_COL || yTiles > TILES_PER_COL)
            {
                // Block extends beyond page limits
                return -1;
            }
            return markLargeBlock(state, page, xTiles, yTiles, wTiles, hTiles);
        }

        inline void markCLUT(
            const bool state,
            const uint8_t page,
            const uint8_t clutLine,
            uint8_t xPx,
            const uint8_t width
        ){
            for (; xPx < width; xPx += COLORS_4BPP){
                if(state)
                    setCLUT(page, clutLine, xPx);
                else
                    clearCLUT(page, clutLine, xPx);
            }
        }

        int markLargeBlock(
            const bool state,
            uint8_t pageStart,
            uint8_t xTile, uint8_t yTile,
            uint16_t wTiles, uint16_t hTiles);

        // Marks parts of the VRAM as in use for the framebuffer so it won't be wastefully checked during allocation.
        int markFrameBuffer(const uint8_t width, const uint8_t yPx, const uint16_t wPx, const uint16_t hPx);

        // Allocate space in the texture page for a CLUT
        int allocateCLUT(PSXTextureObject *ptObj);

        // Allocate space in the texture page for a texture
        int allocateTexture(PSXTextureObject *ptObj);

        // Deallocates space reserved for a texture and CLUT (if used)
        int deallocateTexture(PSXTextureObject *ptObj);

        // Deallocates ALL VRAM
        void clearAll()
        {
            memset(_pages, 0, sizeof(PageAllocState) * (PAGE_GRID_COLS * _vramPageRows));
        }
    };
}
