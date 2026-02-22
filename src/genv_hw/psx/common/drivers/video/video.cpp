/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu-d3d.hpp - Created on 24-04-2025
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

#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "video.hpp"
#include "common/objects/texture.hpp"
#include "common/objects/font.hpp"
#include "common/return_codes.hpp"
#include "common/services/video/color.hpp"
#include "common/services/video/iface_video.hpp"
#include "common/services/video/video.hpp"
#include "common/util/hash.hpp"
#include "common/util/rect.h"

#include "psx/common/registers.hpp"
#include "common/logger/log.hpp"
#include "halt_screen/halt_screen.h"
#include "psx/common/halt/halt.h"
#include "psxtex.hpp"
#include "texmgr.hpp"

#define GPUNUM(num) (gpuListPtr = _allocatePacket(this->chain, num))
#define GPUCMD(cmd) \
    if (!_dmaOverflow) { gpuListPtr[dmaPtrIdx++] = cmd; }

namespace System::PSX::GPU
{
    constexpr const size_t PSX_GPU_VSYNC_TIMEOUT = 0x000FFFFF;
    constexpr const int PSX_GPU_DMA_FIFO_SIZE    = 16;

    PSXGPU::PSXGPU() : _texmgr(vramSize)
    {
        registerHaltScreen();
    }

    PSXGPU::PSXGPU(GP1VRAMSize vram_size) : vramSize(vram_size), _texmgr(vram_size)
    {
        registerHaltScreen();
    }

    void PSXGPU::registerHaltScreen()
    {
        GenV_HaltScreenFuncs ops;
        ops.show = &psx_halt_screen_show;
        genv_halt_screen_register(&ops);
    }

    bool PSXGPU::init()
    {
        gpuMode = static_cast<GP1VideoMode>(GPU_GP1 & GP1_STAT_FB_MODE_BITMASK);

        _texmgr.init();

        GPU_GP1 = gp1_resetGPU();
        GPU_GP1 = gp1_vramSize(vramSize);

        // Set the default resolution (240P)
        setResolution(320, 240);

        // Clear VRAM, since it's not uncommon to think something is working because the last action worked.
        _waitForGP0Ready();
        GPU_GP0 = (gp0_rgb(0, 0, 0) | gp0_vramFill());
        GPU_GP0 = (gp0_xy(0, 0));
        GPU_GP0 = (gp0_xy(VRAM_WIDTH / 2, VRAM_HEIGHT));

        // Set up viewport
        _waitForGP0Ready();
        GPU_GP0 = (gp0_texpage(0, true, false));
        GPU_GP0 = (gp0_fbOffset1(0, 0));
        GPU_GP0 = (gp0_fbOffset2(320 - 1, 240 - 1));
        GPU_GP0 = (gp0_fbOrigin(0, 0));

        defaultTexture = Textures::createDefaultTexture();
        uploadTexture(defaultTexture);

#ifndef NDEBUG
        // Fill the screen with blue so we know the init pass of the GPU ran.
        // Might not be visible but if something fails between now and app render
        // we know the GPU at least got to this point.
        _waitForGP0Ready();
        auto color = Colors::Blue;
        GPU_GP0    = (gp0_rgb(color.r, color.g, color.b) | gp0_vramFill());
        GPU_GP0    = (gp0_xy(0, 0));
        GPU_GP0    = (gp0_xy(320, 480));
#endif

        // Enable DMA and unblank the screen
        DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_GPU);
        GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
        GPU_GP1 = gp1_dispBlank(false);

        IRQ_MASK |= (1 << IRQ_VSYNC);
        return GV_OK;
    }

    int PSXGPU::setResolution(int w, int h, bool updateWindow)
    {
        // Set the origin of the displayed framebuffer. These "magic" values,
        // derived from the GPU's internal clocks, will center the picture on most
        // displays and upscalers.
        int x = 0x760;
        int y = (gpuMode == GP1_MODE_PAL) ? 0xa3 : 0x88;

        int result = 0;
        if (w > 640 || h > 320)
            result = V_RES_TOO_HIGH;
        w = (w > 640 ? 640 : w);
        h = (h > 480 ? 480 : h);

        uint32_t mode = findNearestVideoMode(&PSX_Video_Modes, w, h);
        if (mode < 0)
        {
            LOG("psxgpu", "findNearestVideoMode(%u,%u,%u) failed with %u.", &PSX_Video_Modes, w, h, mode);
            return mode;
        }

        Video::VideoResolution vidMode = PSX_Video_Modes.resList[(mode & 0x7F)];
        if (mode & V_RES_MODIFIED)
        {
            LOG("psxgpu", "requested %ux%u, but this is not a valid mode. Using %ux%u instead.", w, h, vidMode.width, vidMode.height);
            if (!result)
                result = V_RES_MODIFIED;
        }

        _screen.res.width   = vidMode.width;
        _screen.res.height  = vidMode.height;
        _screen.refreshRate = (gpuMode == GP1_MODE_NTSC ? 60 : 50); // Refresh rate is constant.

        // Set the resolution. The GPU provides a number of fixed horizontal (256,
        // 320, 368, 512, 640) and vertical (240-256, 480-512) resolutions to pick
        // from, which affect how fast pixels are output and thus how "stretched"
        // the framebuffer will appear.
        GP1HorizontalRes horizontalRes = GP1HorizontalResList[mode & 0x05];
        GP1VerticalRes verticalRes     = GP1VerticalResList[(vidMode.height > 240)]; // Either 256 or 512, no inbetween

        // Set the number of displayed rows and columns. These values are in GPU
        // clock units rather than pixels, thus they are dependent on the selected
        // resolution.
        int offsetX = (w * gp1_clockMultiplierH(horizontalRes)) / 2;
        int offsetY = (h / gp1_clockDividerV(verticalRes)) / 2;

        bool interlace = false;
        if (verticalRes != GP1_VRES_256)
        {
            _useDoubleBuffer = false;
            interlace        = true;
        }

        // Hand all parameters over to the GPU by sending GP1 commands.
        // GP1 commands are sent directly to the register, not through DMA.
        GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
        GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
        GPU_GP1 = gp1_fbMode(
            horizontalRes,
            verticalRes,
            gpuMode,
            interlace,
            GP1_COLOR_16BPP);

        // Marks the screen buffer areas as unavailable for textures and CLUTs
        if (_useDoubleBuffer) h *= 2;

        // The raw framebuffer is larger than the resolution, so the bounding box
        // to mark as in use uses the raw resolution.
        const uint16_t rw = GPURawHorizontalResolution[mode % 5];
        const uint16_t rh = GPURawVerticalResolution[((mode & 0x7F) >= 10)];
        _texmgr.markFrameBuffer(0, 0, rw, rh);

        return result;
    }

    void PSXGPU::fillScreen(Color color)
    {
        if (_dmaOverflow) return;

        GPUNUM(3);
        GPUCMD(gp0_rgb(color.r, color.g, color.b) | gp0_vramFill());
        GPUCMD(gp0_xy(frameX, frameY));
        GPUCMD(gp0_xy(_screen.res.width, _screen.res.height));
    }

    bool PSXGPU::waitingForVSync(void)
    {
        static uint32_t timeout = PSX_GPU_VSYNC_TIMEOUT;
        if (_waitingForVsync)
        {
            if (timeout)
                timeout -= 1;
            else
            {
                LOG("psxgpu", "WARNING: VSync interrupt timeout.");
                timeout = PSX_GPU_VSYNC_TIMEOUT;
                IRQ_MASK |= (1 << IRQ_VSYNC); // Reset the interrupt mask just incase it somehow was cleared
            }
        }
        else if (!_waitingForVsync && timeout < PSX_GPU_VSYNC_TIMEOUT)
        {
            timeout = PSX_GPU_VSYNC_TIMEOUT;
        }
        return _waitingForVsync;
    }

    void PSXGPU::doWaitForVSync()
    {
        while (waitingForVSync()) {}
    }

    void PSXGPU::_swapFrameBuffer()
    {
        // Pick which DMA chain and framebuffer area to use for the new frame.
        // The chain index alternates so we never overwrite a chain that DMA may
        // still be reading from the previous frame.
        chain            = &dmaChains[screenBufferPage];
        screenBufferPage = !screenBufferPage;

        frameX = 0;
        frameY = (screenBufferPage ? _screen.res.height : 0);

        // Reset the chain for building the new frame.
        chain->nextPacket = chain->data;
        _dmaOverflow      = false;

        // Tell the display hardware to show this framebuffer area. The change
        // takes effect on the next VSync, so the frame we are about to draw
        // will be displayed once the scanout catches up.
        GPU_GP1 = gp1_fbOffset(frameX, frameY);

        GPUNUM(4);
        GPUCMD(gp0_texpage(0, true, false));
        GPUCMD(gp0_fbOffset1(frameX, frameY));
        GPUCMD(gp0_fbOffset2(
            frameX + _screen.res.width - 1,
            frameY + _screen.res.height - 1));
        GPUCMD(gp0_fbOrigin(frameX, frameY));
    }

    uint32_t *PSXGPU::_allocatePacket(DMAChain *chain, int numCommands)
    {
        // NOTE: Overflow protection — if the requested allocation would exceed
        // the DMA chain buffer, set the overflow flag and return nullptr. Draw
        // calls become no-ops for the rest of the frame. Warn once per overflow.
        uint32_t *end = &(chain->data)[iPSXDMAListSize];
        if (chain->nextPacket + numCommands + 1 >= end)
        {
            if (!_dmaOverflow)
            {
                LOG("psxgpu", "WARNING: DMA chain overflow, dropping draw calls until next frame.");
                _dmaOverflow = true;
            }
            return nullptr;
        }

        // Grab the current pointer to the next packet then increment it to allocate
        // a new packet. We have to allocate an extra word for the packet's header,
        // which will contain the number of GP0 commands the packet is made up of as
        // well as a pointer to the next packet (or a special "terminator" value to
        // tell the DMA unit to stop).
        uint32_t *ptr = chain->nextPacket;
        chain->nextPacket += numCommands + 1;

        // Write the header and set its pointer to point to the next packet that
        // will be allocated in the buffer.
        *ptr = gp0_tag(numCommands, chain->nextPacket);

        dmaPtrIdx = 0;
        return &ptr[1];
    }

    void PSXGPU::_sendLinkedList(const void *data)
    {
        // Wait until the GPU's DMA unit has finished sending data and is ready.
        _waitForDMADone();

        // Make sure the pointer is aligned to 32 bits (4 bytes). The DMA engine is
        // not capable of reading unaligned data.
        assert(!((uint32_t)data % 4));
        assert(data);

        if (GPU_GP1 & GP1_STAT_FB_INTERLACE)
        {
            for (;;)
            {
                auto status    = GPU_GP1;
                auto drawField = (status / GP1_STAT_DRAW_FIELD_ODD) & 1;
                auto dispField = (status / GP1_STAT_DISP_FIELD_ODD) & 1;

                if (drawField == dispField) continue;
                if (drawField == uint32_t(screenBufferPage)) break;
            }
        }

        // Give DMA a pointer to the beginning of the data and tell it to send it in
        // linked list mode. The DMA unit will start parsing a chain of "packets"
        // from RAM, with each packet being made up of a 32-bit header followed by
        // zero or more 32-bit commands to be sent to the GP0 register.
        DMA_MADR(DMA_GPU) = (uint32_t)data;
        DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_LIST | DMA_CHCR_ENABLE;
    }

    bool PSXGPU::beginRender()
    {
        // On Frame 0, chain is nullptr. After rendering the first frame,
        // we will send on the next call
        if (chain) _sendLinkedList(chain->data);
        _swapFrameBuffer();
        return true;
    }

    bool PSXGPU::endRender()
    {
        *(chain->nextPacket) = gp0_endTag(0);
        _waitingForVsync     = true;

        return true;
    }

    Textures::TextureObject *PSXGPU::createTexture(util::Hash objectID)
    {
        PSXTextureObject *ptObj = new PSXTextureObject(objectID);
        return (ptObj != nullptr ? ptObj : defaultTexture);
    }

    Textures::TextureObject *PSXGPU::createTexture(util::Hash objectID, const char *filePath)
    {
        PSXTextureObject *ptObj = new PSXTextureObject(objectID);
        if (ptObj != nullptr)
        {
            if (ptObj->loadTextureFromFile(filePath) == GV_OK)
                return ptObj;
            else
            {
                delete ptObj;
                ptObj = nullptr;
            }
        }
        return (ptObj != nullptr ? ptObj : defaultTexture);
    }

    int PSXGPU::uploadTexture(Textures::TextureObject *tObj)
    {
        // Apps must always use PSTexture objects from Video::createTexture()
        if (!tObj ||
            tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME ||
            tObj->bpp == 0 ||
            (tObj->height == 0 || tObj->width == 0))
            return V_ERROR(GV_ERR_INVALID_PARAM);

        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        if (ptObj->vramX != 0 || ptObj->vramY != 0)
            return V_ERROR(GV_ERR_BAD_OBJECT);

        _texmgr.allocateTexture(ptObj);

        int w = 0;
        switch (ptObj->bpp)
        {
        case Textures::BPP_4BIT: w = ptObj->width / 4; break;
        case Textures::BPP_8BIT: w = ptObj->width / 2; break;
        case Textures::BPP_16BIT: w = ptObj->width; break;
        default: return V_ERROR(GV_ERR_INVALID_PARAM);
        }

        _waitForDMADone();
        _sendVRAMData(&*ptObj->bitmap, ptObj->bitmapLength,
                      {ptObj->vramX, ptObj->vramY, w, (int)ptObj->height});

        if (ptObj->bpp == Textures::BPP_4BIT ||
            ptObj->bpp == Textures::BPP_8BIT)
        {
            return _uploadPalette(ptObj);
        }
        return GV_OK;
    }

    int PSXGPU::_uploadPalette(PSXTextureObject *ptObj)
    {
        uint16_t clutWidth = TextureManager::clutWidth(ptObj->bpp);

        // TODO: Cleanup?
        uint16_t pA[256] = {0};
        array_from_colors<uint16_t>(
            pA, clutWidth, ptObj->palette,
            ptObj->paletteLength, CT_BGR555);
        _sendVRAMData(&pA, (sizeof(uint16_t) * clutWidth), {ptObj->clutX, ptObj->clutY, clutWidth, 1});
        _waitForDMADone();
        return GV_OK;
    }

    int PSXGPU::releaseTexture(Textures::TextureObject *tObj)
    {
        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        _texmgr.deallocateTexture(ptObj);
        return GV_OK;
    }

    PSXGPU::GraphicsData PSXGPU::allocate(size_t length)
    {
        size_t _l    = getBufferSize(length);
        uint8_t *ptr = new uint8_t[_l];
        memset(ptr, 0, _l);
        return ptr;
    }

    void PSXGPU::_sendVRAMData(const void *data, int length, RectWH r)
    {
        _waitForDMADone();
        assert(!((uint32_t)data % 4) || !(length / 4));

        size_t chunkSize, numChunks;
        uint32_t len32 = length / 4; // To adjust for DMA packets, must be in multiples of 32-bit words

        if (len32 < bPSXDMAChunkSize)
        {
            chunkSize = len32;
            numChunks = 1;
        }
        else
        {
            chunkSize = bPSXDMAChunkSize;
            numChunks = len32 / bPSXDMAChunkSize;

            assert(!(len32 % bPSXDMAChunkSize));
        }

        GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_NONE);

        _waitForGP0Ready();
        GPU_GP0 = gp0_flushCache();
        GPU_GP0 = gp0_vramWrite();
        GPU_GP0 = gp0_xy(r.x, r.y);
        GPU_GP0 = gp0_xy(r.w, r.h);

        GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
        while (!(GPU_GP1 & GP1_STAT_WRITE_READY))
            __asm__ volatile("");

        DMA_MADR(DMA_GPU) = (uint32_t)data;
        DMA_BCR(DMA_GPU)  = chunkSize | (numChunks << 16);
        DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_SLICE | DMA_CHCR_ENABLE;
    }

    void PSXGPU::drawRect(
        int x, int y, int width, int height, Color color)
    {
        GPUNUM(3);
        GPUCMD(color.toBGR888() | gp0_rectangle(false, false, GP0_BLEND_SEMITRANS));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(gp0_xy(width, height));
    }

    void PSXGPU::drawGradientRectH(
        int x, int y, int width, int height, Color left, Color right)
    {
        GPUNUM(8);
        GPUCMD(left.toRGB888() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(right.toRGB888());
        GPUCMD(gp0_xy(x + width, y));
        GPUCMD(left.toRGB888());
        GPUCMD(gp0_xy(x, y + height));
        GPUCMD(right.toRGB888());
        GPUCMD(gp0_xy(x + width, y + height));
    }

    void PSXGPU::drawGradientRectV(
        int x, int y, int width, int height, Color top, Color bottom)
    {
        GPUNUM(8);
        GPUCMD(top.toRGB888() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(top.toRGB888());
        GPUCMD(gp0_xy(x + width, y));
        GPUCMD(bottom.toRGB888());
        GPUCMD(gp0_xy(x, y + height));
        GPUCMD(bottom.toRGB888());
        GPUCMD(gp0_xy(x + width, y + height));
    }

    void PSXGPU::drawGradientRectD(
        int x, int y, int width, int height, Color top, Color middle, Color bottom)
    {
        GPUNUM(8);
        GPUCMD(top.toRGB888() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(middle.toRGB888());
        GPUCMD(gp0_xy(x + width, y));
        GPUCMD(middle.toRGB888());
        GPUCMD(gp0_xy(x, y + height));
        GPUCMD(bottom.toRGB888());
        GPUCMD(gp0_xy(x + width, y + height));
    }

    uint32_t findNearestVideoMode(const VideoModeList *list, uint16_t reqW, uint16_t reqH, uint16_t reqR)
    {
        if (list == nullptr)
            return V_ERROR(V_RES_LIST_INVALID);
        if (list->resLength == 0 || list->resList == nullptr || list->refreshLength == 0 || list->refreshList == nullptr)
            return V_ERROR(V_RES_LIST_INVALID);

        int bestIndex      = -1;
        uint32_t bestScore = 0xFFFFFFFF; // lower score = better fit

        for (uint16_t i = 0; i < list->resLength; i++)
        {
            const VideoResolution &res = list->resList[i];

            // Rule 1: Skip if the resolution is smaller than requested (would crop)
            if (res.width < reqW || res.height < reqH)
                continue;

            // Compute how well it fits: prioritize matching exactly, then closeness
            uint16_t dw    = reqW - res.width;
            uint16_t dh    = reqH - res.height;
            uint32_t score = (uint32_t)dw * dw + (uint32_t)dh * dh;

            // Prefer exact match if found
            if (dw == 0 && dh == 0)
            {
                bestIndex = i;
                bestScore = 0;
                break;
            }

            if (score < bestScore)
            {
                bestScore = score;
                bestIndex = i;
            }
        }

        // If no suitable mode found under or equal to requested, pick the largest available one
        if (bestIndex == -1)
        {
            uint32_t largestArea = 0;
            for (uint16_t i = 0; i < list->resLength; i++)
            {
                const VideoResolution &res = list->resList[i];
                uint32_t area              = (uint32_t)res.width * res.height;
                if (area > largestArea)
                {
                    bestIndex   = i;
                    largestArea = area;
                }
            }
        }

        // Handle refresh rate similarly: pick the closest below or equal, else lowest available
        int bestRefreshIndex     = -1;
        uint16_t bestRefreshDiff = 0xFFFF;
        for (uint16_t i = 0; i < list->refreshLength; i++)
        {
            uint16_t diff = (reqR >= list->refreshList[i]) ? (reqR - list->refreshList[i]) : 0xFFFF;
            if (diff < bestRefreshDiff)
            {
                bestRefreshDiff  = diff;
                bestRefreshIndex = i;
            }
        }

        // If still no refresh found, pick the first
        if (bestRefreshIndex == -1 && list->refreshLength > 0)
            bestRefreshIndex = 0;

        uint32_t result = 0;
        if (bestIndex >= 0)
        {
            result |= bestIndex;
            if (list->resList[bestIndex].width != reqW || list->resList[bestIndex].height != reqH)
                result |= V_RES_MODIFIED;
        }
        else
        {
            return V_ERROR(V_RES_UNSUPPORTED);
        }

        if (bestRefreshIndex >= 0)
        {
            result |= (bestRefreshIndex << 8);
            if (list->refreshList[bestRefreshIndex] != reqR)
                result |= V_REFRESH_MODIFIED;
        }

        return result;
    }

    void PSXGPU::drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint)
    {
        GPUNUM((8 * 3));
        // Left square
        GPUCMD(left.toBGR888() | gp0_shadedQuad(true, false, false));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(left.toBGR888());
        GPUCMD(gp0_xy(x + startPoint, y));
        GPUCMD(left.toBGR888());
        GPUCMD(gp0_xy(x, y + h));
        GPUCMD(left.toBGR888());
        GPUCMD(gp0_xy(x + startPoint, y + h));

        // Gradient square
        GPUCMD(left.toBGR888());
        GPUCMD(gp0_xy(x + startPoint + 1, y));
        GPUCMD(right.toBGR888());
        GPUCMD(gp0_xy(x + endPoint - 1, y));
        GPUCMD(left.toBGR888());
        GPUCMD(gp0_xy(x + startPoint + 1, y + h));
        GPUCMD(right.toBGR888());
        GPUCMD(gp0_xy(x + endPoint - 1, y + h));

        // Right square
        GPUCMD(right.toBGR888() | gp0_shadedQuad(true, false, false));
        GPUCMD(gp0_xy(x + endPoint, y));
        GPUCMD(right.toBGR888());
        GPUCMD(gp0_xy(x + w, y));
        GPUCMD(right.toBGR888());
        GPUCMD(gp0_xy(x + endPoint, y + h));
        GPUCMD(right.toBGR888());
        GPUCMD(gp0_xy(x + w, y + h));
    }

    void PSXGPU::drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint)
    {
        GPUNUM((8 * 3));
        // Top square
        GPUCMD(top.toBGR888() | gp0_shadedQuad(true, false, false));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(top.toBGR888());
        GPUCMD(gp0_xy(x + w, y));
        GPUCMD(top.toBGR888());
        GPUCMD(gp0_xy(x, y + startPoint));
        GPUCMD(top.toBGR888());
        GPUCMD(gp0_xy(x + w, y + startPoint));

        // Gradient square
        GPUCMD(top.toBGR888());
        GPUCMD(gp0_xy(x, y + startPoint + 1));
        GPUCMD(bottom.toBGR888());
        GPUCMD(gp0_xy(x + w, y + startPoint + 1));
        GPUCMD(top.toBGR888());
        GPUCMD(gp0_xy(x, y + endPoint - 1));
        GPUCMD(bottom.toBGR888());
        GPUCMD(gp0_xy(x + w, y + endPoint - 1));

        // bottom square
        GPUCMD(bottom.toBGR888() | gp0_shadedQuad(true, false, false));
        GPUCMD(gp0_xy(x, y + endPoint));
        GPUCMD(bottom.toBGR888());
        GPUCMD(gp0_xy(x + w, y + endPoint));
        GPUCMD(bottom.toBGR888());
        GPUCMD(gp0_xy(x, y + h));
        GPUCMD(bottom.toBGR888());
        GPUCMD(gp0_xy(x + w, y + h));
    }

    void PSXGPU::drawLine(int x1, int y1, int x2, int y2, int width, Color color)
    {
        assert(width);
        switch (width)
        {
        case 0: return;
        case 1:
            GPUNUM(4);
            GPUCMD(color.toBGR888() | gp0_polyLine(false, false));
            GPUCMD(gp0_xy(x1, y1));
            GPUCMD(gp0_xy(x2, y2));
            GPUCMD(gp0_xy(0x5000, 0x5000));
            break;
        default:
        {
            int mid = width / 2;
            GPUNUM(5);
            GPUCMD(color.toBGR888() | gp0_quad(false, false));
            GPUCMD(gp0_xy(x1, y1 - mid));
            GPUCMD(gp0_xy(x2, y2 - mid));
            GPUCMD(gp0_xy(x1, y1 + mid));
            GPUCMD(gp0_xy(x2, y2 + mid));
            break;
        }
        }
    }

    void PSXGPU::drawGradientLine(int x1, int y1, int x2, int y2, int width, Color c1, Color c2)
    {
        assert(width);
        switch (width)
        {
        case 0: return;
        case 1:
            GPUNUM(5);
            GPUCMD(c1.toBGR888() | gp0_polyLine(true, false));
            GPUCMD(gp0_xy(x1, y1));
            GPUCMD(c2.toBGR888());
            GPUCMD(gp0_xy(x2, y2));
            GPUCMD(gp0_xy(0x5000, 0x5000));
            break;
        default:
        {
            int mid = width / 2;
            GPUNUM(8);
            GPUCMD(c1.toBGR888() | gp0_quad(false, false));
            GPUCMD(gp0_xy(x1, y1 - mid));
            GPUCMD(c1.toBGR888());
            GPUCMD(gp0_xy(x2, y2 - mid));
            GPUCMD(c2.toBGR888());
            GPUCMD(gp0_xy(x1, y1 + mid));
            GPUCMD(c2.toBGR888());
            GPUCMD(gp0_xy(x2, y2 + mid));
            break;
        }
        }
    }

    int PSXGPU::drawText(Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h, Color color, uint8_t mode)
    {
        if (!fObj || fObj->getObjectType() != Fonts::GENV_FONT_OBJ_TYPENAME)
            return V_ERROR(GV_ERR_INVALID_PARAM);

        auto *tObj = fObj->getTexture();
        if (!tObj || tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME) return GV_ERR_INVALID_PARAM;
        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        auto &fHeader           = *fObj->getHeader();

        bool clutModified   = false;
        uint16_t foreground = color.toBGR555();
        size_t temp         = 0;

        uint16_t backupClut[2] = {
            ptObj->clutX, ptObj->clutY};

        // If the color isnt the font default of white, then try to pick the colour.
        // Defaults to white if for whatever reason it cannot setup the right colour
        if (foreground != Colors::White.toBGR555())
        {
            // Get the color table, if it exists
            FontColorTable *fcTable = nullptr;
            if (fObj->getParam(PSXFONT_PALETTE_PTR, temp))
            {
                fcTable = (FontColorTable *)temp;
            }
            else
            {
                // Create a Font Color Table and store the pointer to it
                fcTable = new FontColorTable;
                if (fcTable)
                    fObj->setParam(PSXFONT_PALETTE_PTR, (size_t)fcTable);
            }

            if (fcTable)
            {
                FontPaletteEntry *fpEntry = nullptr;
                if (fcTable->find(foreground, &fpEntry))
                {
                    // Print in color
                    ptObj->clutX = fpEntry->vramX;
                    ptObj->clutY = fpEntry->vramY;
                    clutModified = true;
                }
                else
                {
                    // Add and upload color
                    uint16_t _cx = 0, _cy = 0;
                    if (_texmgr.allocateCLUT(Textures::BPP_4BIT, _cx, _cy) == GV_OK)
                    {
                        fcTable->add(_cx, _cy, foreground);

                        uint16_t clut[MAX_COLORS_4BPP] = {0};
                        clut[fHeader.foregroundIndex]  = foreground;
                        clut[fHeader.shadowIndex]      = (Fonts::font_shadow.toBGR555() | (1 << 15));
                        _sendVRAMData(&clut, sizeof(uint16_t) * MAX_COLORS_4BPP, {_cx, _cy, MAX_COLORS_4BPP, 1});
                        _waitForDMADone();

                        ptObj->clutX = _cx;
                        ptObj->clutY = _cy;
                        clutModified = true;
                    }
                }
            }
        }

        // Pre-scan the string to count only renderable glyphs (control
        // characters like \r, \n, \t and spaces don't emit GP0 commands).
        int glyphCount = 0;
        {
            const char *scan = str;
            for (;;)
            {
                auto ch = util::parseUTF8Character(scan);
                if (!ch.length)
                {
                    scan++;
                    continue;
                }
                if (!ch.codePoint) break;
                scan += ch.length;

                if (ch.codePoint != '\t' && ch.codePoint != '\r' &&
                    ch.codePoint != '\n' && ch.codePoint != ' ')
                    glyphCount++;
            }
        }

        // Precompute values that are invariant across all glyphs to avoid
        // recomputing shifts/divisions inside the per-character hot loop.
        const uint32_t rectCmd  = gp0_rgb(255, 255, 255) | gp0_rectangle(true, true, false);
        const uint16_t clutAttr = gp0_clut(ptObj->clutX / 16, ptObj->clutY);
        const uint8_t tpOffsetX = ptObj->tpage.offsetX;
        const uint8_t tpOffsetY = ptObj->tpage.offsetY;
        const uint8_t fontH     = fHeader.fontSize;

        // Each renderable glyph emits 4 GP0 commands, +1 for the texpage command.
        int totalCommands = (4 * glyphCount) + 1;
        int startX        = x;
        int tabWidth      = fHeader.tabWidth;
        int lineTab       = 1; // When reading the TAB character, move to the next tab line, rather than just adding pixels to x
        const char *cur   = str;

        // We only need to send the texture page command once. Also sending it means we potentially arent able to render
        // a fourth glyph (We probably could, but to err on the side of caution)
        bool textPageDone = false;
        while (totalCommands > 0)
        {
            // The PSX GPU FIFO can only have a maximum of 16-word entries.
            // If the string will overload that, break it up into managable chunks to not overflow the FIFO.
            int chunkSize = (totalCommands > PSX_GPU_DMA_FIFO_SIZE) ? PSX_GPU_DMA_FIFO_SIZE : totalCommands;

            // If texture page needs to be added, and this will go over our limit,
            // it must be accounted for by removing a command from the chunk packet
            if (!textPageDone) chunkSize++;
            if (chunkSize > PSX_GPU_DMA_FIFO_SIZE) chunkSize -= 4;

            GPUNUM(chunkSize);

            int emitted = 0;
            if (!textPageDone)
            {
                GPUCMD(gp0_texpage(gp0_page(ptObj->tpage.x, ptObj->tpage.y, GP0_BLEND_ADD, GP0_COLOR_4BPP), true, false));
                textPageDone = true;
                emitted++;
            }

            while (emitted + 4 <= chunkSize)
            {
                auto ch = util::parseUTF8Character(cur);
                if (!ch.length)
                {
                    cur++;
                    continue;
                }
                if (!ch.codePoint) break;
                cur += ch.length;

                switch (ch.codePoint)
                {
                // Non-renderable chars
                case '\t': x = (tabWidth * lineTab); break;
                case '\r':
                    x       = startX;
                    lineTab = 1;
                    break;
                case '\n': y += fontH; break;
                case ' ': x += fHeader.spaceWidth; break;
                default:
                {
                    // Render directly to help with the CPU pipeline
                    auto c = fObj->get(ch.codePoint);
                    GPUCMD(rectCmd);
                    GPUCMD(gp0_xy(x, y));
                    GPUCMD(gp0_uv(tpOffsetX + c.x, tpOffsetY + c.y, clutAttr));
                    GPUCMD(gp0_xy(c.w, fontH));
                    x += c.w;
                    emitted += 4;
                    break;
                }
                }
            }
            // Remove the count of emitted commands from the total count
            totalCommands -= emitted;
        }

        // Restore pre-draw clut values
        if (clutModified)
        {
            ptObj->clutX = backupClut[0];
            ptObj->clutY = backupClut[1];
        }

        return GV_OK;
    }

    int PSXGPU::setDefaultFont(Fonts::FontObject *fObj)
    {
        /*
        if (!fObj) return GV_ERR_INVALID_PARAM;
        auto *tObj = fObj->getTexture();
        if (!tObj || tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME) return GV_ERR_INVALID_PARAM;
        const PSXTextureObject *ptObj = reinterpret_cast<const PSXTextureObject *>(tObj);

        static HaltScreenFont failFont = HaltScreenFont(
            {ptObj->tpage.x, ptObj->tpage.y, ptObj->tpage.offsetX, ptObj->tpage.offsetY},
            {ptObj->vramX, ptObj->vramY},
            {ptObj->clutX, ptObj->clutY},
            fObj->getHeader()->tabWidth,
            fObj->getHeader()->spaceWidth,
            fObj->getHeader()->fontSize,
            fObj->getTable(),
            fObj->getHeader()->numBuckets);

        psx_halt_register_font(&failFont);
        */
        return 0;
    }

    void PSXGPU::drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h)
    {
        auto *tObj = sObj->getTexture();
        if (!tObj || tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME) return; // GV_ERR_INVALID_PARAM;
        const PSXTextureObject *ptObj = reinterpret_cast<const PSXTextureObject *>(tObj);

        GP0ColorDepth colorDepth = GP0_COLOR_16BPP;
        if (sObj->getTexture()->bpp == Textures::BPP_8BIT)
            colorDepth = GP0_COLOR_8BPP;
        else if (sObj->getTexture()->bpp == Textures::BPP_4BIT)
            colorDepth = GP0_COLOR_4BPP;

        GPUNUM(5);
        GPUCMD(gp0_texpage(gp0_page(ptObj->tpage.x, ptObj->tpage.y, GP0_BLEND_ADD, colorDepth), true, false));
        GPUCMD(gp0_rgb(255, 255, 255) | gp0_rectangle(true, true, false));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(gp0_uv(ptObj->tpage.offsetX + w, ptObj->tpage.offsetY + h, gp0_clut((ptObj->clutX / 16), ptObj->clutY)));
        GPUCMD(gp0_xy(w, h));
    }

    int PSXGPU::drawTextureObject(
        const Textures::TextureObject *tObj,
        int x, int y, int w, int h,
        ifloat u1, ifloat v1,
        ifloat u2, ifloat v2)
    {
        if (!tObj || tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME) return GV_ERR_INVALID_PARAM;
        const PSXTextureObject *ptObj = reinterpret_cast<const PSXTextureObject *>(tObj);

        GPUNUM(4);
        GPUCMD(gp0_rgb(255, 255, 255) | gp0_rectangle(true, true, false));
        GPUCMD(gp0_xy(x, y));
        GPUCMD(gp0_uv(ptObj->tpage.offsetX + (size_t)(int)u1, ptObj->tpage.offsetY + (size_t)(int)v1, gp0_clut((ptObj->clutX / 16), ptObj->clutY)));
        GPUCMD(gp0_xy(w, h));
        return GV_OK;
    }

} // namespace System::PSX::GPU