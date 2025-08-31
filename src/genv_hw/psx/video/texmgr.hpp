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

#include <string.h>
#include <stdint.h>

#include "common/util/rect.h"
#include "common/logger/log.hpp"

#include "gpucmd.h"
#include "psxtex.hpp"
#include "gpudef.hpp"

namespace System::PSX::GPU
{
    // VRAM layout
    static constexpr int VRAM_WIDTH = 1024;   // Total width of a VRAM row in px
    static constexpr int MIN_TILE_SIZE = 8;   // 1 Tile: Minimum of 8×8 pixels in 4BPP (GENV), equates to 4px 8 BPP, 2px 16 BPP, 1px 24 BPP
    static constexpr int PAGE_SIZE = 256;     // 256x256 texture page size for W/H
    static constexpr int PAGE_GRID_COLS = 16; // Texture pages per row

    // Derived
    static constexpr int TILES_PER_ROW = PAGE_SIZE / MIN_TILE_SIZE; // How many tiles per page row
    static constexpr int TILES_PER_COL = PAGE_SIZE / MIN_TILE_SIZE; // How many tiles per page column
    static constexpr int MAX_CLUT_LINES_IN_TILES = 5;               // How many lines of CLUTs to use before considering there's too many. As this eats into tile space, this is in multiples of MIN_TILE_SIZE.
    static constexpr int MAX_CLUT_LINES_PER_PAGE = MIN_TILE_SIZE * MAX_CLUT_LINES_IN_TILES;

    // How many tiles per page column
    static constexpr const int PIXELS_PER_TILE(const uint8_t bpp)
    {
        // Switch to enforce 4/8/16/24 bpp results
        switch (bpp)
        {
        case 4: return 8;
        case 8: return 4;
        case 16: return 2;
        case 24: return 1;
        default: return 0;
        }
    }

    static constexpr unsigned int MAX_COLORS_4BPP = (1 << 4);
    static constexpr unsigned int MAX_COLORS_8BPP = (1 << 8);
    static constexpr unsigned int MAX_COLORS_16BPP = (1 << 16);
    static constexpr unsigned int MAX_COLORS_24BPP = (1 << 24);

    /*
    Texture pages in TextureManager

    In TextureManager, the term pages is not strictly related to the PS1's texture pages. They are
    similar but used in slightly different contexts.

    In the PS1 GPU, a texture page is a window into the VRAM, where the texture page sets the offset
    of the window from 0-31 (0-63 in 2MiB systems). Depending on the current colour bitwidth, this window
    can either encompass 1 to 6 texture pages, which is 256 x 4BPP pixels or 128~byte divisions of VRAM.
    The window will always be 256 pixels by 256px, but the interpretation of the bytes will change depending
    on bitdepth, from 2 pixels per byte, to 1 pixel per 6 bytes.

    It's a nuts system compared to just using 16-bit x and y for vram. That and the fact there's no model storage
    or geometry transformation hardward inside of it. You got to wonder whether or not folks at Toshiba
    faced a hardware limitation or they just got too drunk on Sake when designing the GPU.

    In TextureManager, pages are based on a similar principle, but only offer an insight into a single
    "128 byte" region. Each page has 32 32-bit bitmaps where each bit of a word represents 8 pixels (1 tile).
    Each page also has 5 lines worth of 32-bit CLUT table bitmaps, where here each word also represents 1 tile.
    CLUT bitmaps differ to Tile bitmaps as Tile bitmaps will assume a 8x8 pixel area is allocated to a texture,
    where as a CLUT bitmap assumes a 16x1 pixel area is allocated. CLUTs are either 16px or 256px wide.

    The frame buffer is treated as Tiles and are marked as in use, however to save on allocation time, the
    frame buffer has it's texture page boundries marked out explicitly so that the TextureManager will not
    attempt to search in those texture pages and decrement the page until having to move to the next line.

    Tiles are assigned Top-Left to Bottom-Right, CLUTs are assigned Bottom-Left to Top-Right.
    If there is no more room (collides with existing allocations), the page index will decrement and start over.
    If the page index would encroch the frame buffer, then the index is set to the next texture page line and
    move from right to left again. If no space is available for the texture, the allocation will fail with
    TMGR_NO_FREE_SPACE.

    When looking for an allocation of a texture, TextureManager will look at both the Tile bitmap and the CLUT
    bitmap, and fail if either are marked as allocated for that tile/line.

    */

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
            TMGR_MALLOC_FAIL,
            TMGR_INVALID_OBJECT,
            TMGR_INVALID_BPP,
            TMGR_OUT_OF_BOUNDS,
            TMGR_COLLISION,
            TMGR_NO_FREE_SPACE,
        };

        struct TexPageEntry
        {
            uint8_t page = 0, tileX = 0, tileY = 0;
        };

        struct VRAMEntry
        {
            uint16_t x = 0, y = 0;
        };

    private:
        // A simple POD to describe one texture page's allocation state:
        struct PageAllocState
        {
            // Bitmaps for tracking free/used tiles and CLUT slots:
            uint32_t tileBitmap[TILES_PER_ROW];
            uint32_t clutBitmap[MAX_CLUT_LINES_PER_PAGE];
        } *_pages;

        uint8_t _vramSize = 0;     // VRAM size in MiB
        uint8_t _vramPageRows = 0; // How many VRAM page rows
        RectWH _frameBufferBox;    // In pages (i.e. X/Y = Page 0, w5xh2 pages).

        // Finds a free space in the texture page where the requested block can go, or returns no space.
        int findFreeBlock(TexPageEntry &v, uint8_t bpp, uint8_t wTiles, uint8_t hTiles) const;

        // Finds a free space in the texture page where the requested CLUT can go, or returns no space.
        // A CLUT can go anywhere within reason in the VRAM, but for the sake of simplicity, for now
        // findFreeCLUT will only return TMGR_OKAY if it can allocate space on the same texture page.
        // X and Y are PIXEL values, not tile values.
        // TODO: Allow CLUT to exist independantly of texture
        int findFreeCLUT(uint8_t page, uint8_t bpp, uint8_t &clutLine, uint8_t &width) const;

        int processLargeBlock(
            uint8_t state,                    // State to write (ignored on dry run)
            uint8_t pageStart,                // Starting texture page
            uint8_t xTile, uint8_t yTile,     // Origin of tile block
            uint16_t wTiles, uint16_t hTiles, // W/H of tile block
            bool dryRun);                     // Does not modify anything if true

    public:
        TextureManager(uint8_t vram_size) // vram_size in MiB
        {
            _vramSize = vram_size;
            _vramPageRows = (2 * vram_size);
            _pages = new PageAllocState[16 * _vramPageRows];
            if (_pages)
                memset(_pages, 0, (sizeof(PageAllocState) * (16 * _vramPageRows)));
            else
                LOG("TextureManager", "Unexpected error in allocating _pages bitmap. Dynamic allocation will likely crash!");
        }

        ~TextureManager()
        {
            if (_pages) // Should always be valid, but for safety.
                delete[] _pages;
        }

        // Aligns a pixel to a tile index
        static inline constexpr uint8_t pxToTile(uint16_t px)
        {
            return ((px % PAGE_SIZE) / MIN_TILE_SIZE);
        }
        // Gets the starting pixel of a tile
        static inline constexpr uint16_t tileToPx(uint8_t tile)
        {
            return (MIN_TILE_SIZE * tile);
        }

        static inline constexpr TexPageEntry vramToTPage(VRAMEntry v) { return vramToTPage(v.x, v.y); }
        static inline constexpr TexPageEntry vramToTPage(uint16_t xPx, uint16_t yPx)
        {
            TexPageEntry t;
            t.page = (PAGE_GRID_COLS * (yPx / PAGE_SIZE)) +
                     (xPx / TILES_PER_COL);
            t.tileX = (xPx % TILES_PER_COL);
            t.tileY = (yPx % TILES_PER_COL);
            return t;
        }

        static inline constexpr VRAMEntry tpageToVRAM(TexPageEntry t) { return tpageToVRAM(t.page, t.tileX, t.tileY); }
        static inline constexpr VRAMEntry tpageToVRAM(uint8_t page, uint8_t tileX, uint8_t tileY)
        {
            VRAMEntry v;
            v.y = (PAGE_SIZE * (page / PAGE_GRID_COLS)) + tileToPx(tileY);
            v.x = (MIN_TILE_SIZE * (TILES_PER_COL * (page % PAGE_GRID_COLS))) + tileToPx(tileX);
            return v;
        }

        // Returns VRAM coords for CLUT in a page.
        // Clamps width to align to 16, 32, 48 etc
        // Also clamps yPx to maximum CLUT lines per page.
        static inline constexpr VRAMEntry clutToVRAM(uint8_t page, uint8_t bpp, uint8_t clutLine)
        {
            VRAMEntry v;
            v.x = (MAX_COLORS_4BPP * (bpp / MAX_COLORS_4BPP)) + (PAGE_SIZE * (page % PAGE_GRID_COLS));
            v.y = ((PAGE_SIZE - 1) - (clutLine % MAX_CLUT_LINES_PER_PAGE)) + (PAGE_SIZE * (page / PAGE_GRID_COLS));
            return v;
        }

        static inline constexpr uint16_t clutWidth(uint8_t bpp)
        {
            return (bpp == Textures::BPP_8BIT ? MAX_COLORS_8BPP : MAX_COLORS_4BPP);
        }

        // Creates and clears the allocation maps.
        int init();

        // Checks if vram area in XY of page is in use. Checks per tile row for tiles.
        // false if it's free, true if in used
        bool testTile(const uint8_t page, const uint8_t tileX, const uint8_t tileY) const;

        // Checks if vram area in XY of page is in use. Checks per line for CLUTs
        // false if it's free, true if in used
        inline bool testCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t bpp) const
        {
            uint8_t line = (PAGE_SIZE - 1) - clutLine;
            return (((_pages[page].tileBitmap[pxToTile(line)] & (1 << pxToTile(bpp))) != 0) ||
                    (_pages[page].clutBitmap[clutLine] & (1 << (bpp / 2))) != 0);
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
        inline void setCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t bpp)
        {
            _pages[page].clutBitmap[clutLine] |= (1 << (bpp / 16));
        }
        inline void clearCLUT(const uint8_t page, const uint8_t clutLine, const uint8_t bpp)
        {
            _pages[page].clutBitmap[clutLine] &= ~(1 << (bpp / 16));
        }

        // Marks bits within a page as `state`. Constrains to within the page.
        inline int markBlock(
            const bool state,
            const uint8_t page,
            uint8_t xTiles, uint8_t yTiles,
            const uint8_t wTiles = 1, const uint8_t hTiles = 1)
        {
            if (tileToPx(xTiles + wTiles) >= PAGE_SIZE ||
                tileToPx(yTiles + hTiles) >= PAGE_SIZE)
            {
                // Block extends beyond page + x/y limits, as x/y are 8-bit during render
                return -1;
            }
            return markLargeBlock(state, page, xTiles, yTiles, wTiles, hTiles);
        }

        inline void markCLUT(
            const bool state,
            const uint8_t bpp,
            const uint8_t page,
            const uint8_t clutLine,
            uint8_t xPx)
        {
            uint16_t depthWidth = (bpp == Textures::BPP_8BIT ? MAX_COLORS_8BPP : MAX_COLORS_4BPP);
            for (; xPx < depthWidth; xPx += MAX_COLORS_4BPP)
            {
                if (state)
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
        // Only supports 16bpp framebuffers
        int markFrameBuffer(const uint8_t xPx, const uint8_t yPx, const uint16_t wPx, const uint16_t hPx);

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
} // namespace System::PSX::GPU
