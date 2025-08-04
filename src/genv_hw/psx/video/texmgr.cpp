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

#include <string.h>
#include "texmgr.hpp"

using namespace Textures;

namespace PSX
{
    int TextureManager::init()
    {
        if (_pages)
            delete[] _pages;
        _pages = new PageAllocState[(PSX_GPU_PAGE_GRID_COLS * _vramTPageRows)];
        memset(_pages, 0, sizeof(PageAllocState) * (PSX_GPU_PAGE_GRID_COLS * _vramTPageRows));

        return TPAGE_OKAY;
    }

    int TextureManager::processLargeBlock(
        uint8_t state,
        uint8_t tpageStart,
        uint8_t xTile, uint8_t yTile,
        uint16_t wTiles, uint16_t hTiles,
        bool dryRun)
    {
        uint16_t rowsLeft = hTiles;
        uint8_t page = tpageStart;
        // NOTE: yOffset never resets, so each page starts at the same tile-row
        uint16_t yOffset = yTile;

        while (rowsLeft > 0)
        {
            uint16_t availRows = PSX_GPU_TILES_PER_ROW - yOffset;
            uint16_t doRows = (rowsLeft < availRows ? rowsLeft : availRows);

            uint16_t yEnd = yOffset + doRows;
            uint16_t xEnd = xTile + wTiles;

            for (uint16_t yy = yOffset; yy < yEnd; ++yy)
            {
                for (uint16_t xx = xTile; xx < xEnd; ++xx)
                {
                    if (!dryRun)
                    {
                        // collision-check pass
                        if (testTile(page, xx, yy))
                            return TPAGE_COLLISION;
                    }
                    else
                    {
                        // marking pass
                        if (state)
                            setTile(page, xx, yy);
                        else
                            clearTile(page, xx, yy);
                    }
                }
            }

            rowsLeft -= doRows;
            ++page; // move to next page, keep yOffset the same
        }

        return TPAGE_OKAY;
    }

    int TextureManager::markLargeBlock(
        const bool state,
        uint8_t tpageStart,
        uint8_t xTile, uint8_t yTile,
        const uint16_t wTiles, const uint16_t hTiles)
    {
        // Each tpage colum is represented by a bit in a 32-bit memory cell.
        uint16_t rowsLeft = hTiles;
        uint8_t page = tpageStart;
        uint16_t yOffset = yTile; // preserve this for every page

        // Check for collisions with existing tiles
        int r = processLargeBlock(state, tpageStart, xTile, yTile, wTiles, hTiles, true);
        if (r < 0)
            return r;

        // Phase 2: actually set or clear the bits
        return processLargeBlock(state, tpageStart, xTile, yTile, wTiles, hTiles, false);
    }

    int TextureManager::markFrameBuffer(
        const uint8_t xPx, const uint8_t yPx,
        const uint16_t wPx, const uint16_t hPx)
    {
        int r = 0;
        _frameBufferBox.x = (xPx / PSX_GPU_PAGE_PIXELS);
        _frameBufferBox.y = (yPx / PSX_GPU_PAGE_PIXELS);
        _frameBufferBox.w = ((xPx + wPx) / PSX_GPU_PAGE_PIXELS);
        _frameBufferBox.h = ((yPx + hPx) / PSX_GPU_PAGE_PIXELS);
        r = markLargeBlock(
            TPAGE_INUSE,
            _frameBufferBox.x + (PSX_GPU_PAGE_GRID_COLS * _frameBufferBox.y),
            _frameBufferBox.x / PSX_GPU_MIN_TILE_SIZE,
            _frameBufferBox.y / PSX_GPU_MIN_TILE_SIZE,
            _frameBufferBox.w / PSX_GPU_MIN_TILE_SIZE,
            _frameBufferBox.h / PSX_GPU_MIN_TILE_SIZE);
        return r;
    }

    int TextureManager::findFreeBlock(
        uint8_t tpage,
        uint8_t wTiles, uint8_t hTiles,
        uint8_t &xTile, uint8_t &yTile) const
    {
        for (; yTile < PSX_GPU_TILES_PER_ROW; yTile++)
        {
            // Always move to the left-most position when starting a new row
            xTile = 0;
            if (((uint16_t)yTile + (uint16_t)hTiles) >= PSX_GPU_TILES_PER_COL)
            {
                return TPAGE_NO_FREE_SPACE;
            }

            for (; xTile < PSX_GPU_TILES_PER_COL; xTile++)
            {
                if (((uint16_t)xTile + (uint16_t)wTiles) >= PSX_GPU_TILES_PER_COL)
                {
                    // Would exceed texture page
                    // TODO: Allow?
                    break;
                }

                // Test left side first, as we can move to the right
                // to avoid left-most tiles
                if (testTile(tpage, xTile, yTile) ||
                    testTile(tpage, xTile, yTile + hTiles))
                    continue;

                // If there is a texture to the right, break the x loop
                // as there is no point trying to shift right.
                if (testTile(tpage, xTile + wTiles, yTile) ||
                    testTile(tpage, xTile + wTiles, yTile + hTiles))
                    break;

                // Four corners are free, check the block
                bool blockInUse = false;
                for (int yy = yTile; yy < hTiles; yy++)
                {
                    for (int xx = xTile; xx < wTiles; xx++)
                    {
                        if (testTile(tpage, xx, yy))
                        {
                            blockInUse = true;
                            break;
                        }
                    }
                    if (blockInUse)
                    {
                        break;
                    }
                }

                if (!blockInUse)
                {
                    // Bock is free
                    return TPAGE_OKAY;
                }
                else
                {
                    // Somewhere there are tiles in use,
                    // assume the entire block is in use
                    yTile += hTiles;
                    break;
                }
            }
        }
        return TPAGE_NO_FREE_SPACE;
    }

    // CLUT Index and Line is in px
    int TextureManager::findFreeCLUT(uint8_t tpage, GP0ColorDepth bitdepth, uint8_t &clutIdx, uint8_t &clutLine) const
    {
        // CLUTs must be aligned to x = (16 * idx)
        // 8bpp takes up the entire row.
        uint8_t depthWidth = (bitdepth == GP0_COLOR_8BPP ? PSX_GPU_8BPP_COLORS : PSX_GPU_4BPP_COLORS);

        for (; clutLine > (PSX_GPU_PAGE_PIXELS - PSX_GPU_MAX_CLUT_LINES_PER_PAGE); clutLine--)
        {
            // Always move to the left-most position when starting a new row
            clutIdx = 0;

            for (; (uint16_t)clutIdx < (PSX_GPU_PAGE_PIXELS - 1);)
            {
                // Check the strip
                bool stripInUse = false;
                for (int xx = clutIdx; xx < (PSX_GPU_PAGE_PIXELS - 1); xx++)
                {
                    if (testTile(tpage, xx, clutLine))
                    {
                        stripInUse = true;
                        break;
                    }
                }

                if (!stripInUse)
                {
                    // Strip is free
                    return TPAGE_OKAY;
                }
                clutIdx += depthWidth;
            }
        }
        return TPAGE_NO_FREE_SPACE;
    }

    int TextureManager::allocateCLUT(PSXTextureObject *ptObj)
    {
        if (!ptObj || (ptObj->bpp != TextureObject::BPP_4BIT && ptObj->bpp != TextureObject::BPP_8BIT))
            return TPAGE_INVALID_OBJECT;

        int r = 0;
        uint8_t x = 0, y = 255; // Try to put CLUTs at the bottom of the table so that there's more space for textures.
        GP0ColorDepth depthWidth = (ptObj->bpp == TextureObject::BPP_8BIT ? GP0_COLOR_8BPP : GP0_COLOR_4BPP);

        r = findFreeCLUT(ptObj->vramP, depthWidth, x, y);
        if (r < 0)
            return r;

        ptObj->clutIdx = x;
        ptObj->clutLine = y;

        r = markBlock(TPAGE_INUSE,
                      ptObj->vramP,
                      pxToTile(x), pxToTile(y),
                      depthWidth, 1);

        return r;
    }

    int TextureManager::allocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj)
            return TPAGE_INVALID_OBJECT;

        int r = 0;
        bool stopSearch = false;
        uint8_t tpage = PSX_GPU_PAGE_GRID_COLS; // Top right of VRAM
        uint8_t tpage_row = 0;
        uint8_t xTile = 0, yTile = 0;

        do
        {
            r = findFreeBlock(tpage, pxToTile(ptObj->width), pxToTile(ptObj->height), xTile, yTile);
            if (r == TPAGE_OKAY)
            {
                // Try to mark the block
                if (markLargeBlock(
                        TPAGE_INUSE,
                        tpage, xTile, yTile,
                        pxToTile(ptObj->width), pxToTile(ptObj->height)) == TPAGE_OKAY)
                {
                    stopSearch = true;
                }
            }
            else
            {
                // Top-right to Top-left, then try second row
                tpage--;
                // This test tries to avoid testing the framebuffer region
                if (tpage <= _frameBufferBox.w && tpage_row <= _frameBufferBox.h)
                {
                    tpage_row++;
                    if (tpage_row >= _vramTPageRows)
                    {
                        return TPAGE_NO_FREE_SPACE;
                    }
                    tpage = (PSX_GPU_PAGE_GRID_COLS * tpage_row);
                }
            }
        } while (!stopSearch);

        ptObj->vramP = tpage;
        ptObj->vramX = tileToPx(xTile);
        ptObj->vramY = tileToPx(yTile);

        if (ptObj->bpp == TextureObject::BPP_4BIT || ptObj->bpp == TextureObject::BPP_8BIT)
        {
            return allocateCLUT(ptObj);
        }

        return TPAGE_OKAY;
    }

    int TextureManager::deallocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj)
            return TPAGE_INVALID_OBJECT;

        int r = 0;
        r = markLargeBlock(TPAGE_FREE,
                           ptObj->vramP,
                           pxToTile(ptObj->vramX),
                           pxToTile(ptObj->vramY),
                           pxToTile(ptObj->width),
                           pxToTile(ptObj->height));

        if (ptObj->bpp == TextureObject::BPP_4BIT || ptObj->bpp == TextureObject::BPP_8BIT)
        {
            uint8_t depthWidth = (ptObj->bpp == GP0_COLOR_8BPP ? PSX_GPU_8BPP_COLORS : PSX_GPU_4BPP_COLORS);
            markBlock(TPAGE_FREE,
                      ptObj->vramP,
                      ptObj->clutIdx,
                      ptObj->clutLine,
                      depthWidth,
                      1);
        }

        return r;
    }
}