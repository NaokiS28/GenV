/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * texman.cpp - Created on 04-08-2025
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

// TODO: Code works but has unnaccounted edge cases and an O(-3) problem as spicy puts it.
// Ideally needs going through and tidying up.

#include <stdint.h>
#include <string.h>
#include "texmgr.hpp"
#include "common/logger/log.hpp"
#include "common/util/rect.h"
#include "gpucmd.hpp"
#include "common/return_codes.hpp"

namespace System::PSX::GPU
{
    int TextureManager::VRAM_Bitmap_POD::init(GP1VRAMSize _vram)
    {
        // Using 8bpp width here as we're dealing with bytes, hieght is 4bpp since each line is a pixel
        _tile_bitmap_size = ((VRAM_WIDTH / (PIXELS_PER_TILE(8) * 32)) * (VRAM_HEIGHT / (PIXELS_PER_TILE(4))));
        _clut_bitmap_size = ((VRAM_WIDTH / (PIXELS_PER_TILE(8) * 32)) * (MAX_CLUT_LINES_PER_PAGE * (PAGE_MIN_ROWS * (1 + _vram))));
        _tile_bitmap      = new uint32_t[_tile_bitmap_size];
        _clut_bitmap      = new uint32_t[_clut_bitmap_size];

        if (!_tile_bitmap || !_clut_bitmap)
        {
            LOG("TextureManager", "Unexpected error in allocating bitmap(s). Dynamic allocation is not possible!");
            return TM_ERROR(GV_ERR_OUT_OF_MEMORY);
        }
        // UINT32_MAX allows TMGR_FREE to be true for bool functions
        memset(_tile_bitmap, (UINT32_MAX * TMGR_FREE), sizeof(uint32_t) * _tile_bitmap_size);
        memset(_clut_bitmap, (UINT32_MAX * TMGR_FREE), sizeof(uint32_t) * _clut_bitmap_size);
        return GV_OK;
    }

    bool TextureManager::VRAM_Bitmap_POD::tile_available(int x, int y) const
    {
        if (!_tile_bitmap) return TMGR_INUSE;
        uint32_t bitmask = (1 << (x % 32));
        size_t tile_idx  = ((x / 32) + ((VRAM_WIDTH / (PIXELS_PER_TILE(8) * 32)) * y));
        return ((_tile_bitmap[tile_idx] & bitmask) != TMGR_INUSE);
    }

    bool TextureManager::VRAM_Bitmap_POD::clut_available(int x, int y, int w) const
    {
        uint8_t row = clut_y_row(y);

        if (!_clut_bitmap) return TMGR_INUSE;
        if ((x % 2) != 0 ||                                 // X can only be in multiples of 16
            !clut_y_valid(y) ||                             // Y can only be in the last N lines of a page
            (row >= MAX_CLUT_LINES_PER_PAGE) ||             // Lines must be within the allowed limit
            (w != MAX_COLORS_4BPP && w != MAX_COLORS_8BPP)) // Bitdepth width must be 4bpp or 8bpp
            return TMGR_INUSE;

        uint32_t bitmask = (1 << (x % 32));
        size_t clut_idx  = ((x / 32) + (((VRAM_WIDTH * 2) / (PIXELS_PER_TILE(8) * 32)) * row) + (y % MAX_CLUT_LINES_PER_PAGE));
        for (int i = 0; i < w; i += MAX_COLORS_4BPP)
        {
            if ((_clut_bitmap[clut_idx] & bitmask) != TMGR_FREE)
                return TMGR_INUSE;
        }
        return TMGR_FREE;
    }

    bool TextureManager::VRAM_Bitmap_POD::test_block(int x, int y, int w, int h) const
    {
        bool block = TMGR_FREE;
        for (uint16_t yy = y; yy < (y + h); yy++)
        {
            for (uint16_t xx = x; xx < (x + w); xx++)
            {
                if (!tile_available(xx, yy))
                {
                    block = TMGR_INUSE;
                    break;
                }
            }
            if (block == TMGR_INUSE) break;
        }
        return block;
    }

    void TextureManager::VRAM_Bitmap_POD::mark_tile(int x, int y, bool state)
    {
        if (!_tile_bitmap) return;
        uint32_t bitmask = (1 << (x % 32));
        size_t tile_idx  = ((x / 32) + ((VRAM_WIDTH / (PIXELS_PER_TILE(8) * 32)) * y));
        if (state)
            _tile_bitmap[tile_idx] |= bitmask;
        else
            _tile_bitmap[tile_idx] &= ~(bitmask);
    }

    void TextureManager::VRAM_Bitmap_POD::mark_clut(int x, int y, int w, bool state)
    {
        uint8_t row = clut_y_row(y);

        if (!_clut_bitmap) return;
        if ((x % 2) != 0 ||                                 // X can only be in multiples of 16
            !clut_y_valid(y) ||                             // Y can only be in the last N lines of a page
            (row >= MAX_CLUT_LINES_PER_PAGE) ||             // Lines must be within the allowed limit
            (w != MAX_COLORS_4BPP && w != MAX_COLORS_8BPP)) // Bitdepth width must be 4bpp or 8bpp
            return;

        uint32_t bitmask = (1 << (x % 32));
        size_t clut_idx  = ((x / 32) + (((VRAM_WIDTH * 2) / (PIXELS_PER_TILE(8) * 32)) * row) + (y % MAX_CLUT_LINES_PER_PAGE));
        if (state)
            _clut_bitmap[clut_idx] |= bitmask;
        else
            _clut_bitmap[clut_idx] &= ~(bitmask);
    }

    bool TextureManager::VRAM_Bitmap_POD::process_block(int x, int y, int w, int h, bool state, bool dry_run)
    {
        uint16_t xEnd = x + w;
        uint16_t yEnd = y + h;
        // Rows
        for (uint16_t yy = y; yy < yEnd; ++yy)
        {
            // Cols
            for (uint16_t xx = x; xx < xEnd; ++xx)
            {
                if (dry_run)
                {
                    // collision-check pass
                    int r = 0;
                    for (int cy = yy; clut_y_valid(cy) && cy < yy + 8; cy++)
                    {
                        if (clut_available(xx, yy + cy, MAX_COLORS_4BPP))
                        {
                            r = TMGR_INUSE;
                            break;
                        }
                    }
                    if (r || !tile_available(xx, yy))
                        return TMGR_INUSE;
                }
                else
                {
                    mark_tile(xx, yy, state);
                    if (clut_y_valid(yy))
                        for (int cy = yy; clut_y_valid(cy) && cy < yy + 8; cy++)
                        {
                            mark_clut(xx, yy + cy, MAX_COLORS_4BPP, state);
                        }
                }
            }
        }
        return TMGR_FREE;
    }

    bool TextureManager::VRAM_Bitmap_POD::mark_block(int x, int y, int w, int h, bool state)
    {
        int r = process_block(x, y, w, h, state, true);
        if (r < 0)
            return r;
        return process_block(x, y, w, h, state, false);
    }

    int TextureManager::markFrameBuffer(const uint16_t x, const uint16_t y, const uint16_t w, const uint16_t h)
    {
        int r = 0;
        if (_frameBufferBox.w && _frameBufferBox.h)
        {
            r = _vramBitmap.mark_block(
                _frameBufferBox.x,
                _frameBufferBox.y,
                _frameBufferBox.w,
                _frameBufferBox.h,
                VRAM_Bitmap_POD::TMGR_FREE);
        }

        _frameBufferBox = {
            pxToTile(x),
            pxToTile(y),
            (w / PIXELS_PER_TILE(16)),
            (h / MIN_TILE_SIZE)};
        r = _vramBitmap.mark_block(
            _frameBufferBox.x,
            _frameBufferBox.y,
            _frameBufferBox.w,
            _frameBufferBox.h,
            VRAM_Bitmap_POD::TMGR_INUSE);
        return r;
    }

    int TextureManager::findFreeBlock(uint16_t &x, uint16_t &y, uint8_t w, uint8_t h, uint8_t bpp) const
    {
        // Always start at top left corner and try to align to left edge of nearest page.
        // Push to farthest right page start
        uint32_t pageW = (VRAM_WIDTH_IN_PX(bpp) / bpp); // in pixels
        uint32_t pageX = (PAGE_GRID_COLS - (1 + (w / pageW)));
        uint32_t pageY = 0;
        int startX     = ((PAGE_SIZE / MIN_TILE_SIZE) * pageX);
        int startY     = 0;
        int maxH       = PAGE_SIZE / MIN_TILE_SIZE;
        int maxW       = (VRAM_WIDTH_IN_PX(bpp) / MIN_TILE_SIZE);

        RectWH t = {startX, startY, w, h};

        for (;;)
        {
            // Page alignment
            for (; (t.y + (t.h - 1)) < maxH + (32 * (int)pageY); t.y++)
            {
                for (; (t.x + (t.w - 1)) < (maxW * (bpp / 4)); t.x++)
                {
                    // If we're in the framebuffer box, try to snap to the outer edge of it, or move to the next page
                    if (t.x >= _frameBufferBox.x && t.x < (_frameBufferBox.x + _frameBufferBox.w))
                    {
                        if ((((_frameBufferBox.x + _frameBufferBox.w)) + (t.w - 1)) > (maxW * 4))
                            break;
                        t.x = (_frameBufferBox.x + _frameBufferBox.w);
                        continue;
                    }

                    // If fine movement to right isn't going to help, skip next block
                    if (!_vramBitmap.tile_available(t.x + (t.w - 1), t.y) &&
                        !_vramBitmap.tile_available(t.x + (t.w - 1), t.y + (t.h - 1)))
                    {
                        t.x += (t.w - 1); // -1 to account for loop ++
                        continue;
                    }

                    // Check starting corner
                    if (!_vramBitmap.tile_available(t.x, t.y))
                        continue;

                    // Full block probe
                    bool blockFree = _vramBitmap.test_block(t.x, t.y, t.w, t.h);
                    if (blockFree)
                    {
                        // Found a free block at (t.x, t.y)
                        x = t.x;
                        y = t.y;
                        return GV_OK;
                    }
                }

                // Always start a new row from the left
                t.x = ((PAGE_SIZE / MIN_TILE_SIZE) * pageX);

                // If moving down by one line won't help, skip next block.
                if (!_vramBitmap.tile_available(t.x, t.y + (t.h - 1)) &&
                    !_vramBitmap.tile_available(t.x + (t.w - 1), t.y + (t.h - 1)))
                {
                    t.y += (t.h - 1); // -1 to account for loop ++
                    continue;
                }
            }

            // Failed to find space in this page. Move left 1 page or to new line and rightmost page
            if (pageX)
            {
                pageX--;
                t.x = ((PAGE_SIZE / MIN_TILE_SIZE) * pageX);
                t.y = (32 * pageY);
            }
            else if (pageY < (_vramSize + 1))
            {
                pageX = (PAGE_GRID_COLS - 1);
                pageY++;
                t.x = ((PAGE_SIZE / MIN_TILE_SIZE) * pageX);
                t.y = (32 * pageY);
            }
            else
            {
                return TM_ERROR(GV_ERR_OUT_OF_SPACE);
            }
        }
    }

    // X and Y is in px
    int TextureManager::findFreeCLUT(uint16_t &x, uint16_t &y, uint8_t bpp) const
    {
        uint8_t pageX   = 0;
        uint8_t pageY   = ((2 * (_vramSize + 1)) - 1); // Bottom line of VRAM
        uint16_t minH   = (PAGE_SIZE - MAX_CLUT_LINES_PER_PAGE);
        uint16_t startY = (PAGE_SIZE * (pageY + 1)) - 1;

        // CLUT strips are aligned by their pixel width for the given bpp.
        // For 4bpp a strip is 16 pixels wide; for 8bpp it's 256 (i.e., whole row).
        const uint16_t width = clutWidth(bpp); // returns strip width in pixels

        // Search bottom-up within the CLUT band (inclusive).
        x = 0;
        y = startY;
        for (;;)
        {
            // Page alignment
            for (; y > (minH + (PAGE_SIZE * (pageY))); --y)
            {
                bool clutFree = false;
                for (; x < PAGE_SIZE; x += width)
                {
                    if (_vramBitmap.clut_available(x, y, width))
                    {
                        clutFree = true;
                        break;
                    }
                }
                if (clutFree) return GV_OK;
            }

            // Failed to find space in this page. Move right 1 page or to previous line and left most page
            if ((pageX + 1) < (PAGE_GRID_COLS - 1))
                pageX++;
            else if (pageY)
            {
                pageX = (PAGE_GRID_COLS - 1);
                pageY--;
            }
            else
            {
                return TM_ERROR(GV_ERR_OUT_OF_SPACE);
            }
        }

        return TM_ERROR(GV_ERR_OUT_OF_SPACE);
    }

    int TextureManager::allocateCLUT(uint8_t bpp, uint16_t &x, uint16_t &y)
    {
        int r = 0;
        r     = findFreeCLUT(x, y, bpp);
        if (r != GV_OK)
            return r;

        _vramBitmap.mark_clut(x, y, clutWidth(bpp), VRAM_Bitmap_POD::TMGR_INUSE);
        return r;
    }

    int TextureManager::allocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj || !ptObj->width || !ptObj->height)
            return TM_ERROR(GV_ERR_INVALID_PARAM);

        if (ptObj->width >= PAGE_SIZE || ptObj->width >= PAGE_SIZE)
            return TM_ERROR(GV_ERR_INCOMPATIBLE_TYPE);

        int r = 0;

        uint16_t x = 0, y = 0;
        uint8_t w = (ptObj->width / PIXELS_PER_TILE(ptObj->bpp)); // width in tiles (bpp affects width)
        uint8_t h = (ptObj->height / MIN_TILE_SIZE);              // hieght in tiles (bpp does not affect hieght)

        if (w == 0) w = 1;
        if (h == 0) h = 1;

        r = findFreeBlock(x, y, w, h, ptObj->bpp);
        if (r != GV_OK)
            return r;

        r = _vramBitmap.mark_block(x, y, w, h, VRAM_Bitmap_POD::TMGR_INUSE) == GV_OK;
        if (r != GV_OK)
            return r;

        ptObj->vramX = (tileToPx(x) / 4);
        ptObj->vramY = tileToPx(y);
        // TODO: UGLY. UGLY hack.
        ptObj->tpage.offsetX = ((ptObj->vramX % 64) * (ptObj->bpp == 4 ? 4 : ptObj->bpp == 8 ? 2
                                                                                             : 1));
        ptObj->tpage.offsetY = (ptObj->vramY % 256);
        ptObj->tpage.x       = (ptObj->vramX / 64);
        ptObj->tpage.y       = (16 * (ptObj->vramY / 256));

        if (ptObj->bpp == Textures::BPP_4BIT || ptObj->bpp == Textures::BPP_8BIT)
        {
            return allocateCLUT(ptObj);
        }

        return GV_OK;
    }

    int TextureManager::deallocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj)
            return TM_ERROR(GV_ERR_INVALID_PARAM);

        int r = 0;
        // higher bitdepths use more tiles.
        uint16_t x = pxToTile(ptObj->vramX);
        uint16_t y = pxToTile(ptObj->vramY);
        uint8_t w  = (ptObj->width / PIXELS_PER_TILE(ptObj->bpp)); // width in tiles (bpp affects width)
        uint8_t h  = (ptObj->height / MIN_TILE_SIZE);              // hieght in tiles (bpp does not affect height)

        if (w == 0) w = 1;
        if (h == 0) h = 1;

        r = _vramBitmap.mark_block(x, y, w, h, VRAM_Bitmap_POD::TMGR_FREE);

        if (ptObj->bpp == Textures::BPP_4BIT || ptObj->bpp == Textures::BPP_8BIT)
        {
            _vramBitmap.mark_clut(x, y, ptObj->bpp, VRAM_Bitmap_POD::TMGR_FREE);
        }

        return r;
    }
} // namespace System::PSX::GPU