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

namespace PSX
{
    class TextureManager
    {

    public:
        // VRAM layout
        static constexpr int PSX_GPU_MIN_TILE_SIZE = 8;   // Tile: Minimum of 8×8 pixels (GENV)
        static constexpr int PSX_GPU_PAGE_PIXELS = 256;   // 256×256-pixel TPages
        static constexpr int PSX_GPU_PAGE_GRID_COLS = 16; // TPages per row

        // Derived
        static constexpr int PSX_GPU_VRAM_WIDTH = PSX_GPU_PAGE_PIXELS * PSX_GPU_PAGE_GRID_COLS;      // 4096px total width in a row
        static constexpr int PSX_GPU_TILES_PER_COL = PSX_GPU_PAGE_PIXELS / PSX_GPU_MIN_TILE_SIZE;    // How many tiles per tpage column
        static constexpr int PSX_GPU_TILES_PER_ROW = PSX_GPU_TILES_PER_COL;                          // How many tiles per tpage row
        static constexpr int PSX_GPU_TILES_PER_PAGE = PSX_GPU_TILES_PER_COL * PSX_GPU_TILES_PER_COL; // How many tiles per tpage
        static constexpr int PSX_GPU_MAX_CLUT_LINES_PER_PAGE = PSX_GPU_MIN_TILE_SIZE * 6;            // How many lines of CLUTs to use before considering there's too many. As this eats into tile space, this is in multiples of MIN_TILE_SIZE.

        static constexpr uint8_t PSX_GPU_4BPP_COLORS = 16;
        static constexpr uint8_t PSX_GPU_8BPP_COLORS = 255;

    private:
        // A simple POD to describe one texture page's allocation state:
        struct PageAllocState
        {
            // Bitmaps for tracking free/used tiles and CLUT slots:
            uint32_t tileBitmap[PSX_GPU_TILES_PER_ROW];
            uint32_t clutBitmap[PSX_GPU_MAX_CLUT_LINES_PER_PAGE];
        } *_pages;

        uint8_t _vramSize = 0;
        uint8_t _vramTPageRows = 0;

        RectWH _frameBufferBox; // In tpages.

        // Finds a free space in the texture page where the requested block can go, or returns no space.
        int findFreeBlock(uint8_t tpage, uint8_t wTiles, uint8_t hTiles, uint8_t &xTile, uint8_t &yTile) const;
        
        // Finds a free space in the texture page where the requested CLUT can go, or returns no space.
        // A CLUT can go anywhere within reason in the VRAM, but for the sake of simplicity, for now
        // findFreeCLUT will only return TPAGE_OKAY if it can allocate space on the same texture page.
        // TODO: Allow CLUT to exist independantly of texture
        int findFreeCLUT(uint8_t tpage, GP0ColorDepth bitdepth, uint8_t &clutIdx, uint8_t &clutLine) const;

        int processLargeBlock(uint8_t state,                        // State to write (ignored on dry run)
                              uint8_t tpageStart,                   // Starting texture page
                              uint8_t xTile, uint8_t yTile,         // Origin of tile block
                              uint16_t wTiles, uint16_t hTiles,     // W/H of tile block
                              bool dryRun);                         // Does not modify anything if true

    public:
        enum : bool
        {
            TPAGE_FREE,
            TPAGE_INUSE
        };

        enum : uint8_t
        {
            VRAM_1MIB = 1,
            VRAM_2MIB,
            // Was higher amounts even possible?
        };

        enum : int
        {
            TPAGE_OKAY = 0,
            TPAGE_MALLOC_FAIL = -1,
            TPAGE_INVALID_OBJECT = -2,
            TPAGE_OUT_OF_BOUNDS = -3,
            TPAGE_COLLISION = -4,
            TPAGE_NO_FREE_SPACE = -5,
        };

        // vram_size in MiB
        TextureManager(uint8_t vram_size)
        {
            _vramSize = vram_size;
            _vramTPageRows = (2 * vram_size);
        }

        ~TextureManager()
        {
            if (_pages)
                delete[] _pages;
        }

        inline constexpr uint8_t pxToTile(uint8_t px) { return (px + (PSX_GPU_MIN_TILE_SIZE - 1)) / PSX_GPU_MIN_TILE_SIZE; }
        inline constexpr uint8_t tileToPx(uint8_t tile) { return PSX_GPU_MIN_TILE_SIZE * tile; }

        // Allocates and clears the allocation maps.
        int init();

        // Checks if vram area in XY of TPage is in use.
        // false if it's free, true if in used
        inline bool testTile(const uint8_t tpage, const uint8_t xTile, const uint8_t yTile) const
        {
            return ((_pages[tpage].tileBitmap[yTile] & (1 << xTile)) != 0) ||
                   ((_pages[tpage].clutBitmap[yTile] & (1 << (xTile / 2)) != 0));
        }

        // Marks a given vram area in XY of TPage as in use.
        // No safety check is performed, prefer to use testTile prior or markBlock/markLargeBlock.
        inline void setTile(const uint8_t tpage, const uint8_t xTile, const uint8_t yTile)
        {
            _pages[tpage].tileBitmap[yTile] |= (1 << xTile);
            _pages[tpage].clutBitmap[yTile] |= (1 << (xTile / 2));
        }

        // Marks a given vram area in XY of TPage as free.
        inline void clearTile(const uint8_t tpage, const uint8_t xTile, const uint8_t yTile)
        {
            _pages[tpage].tileBitmap[yTile] &= ~(1 << xTile);
            _pages[tpage].clutBitmap[yTile] &= ~(1 << (xTile / 2));
        }

        // Marks bits within a tpage as `state`. Constrains to within the tpage.
        inline int markBlock(
            const bool state,
            const uint8_t tpage,
            uint8_t xTiles, uint8_t yTiles,
            const uint8_t wTiles = 1, const uint8_t hTiles = 1)
        {
            if ((0xFF - xTiles) - wTiles || (0xFF - yTiles) - hTiles || xTiles > PSX_GPU_TILES_PER_COL || yTiles > PSX_GPU_TILES_PER_COL)
            {
                // Block extends beyond tpage limits
                return -1;
            }
            return markLargeBlock(state, tpage, xTiles, yTiles, wTiles, hTiles);
        }

        // In texmgr.hpp, as a private member:
        int processLargeBlock(uint8_t state,
                              uint8_t tpageStart,
                              uint8_t xTile, uint8_t yTile,
                              uint16_t wTiles, uint16_t hTiles,
                              bool doMark);

        // Your existing markLargeBlock becomes:
        int TextureManager::markLargeBlock(const bool state,
                                           uint8_t tpageStart,
                                           uint8_t xTile, uint8_t yTile,
                                           uint16_t wTiles, uint16_t hTiles)
        {
            // Phase 1: just check for collisions
            int r = processLargeBlock(state, tpageStart, xTile, yTile, wTiles, hTiles, false);
            if (r < 0)
                return r;

            // Phase 2: actually set or clear the bits
            return processLargeBlock(state, tpageStart, xTile, yTile, wTiles, hTiles, true);
        }

        // Marks bits within a tpage as `state`. Is not constrained to within the tpage.
        // Use this if marking a large area as in-use or free, i.e. modifying the framebuffer.
        int markLargeBlock(const bool state, uint8_t tpageStart, uint8_t xTile, uint8_t yTile, const uint16_t wTiles, const uint16_t hTiles);

        // Marks parts of the VRAM as in use for the framebuffer so it won't be wastefully checked during allocation.
        int markFrameBuffer(const uint8_t xPx, const uint8_t yPx, const uint16_t wPx, const uint16_t hPx);

        // Allocate space in the texture page for a CLUT
        int allocateCLUT(PSXTextureObject *ptObj);

        // Allocate space in the texture page for a texture
        int allocateTexture(PSXTextureObject *ptObj);

        // Deallocates space reserved for a texture and CLUT (if used)
        int deallocateTexture(PSXTextureObject *ptObj);
    };
}
