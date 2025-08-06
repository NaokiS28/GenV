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

namespace System::PSX::GPU
{
    int TextureManager::init()
    {
        if (_pages)
            delete[] _pages;
        _pages = new PageAllocState[(PSX::GPU::PAGE_GRID_COLS * _vramPageRows)];
        memset(_pages, 0, sizeof(PageAllocState) * (PSX::GPU::PAGE_GRID_COLS * _vramPageRows));

        return TMGR_OKAY;
    }

    int TextureManager::processLargeBlock(
        uint8_t state,
        uint8_t pageStart,
        uint8_t xTile, uint8_t yTile,
        uint16_t wTiles, uint16_t hTiles,
        bool dryRun)
    {
        uint16_t rowsLeft = hTiles;
        uint8_t page = pageStart;
        // NOTE: yOffset never resets, so each page starts at the same tile-row
        uint16_t yOffset = yTile;

        while (rowsLeft > 0)
        {
            uint16_t availRows = PSX::GPU::TILES_PER_ROW - yOffset;
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
                            return TMGR_COLLISION;
                    }
                    else
                    {
                        // marking pass
                        writeTile(state, page, xx, yy);
                    }
                }
            }

            rowsLeft -= doRows;
            ++page; // move to next page, keep yOffset the same
        }

        return TMGR_OKAY;
    }

    int TextureManager::markLargeBlock(
        const bool state,
        uint8_t pageStart,
        uint8_t xTile, uint8_t yTile,
        const uint16_t wTiles, const uint16_t hTiles)
    {
        // Check for collisions with existing tiles
        int r = processLargeBlock(state, pageStart, xTile, yTile, wTiles, hTiles, true);
        if (r < 0)
            return r;

        // Phase 2: actually set or clear the bits
        return processLargeBlock(state, pageStart, xTile, yTile, wTiles, hTiles, false);
    }

    int TextureManager::markFrameBuffer(
        const uint8_t xPx, const uint8_t yPx,
        const uint16_t wPx, const uint16_t hPx)
    {
        int r = 0;
        _frameBufferBox.x = (xPx / PSX::GPU::PAGE_PIXELS);
        _frameBufferBox.y = (yPx / PSX::GPU::PAGE_PIXELS);
        _frameBufferBox.w = ((xPx + wPx) / PSX::GPU::PAGE_PIXELS);
        _frameBufferBox.h = ((yPx + hPx) / PSX::GPU::PAGE_PIXELS);
        r = markLargeBlock(
            TMGR_INUSE,
            _frameBufferBox.x + (PSX::GPU::PAGE_GRID_COLS * _frameBufferBox.y),
            _frameBufferBox.x / PSX::GPU::MIN_TILE_SIZE,
            _frameBufferBox.y / PSX::GPU::MIN_TILE_SIZE,
            _frameBufferBox.w / PSX::GPU::MIN_TILE_SIZE,
            _frameBufferBox.h / PSX::GPU::MIN_TILE_SIZE);
        return r;
    }

    bool TextureManager::testTile(const uint8_t page, const uint8_t tileX, const uint8_t tileY) const
    {
        // Only read CLUT bitmap if we are in the CLUT region
        if (tileY >= (255 - (PSX::GPU::MAX_CLUT_LINES_PER_PAGE / PSX::GPU::MIN_TILE_SIZE)))
        {
            // Check the entire block
            uint8_t startLine = ((PSX::GPU::MIN_TILE_SIZE - 1) - tileY);
            for (int cY = startLine; cY < (startLine + (PSX::GPU::MIN_TILE_SIZE - 1)); cY++)
            {
                if (testCLUT(page, cY, tileToPx(tileX)))
                    return true;
            }
        }
        return ((_pages[page].tileBitmap[tileY] & (1 << tileX)) != 0);
    }

    void TextureManager::writeTile(const bool state, const uint8_t page, const uint8_t tileX, const uint8_t tileY)
    {
        if(state)
            setTile(page, tileX, tileY);
        else
            clearTile(page, tileX, tileY);

        // Only write to CLUT bitmap if we are in the CLUT region
        if (tileY >= (255 - (PSX::GPU::MAX_CLUT_LINES_PER_PAGE / PSX::GPU::MIN_TILE_SIZE)))
        {
            // Clear the entire block
            uint8_t startLine = ((PSX::GPU::MIN_TILE_SIZE - 1) - tileY);
            for (int cY = startLine; cY < (startLine + (PSX::GPU::MIN_TILE_SIZE - 1)); cY++)
            {
                if(state)
                    setCLUT(page, cY, tileToPx(tileX));
                else
                    clearCLUT(page, cY, tileToPx(tileX));
            }
        }
    }

    int TextureManager::findFreeBlock(
        TexPageEntry &t,
        uint8_t wTiles, uint8_t hTiles) const
    {
        for (; t.tileY < PSX::GPU::TILES_PER_ROW; t.tileY++)
        {
            // Always move to the left-most position when starting a new row
            t.tileX = 0;
            if (((uint16_t)t.tileY + (uint16_t)hTiles) >= PSX::GPU::TILES_PER_COL)
            {
                return TMGR_NO_FREE_SPACE;
            }

            for (; t.tileX < PSX::GPU::TILES_PER_COL; t.tileX++)
            {
                if (((uint16_t)t.tileX + (uint16_t)wTiles) >= PSX::GPU::TILES_PER_COL)
                {
                    // Would exceed texture page
                    // TODO: Allow?
                    break;
                }

                // Test left side first, as we can move to the right
                // to avoid left-most tiles
                if (testTile(t.page, t.tileX, t.tileY) ||
                    testTile(t.page, t.tileX, t.tileY + hTiles))
                    continue;

                // If there is a texture to the right, break the x loop
                // as there is no point trying to shift right.
                if (testTile(t.page, t.tileX + wTiles, t.tileY) ||
                    testTile(t.page, t.tileX + wTiles, t.tileY + hTiles))
                    break;

                // Four corners are free, check the block
                bool blockInUse = false;
                for (int yy = t.tileY; yy < hTiles; yy++)
                {
                    for (int xx = t.tileX; xx < wTiles; xx++)
                    {
                        if (testTile(t.page, xx, yy))
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
                    return TMGR_OKAY;
                }
                else
                {
                    // Somewhere there are tiles in use,
                    // assume the entire block is in use
                    t.tileY += hTiles;
                    break;
                }
            }
        }
        return TMGR_NO_FREE_SPACE;
    }

    // X and Y is in px
    int TextureManager::findFreeCLUT(uint8_t page, GP0ColorDepth bitdepth, uint8_t &clutLine, uint8_t &xPx) const
    {
        // CLUTs must be aligned to x = (16 * idx)
        // 8bpp takes up the entire row.
        uint8_t depthWidth = bppPxWidth(bitdepth);

        for (; clutLine > (PSX::GPU::PAGE_PIXELS - PSX::GPU::MAX_CLUT_LINES_PER_PAGE); clutLine--)
        {
            // Always move to the left-most position when starting a new row
            xPx = 0;
            for (; (uint16_t)xPx < (PSX::GPU::PAGE_PIXELS - 1);)
            {
                // Check the strip
                bool stripInUse = false;
                // Check 8 in tiles
                for (int xx = xPx; xx < (PSX::GPU::PAGE_PIXELS - 1); xx += depthWidth)
                {
                    if (testCLUT(page, clutLine, xx))
                    {
                        stripInUse = true;
                        break;
                    }
                }

                if (!stripInUse)
                {
                    // Strip is free
                    return TMGR_OKAY;
                }
                xPx += depthWidth;
            }
        }
        return TMGR_NO_FREE_SPACE;
    }

    int TextureManager::allocateCLUT(PSXTextureObject *ptObj)
    {
        if (!ptObj || (ptObj->bpp != Textures::BPP_4BIT && ptObj->bpp != Textures::BPP_8BIT))
            return TMGR_INVALID_OBJECT;

        int r = 0;
        uint8_t x = 0, clutLine = 255; // Try to put CLUTs at the bottom of the table so that there's more space for textures.
        GP0ColorDepth depthWidth = (ptObj->bpp == Textures::BPP_8BIT ? GP0_COLOR_8BPP : GP0_COLOR_4BPP);

        r = findFreeCLUT(ptObj->texPage, depthWidth, clutLine, x);
        if (r != TMGR_OKAY)
            return r;

        VRAMEntry v = clutToVRAM(ptObj->texPage, x, clutLine);
        ptObj->clutX = v.x;
        ptObj->clutY = v.y;

        markCLUT(TMGR_INUSE, ptObj->texPage, clutLine, v.x, depthWidth);
        return r;
    }

    int TextureManager::allocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj)
            return TMGR_INVALID_OBJECT;

        int r = 0;
        bool stopSearch = false;
        uint8_t TMGR_row = 0;
        TexPageEntry t;
        t.page = (PSX::GPU::PAGE_GRID_COLS - 1); // Top right of VRAM

        do
        {
            r = findFreeBlock(t, pxToTile(ptObj->width), pxToTile(ptObj->height));
            if (r == TMGR_OKAY)
            {
                // Try to mark the block
                if (markLargeBlock(
                        TMGR_INUSE,
                        t.page, t.tileX, t.tileY,
                        pxToTile(ptObj->width), pxToTile(ptObj->height)) == TMGR_OKAY)
                {
                    stopSearch = true;
                }
            }
            else
            {
                // Top-right to Top-left, then try second row
                t.page--;
                // This test tries to avoid testing the framebuffer region
                if (t.page <= _frameBufferBox.w && TMGR_row <= _frameBufferBox.h)
                {
                    TMGR_row++;
                    if (TMGR_row >= _vramPageRows)
                    {
                        return TMGR_NO_FREE_SPACE;
                    }
                    t.page = (PSX::GPU::PAGE_GRID_COLS * TMGR_row);
                }
            }
        } while (!stopSearch);

        VRAMEntry v = tpageToVRAM(t);
        ptObj->texPage = t.page;
        ptObj->vramX = v.x;
        ptObj->vramY = v.y;

        if (ptObj->bpp == Textures::BPP_4BIT || ptObj->bpp == Textures::BPP_8BIT)
        {
            return allocateCLUT(ptObj);
        }

        return TMGR_OKAY;
    }

    int TextureManager::deallocateTexture(PSXTextureObject *ptObj)
    {
        if (!ptObj)
            return TMGR_INVALID_OBJECT;

        int r = 0;
        TexPageEntry t = vramToTPage(ptObj->vramX, ptObj->vramY);
        r = markLargeBlock(TMGR_FREE,
                           t.page, t.tileX, t.tileY,
                           pxToTile(ptObj->width),
                           pxToTile(ptObj->height));

        if (ptObj->bpp == Textures::BPP_4BIT || ptObj->bpp == Textures::BPP_8BIT)
        {
            uint8_t depthWidth = bppPxWidth(ptObj->bpp);
            markCLUT(TMGR_FREE, ptObj->texPage, ptObj->clutY, ptObj->clutX, depthWidth);
        }

        return r;
    }
}