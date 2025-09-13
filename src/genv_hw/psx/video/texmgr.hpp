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

    /*
        VRAM is on a 16-bit bus.
        1 MiB VRAM is laid out as 512 lines of 2048 bytes each.
        2 MiB VRAM (only present on some arcade boads, not on consoles) is laid out as 1024 lines instead.
        It is accessed via coordinates, ranging from (0,0)=Upper-Left to (N,1023)=Lower-Right.
    */
    // VRAM layout
    static constexpr int VRAM_WIDTH = 2048;   // Total width of a VRAM, columns in bytes.
    static constexpr int VRAM_HEIGHT = 512;   // Total height of a VRAM in lines
    static constexpr int MIN_TILE_SIZE = 8;   // 1 Tile: Minimum of 8×8 pixels in 4BPP, equates to 4px 8 BPP, 2px 16 BPP, 1px 24 BPP
    static constexpr int PAGE_SIZE = 256;     // 256x256 texture page size for W/H (in any BPP, serves as offset, 128 bytes (256px) in 4BPP, 256 bytes (128px) in 8bpp etc...)
    static constexpr int PAGE_GRID_COLS = 16; // Texture pages per row
    static constexpr int PAGE_MIN_ROWS = 2;
    static constexpr int MAX_CLUT_LINES_IN_TILES = 5; // How many lines of CLUTs to use before considering there's too many. As this eats into tile space, this is in multiples of MIN_TILE_SIZE.
    static constexpr int MAX_CLUT_LINES_PER_PAGE = MIN_TILE_SIZE * MAX_CLUT_LINES_IN_TILES;

    static constexpr const int VRAM_WIDTH_IN_PX(const uint8_t bpp)
    {
        return ((VRAM_WIDTH * 10) / (((bpp * 10) / 8)));
    }

    static constexpr const int PIXELS_PER_TILE(const uint8_t bpp)
    {
        // Switch to enforce 4/8/16/24 bpp results
        switch (bpp)
        {
        default:
        case 4: return MIN_TILE_SIZE;
        case 8: return (MIN_TILE_SIZE / 2);
        case 16: return (MIN_TILE_SIZE / 4);
        case 24: return (MIN_TILE_SIZE / 5);
        }
    }

    static constexpr const int BYTES_PER_PIXEL(const uint8_t bpp)
    {
        // Switch to enforce 4/8/16/24 bpp results
        switch (bpp)
        {
        default:
        case 4: return 1;
        case 8: return 1;
        case 16: return 2;
        case 24: return 3;
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

        struct TexPageCoord
        {
            uint8_t page = 0, tileX = 0, tileY = 0;
        };

        struct VRAMCoord
        {
            uint16_t x = 0, y = 0;
        };

    private:
        class VRAM_Bitmap_POD
        {
        private:
            // Two bitmaps to save on RAM usage (tile is 8x8, CLUT is 8x1)
            uint32_t *_tile_bitmap;
            size_t _tile_bitmap_size = 0;
            uint32_t *_clut_bitmap;
            size_t _clut_bitmap_size = 0;

        public:
            enum : bool
            {
                TMGR_INUSE,
                TMGR_FREE,
            };

            enum
            {
                TMGR_BITMAP_OKAY,
                TMGR_BITMAP_ERR_MALLOC_FAIL,
                TMGR_BITMAP_ERR_COLLISION,
                TMGR_BITMAP_ERR_OUT_OF_BOUNDS,
                TMGR_BITMAP_ERR_NO_FREE_SPACE
            };

            int init(uint8_t _vram);

            /* Mark a tile or block of tiles as in use/free. X/Y/W/H in tiles */
            inline bool mark_block(RectWH rect, bool state) { return mark_block(rect.x, rect.y, rect.w, rect.h, state); }
            bool mark_block(int x, int y, int w, int h, bool state);

            /* Returns whether a tile or block of tiles as in use/free. X/Y/W/H in tiles */
            inline bool test_block(RectWH rect) const { return test_block(rect.x, rect.y, rect.w, rect.h); }
            bool test_block(int x, int y, int w, int h) const;

            inline bool process_block(RectWH rect, bool state, bool dry_run) { return process_block(rect.x, rect.y, rect.w, rect.h, state, dry_run); }
            bool process_block(int x, int y, int w, int h, bool state, bool dry_run);

            /* Returns whether a tile or block of tiles as in use/free. X/Y/W/H in tiles */
            inline bool tile_available(RectWH rect) const { return tile_available(rect.x, rect.y); }
            bool tile_available(int tile_x, int tile_y) const;

            /* Returns whether a CLUT line is in use/free. X/Y in starting tile space, W in bitwidth, H in line */
            bool clut_available(int x, int y, int w) const;

            void mark_tile(int x, int y, bool state);
            void mark_clut(int x, int y, int w, bool state);

            bool boundries(uint8_t _vram, int &w, int &h)
            {
                if (!_tile_bitmap || !_clut_bitmap) return false;
                w = VRAM_WIDTH;
                h = (VRAM_HEIGHT * _vram);
                return true;
            };

            inline void clear_all()
            {
                if (!_tile_bitmap || !_clut_bitmap) return;
                memset(_tile_bitmap, 0, _tile_bitmap_size);
                memset(_clut_bitmap, 0, _clut_bitmap_size);
            }

            inline bool clut_y_valid(uint16_t y) const
            {
                if ((y < (((VRAM_HEIGHT / MIN_TILE_SIZE) / 2) - MAX_CLUT_LINES_IN_TILES) && y < ((VRAM_HEIGHT / MIN_TILE_SIZE) / 2)) ||
                    y < ((((VRAM_HEIGHT / MIN_TILE_SIZE) / 2) - MAX_CLUT_LINES_IN_TILES) + ((VRAM_HEIGHT / MIN_TILE_SIZE) / 2)))
                    return false;
                return true;
            }

            inline uint8_t clut_y_row(uint16_t y) const
            {
                if (!clut_y_valid(y)) return 0;
                return (y / (((VRAM_HEIGHT / MIN_TILE_SIZE) / 2) - MAX_CLUT_LINES_IN_TILES));
            }

        } _vramBitmap;

        uint8_t _vramSize = 0;  // VRAM size in MiB
        RectWH _frameBufferBox; // In px

        // Finds a free space in the texture page where the requested block can go, or returns no space.
        // X,Y,W,H in tiles
        int findFreeBlock(uint16_t &x, uint16_t &y, uint8_t w, uint8_t h, uint8_t bpp) const;

        // Finds a free space in the texture page where the requested CLUT can go, or returns no space.
        // A CLUT can go anywhere within reason in the VRAM, but for the sake of simplicity, for now
        // findFreeCLUT will only return TMGR_OKAY if it can allocate space on the same texture page.
        // X and Y are PIXEL values, not tile values.
        // TODO: Allow CLUT to exist independantly of texture
        int findFreeCLUT(uint16_t &x, uint16_t &y, uint8_t bpp) const;

        int processLargeBlock(
            uint16_t x, uint16_t y, uint8_t w, uint8_t h, // Block
            uint8_t state,                                // State to write (ignored on dry run)
            bool dryRun);                                 // Does not modify anything if true

    public:
        TextureManager(uint8_t vram_size) // vram_size in MiB
        {
            _vramSize = vram_size;
        }

        ~TextureManager() = default;

        // Aligns a pixel to a tile index
        static inline constexpr uint8_t pxToTile(uint16_t px)
        {
            return ((px % PAGE_SIZE) / MIN_TILE_SIZE);
        }
        // Gets the starting pixel of a tile
        static inline constexpr uint16_t tileToPx(uint16_t tile)
        {
            return (MIN_TILE_SIZE * tile);
        }

        static inline constexpr uint16_t clutWidth(uint8_t bpp)
        {
            return (bpp == Textures::BPP_8BIT ? MAX_COLORS_8BPP : MAX_COLORS_4BPP);
        }

        // Creates and clears the allocation maps.
        inline int init() { return _vramBitmap.init(_vramSize); };

        // Marks parts of the VRAM as in use for the framebuffer so it won't be wastefully checked during allocation.
        // Only supports 16bpp framebuffers
        int markFrameBuffer(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h);

        // Allocate space in the texture page for a CLUT
        int allocateCLUT(PSXTextureObject *ptObj);

        // Allocate space in the texture page for a texture
        int allocateTexture(PSXTextureObject *ptObj);

        // Deallocates space reserved for a texture and CLUT (if used)
        int deallocateTexture(PSXTextureObject *ptObj);

        // Deallocates ALL VRAM
        inline void clearAll(void)
        {
            _vramBitmap.clear_all();
        }
    };
} // namespace System::PSX::GPU
