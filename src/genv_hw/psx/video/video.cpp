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

#include "video.hpp"
#include "common/util/hash.hpp"
#include "gpucmd.h"

#include "../registers.hpp"
#include "../system/sys.h"
#include "common/logger/log.hpp"

namespace System::PSX::GPU
{

#define _GPUC (this->*_GPUCMD)
#define _GP0RDY(cmdcount)                              \
    if (!useDMA)                                       \
    {                                                  \
        _waitForGP0Ready();                            \
    }                                                  \
    else                                               \
    {                                                  \
        gpuListPtr = _allocatePacket(chain, cmdcount); \
    }

    PSXGPU::PSXGPU() : _texmgr(PSX::GPU::VRAM_1MIB)
    {
    }

    PSXGPU::PSXGPU(uint8_t vram_size) : _texmgr(vram_size)
    {
    }

    PSXGPU::~PSXGPU()
    {
    }

    void PSXGPU::_directWrite(uint32_t cmd)
    {
        GPU_GP0 = cmd;
    }

    void PSXGPU::_addToDMAList(uint32_t cmd)
    {
        gpuListPtr[dmaPtrIdx++] = cmd;
    }

    bool PSXGPU::init()
    {
        gpuMode = static_cast<GP1VideoMode>(GPU_GP1 & GP1_STAT_FB_MODE_BITMASK);
        GPU_GP1 = gp1_resetGPU();
        GPU_GP1 = gp1_resetFIFO();

        setResolution(320, 240);

        _GP0RDY(4);
        _GPUC(gp0_texpage(0, true, false));
        _GPUC(gp0_fbOffset1(0, 0));
        _GPUC(gp0_fbOffset2(320 - 1, 240 - 1));
        _GPUC(gp0_fbOrigin(0, 0));
        fillScreen(Colors::Blue);
        _swapFrameBuffer();
        fillScreen(Colors::Blue);
        _swapFrameBuffer();
        GPU_GP1 = gp1_fbOffset(0, 0);
        GPU_GP1 = gp1_dispBlank(false);
        // enableDMA(true);
        IRQ_MASK |= 1 << IRQ_VSYNC;

        uploadTexture(defaultTexture);

        return 0;
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

        screen.res.width = vidMode.width;
        screen.res.height = vidMode.height;
        screen.refreshRate = (mode == GP1_MODE_NTSC ? 60 : 50); // Refresh rate is constant.

        // Set the resolution. The GPU provides a number of fixed horizontal (256,
        // 320, 368, 512, 640) and vertical (240-256, 480-512) resolutions to pick
        // from, which affect how fast pixels are output and thus how "stretched"
        // the framebuffer will appear.
        GP1HorizontalRes horizontalRes = GP1HorizontalResList[mode % 5];
        GP1VerticalRes verticalRes = GP1VerticalResList[((mode & 0x7F) >= 10)]; // Either 256 or 512, no inbetween

        // Set the number of displayed rows and columns. These values are in GPU
        // clock units rather than pixels, thus they are dependent on the selected
        // resolution.
        int offsetX = (w * gp1_clockMultiplierH(horizontalRes)) / 2;
        int offsetY = (h / gp1_clockDividerV(verticalRes)) / 2;

        bool interlace = false;
        if (verticalRes != GP1_VRES_256)
        {
            useDoubleBuffer = false;
            interlace = true;
        }

        // Hand all parameters over to the GPU by sending GP1 commands.
        _GP0RDY(3);
        GPU_GP1 = gp1_fbRangeH(x - offsetX, x + offsetX);
        GPU_GP1 = gp1_fbRangeV(y - offsetY, y + offsetY);
        GPU_GP1 = gp1_fbMode(
            horizontalRes,
            verticalRes,
            gpuMode,
            interlace,
            GP1_COLOR_16BPP);

        // Marks the screen buffer areas as unavailable for textures and CLUTs
        if (useDoubleBuffer)
        {
            h *= 2;
        }
        _texmgr.markFrameBuffer(0, 0, w, h);

        return result;
    }

    void PSXGPU::_waitForGP0Ready(void)
    {
        // Block until the GPU reports to be ready to accept commands through its
        // status register (which has the same address as GP1 but is read-only).
        while (!(GPU_GP1 & GP1_STAT_CMD_READY))
            __asm__ volatile("");
    }

    void PSXGPU::_waitForDMADone(void)
    {
        while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
            __asm__ volatile("");
    }

    void PSXGPU::fillScreen(Color color)
    {
        _GP0RDY(3);
        _GPUC(gp0_rgb(color.r, color.g, color.b) | gp0_vramFill());
        _GPUC(gp0_xy(frameX, frameY));
        _GPUC(gp0_xy(screen.res.width, screen.res.height));
    }

    void PSXGPU::_waitForVSync(void)
    {
        uint32_t timeout = 0x00FFFFFF;
        waitingForVsync = true;
        while (waitingForVsync)
        {
            if (timeout)
            {
                if (!waitingForVsync)
                    break;
                timeout -= 1;
            }
            else
            {
                LOG("psxgpu", "WARNING VSync interrupt timeout.");
                timeout = 0x00FFFFFF;
                IRQ_MASK |= IRQ_GPU;
            }
        }
        frameCount++;
    }

    void PSXGPU::_swapFrameBuffer()
    {
        if (useDMA)
        {
            chain = &dmaChains[screenBufferPage];
            chain->nextPacket = chain->data;
        }

        if (useDoubleBuffer)
            screenBufferPage = !screenBufferPage;
        else
            screenBufferPage = 0;

        frameX = 0;
        frameY = (screenBufferPage ? screen.res.height : 0);

        _GP0RDY(4);
        _GPUC(gp0_texpage(0, true, false));
        _GPUC(gp0_fbOrigin(frameX, frameY));
        _GPUC(gp0_fbOffset1(frameX, frameY));
        _GPUC(gp0_fbOffset2(
            frameX + screen.res.width - 1,
            frameY + screen.res.height - 2));
    }

    uint32_t *PSXGPU::_allocatePacket(DMAChain *chain, int numCommands)
    {
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

        // Make sure we haven't yet run out of space for future packets or a linked
        // list terminator, then return a pointer to the packet's first GP0 command.
        assert(chain->nextPacket < &(chain->data)[iPSXDMAListSize]);
        dmaPtrIdx = 0;
        return &ptr[1];
    }

    void PSXGPU::_sendLinkedList(const void *data)
    {
        // Wait until the GPU's DMA unit has finished sending data and is ready.
        while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
            __asm__ volatile("");

        // Make sure the pointer is aligned to 32 bits (4 bytes). The DMA engine is
        // not capable of reading unaligned data.
        assert(!((uint32_t)data % 4));

        if (GPU_GP1 & GP1_STAT_FB_INTERLACE)
        {
            for (;;)
            {
                auto status = GPU_GP1;
                auto drawField = (status / GP1_STAT_DRAW_FIELD_ODD) & 1;
                auto dispField = (status / GP1_STAT_DISP_FIELD_ODD) & 1;

                if (drawField == dispField)
                    continue;
                if (drawField == uint32_t(screenBufferPage))
                    break;
            }
        }

        GPU_GP1 = gp1_fbOffset(frameX, frameY);
        if (useDMA)
            GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);

        // Give DMA a pointer to the beginning of the data and tell it to send it in
        // linked list mode. The DMA unit will start parsing a chain of "packets"
        // from RAM, with each packet being made up of a 32-bit header followed by
        // zero or more 32-bit commands to be sent to the GP0 register.
        DMA_MADR(DMA_GPU) = (uint32_t)data;
        DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_LIST | DMA_CHCR_ENABLE;
    }

    bool PSXGPU::beginRender()
    {
        _swapFrameBuffer();
        return 1;
    }

    bool PSXGPU::endRender()
    {
        if (useDMA)
            *(chain->nextPacket) = gp0_endTag(0);
        _waitForGP0Ready();
        _waitForDMADone();
        _waitForVSync();
        if (useDMA)
            _sendLinkedList(chain->data);
        return 1;
    }

    void PSXGPU::_enableDMA(bool state)
    {
        if (state)
        {
            useDMA = true;
            _GPUCMD = &PSXGPU::_addToDMAList;
            DMA_DPCR |= DMA_DPCR_CH_ENABLE(DMA_GPU);
            GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
        }
        else
        {
            useDMA = false;
            _GPUCMD = &PSXGPU::_directWrite;
            DMA_DPCR = (DMA_DPCR & ~DMA_DPCR_CH_ENABLE(DMA_GPU));
            GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_NONE);
        }
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
            if (ptObj->loadTextureFile(filePath) == Files::FO_OKAY)
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
        if (tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME)
            return GENV_OBJ_INVALID;

        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        if (ptObj->vramX != 0 || ptObj->vramY != 0 || ptObj->texPage != 0)
            return -2;

        _texmgr.allocateTexture(ptObj);

        _waitForDMADone();
        _sendVRAMData(ptObj->bitmap, ptObj->vramX, ptObj->vramY, ptObj->width, ptObj->height);
        if (ptObj->bpp == Textures::BPP_4BIT ||
            ptObj->bpp == Textures::BPP_8BIT)
        {
            _uploadIndexedTexture(tObj);
        }
        return 0;
    }

    int PSXGPU::_uploadIndexedTexture(Textures::TextureObject *tObj)
    {
        // Apps must always use PSTexture objects from Video::createTexture()
        if (tObj->getObjectType() != GENV_PSX_TEXTURE_TYPE_NAME)
            return GENV_OBJ_INVALID;

        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        uint8_t clutWidth = PSX::GPU::bppPxWidth(ptObj->bpp);
        _texmgr.allocateTexture(ptObj);
        _waitForDMADone();
        _sendVRAMData(ptObj->clut, ptObj->vramX, ptObj->vramY, ptObj->width, ptObj->height);
        _waitForDMADone();
        _sendVRAMData(ptObj->clut, ptObj->clutX, ptObj->clutY, clutWidth, 1);
        return 0;
    }

    int PSXGPU::releaseTexture(Textures::TextureObject *tObj)
    {
        PSXTextureObject *ptObj = static_cast<PSXTextureObject *>(tObj);
        _texmgr.deallocateTexture(ptObj);
        return 0;
    }

    void PSXGPU::_sendVRAMData(
        const void *data,
        int x,
        int y,
        int width,
        int height)
    {
        _waitForDMADone();
        assert(!((uint32_t)data % 4));

        size_t length = (width * height) / 2;
        size_t chunkSize, numChunks;

        if (length < bPSXDMAChunkSize)
        {
            chunkSize = length;
            numChunks = 1;
        }
        else
        {
            chunkSize = bPSXDMAChunkSize;
            numChunks = length / bPSXDMAChunkSize;

            assert(!(length % bPSXDMAChunkSize));
        }

        _waitForGP0Ready();
        GPU_GP0 = gp0_flushCache();
        GPU_GP0 = gp0_vramWrite();
        GPU_GP0 = gp0_xy(x, y);
        GPU_GP0 = gp0_xy(width, height);

        GPU_GP1 = gp1_dmaRequestMode(GP1_DREQ_GP0_WRITE);
        while (!(GPU_GP1 & GP1_STAT_WRITE_READY))
            __asm__ volatile("");

        DMA_MADR(DMA_GPU) = (uint32_t)data;
        DMA_BCR(DMA_GPU) = chunkSize | (numChunks << 16);
        DMA_CHCR(DMA_GPU) = 0 | DMA_CHCR_WRITE | DMA_CHCR_MODE_SLICE | DMA_CHCR_ENABLE;
    }

    /*void PSXGPU::setBlendMode(BlendMode blendMode, bool dither)
    {
        uint16_t page = _lastTexpage & ~gp0_texpage(
                                           gp0_page(0, 0, GP0_BLEND_BITMASK, GP0_COLOR_4BPP), true, true);
        page |= gp0_page(0, 0, blendMode, GP0_COLOR_4BPP);

        setTexturePage(page, dither);
    }*/

    void PSXGPU::drawRect(
        int x, int y, int width, int height, Color color)
    {
        _GP0RDY(3);
        _GPUC(color.toARGB() | gp0_rectangle(false, false, GP0_BLEND_SEMITRANS));
        _GPUC(gp0_xy(x, y));
        _GPUC(gp0_xy(width, height));
    }

    void PSXGPU::drawGradientRectH(
        int x, int y, int width, int height, Color left, Color right)
    {
        _GP0RDY(8);
        _GPUC(left.toARGB() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        _GPUC(gp0_xy(x, y));
        _GPUC(right.toARGB());
        _GPUC(gp0_xy(x + width, y));
        _GPUC(left.toARGB());
        _GPUC(gp0_xy(x, y + height));
        _GPUC(right.toARGB());
        _GPUC(gp0_xy(x + width, y + height));
    }

    void PSXGPU::drawGradientRectV(
        int x, int y, int width, int height, Color top, Color bottom)
    {
        _GP0RDY(8);
        _GPUC(top.toARGB() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        _GPUC(gp0_xy(x, y));
        _GPUC(top.toARGB());
        _GPUC(gp0_xy(x + width, y));
        _GPUC(bottom.toARGB());
        _GPUC(gp0_xy(x, y + height));
        _GPUC(bottom.toARGB());
        _GPUC(gp0_xy(x + width, y + height));
    }

    void PSXGPU::drawGradientRectD(
        int x, int y, int width, int height, Color top, Color middle, Color bottom)
    {
        _GP0RDY(8);
        _GPUC(top.toARGB() | gp0_shadedQuad(true, false, GP0_BLEND_SEMITRANS));
        _GPUC(gp0_xy(x, y));
        _GPUC(middle.toARGB());
        _GPUC(gp0_xy(x + width, y));
        _GPUC(middle.toARGB());
        _GPUC(gp0_xy(x, y + height));
        _GPUC(bottom.toARGB());
        _GPUC(gp0_xy(x + width, y + height));
    }

    uint32_t findNearestVideoMode(const VideoModeList *list, uint16_t reqW, uint16_t reqH, uint16_t reqR)
    {
        if (list == nullptr)
            return V_RES_LIST_INVALID;
        if (list->resLength == 0 || list->resList == nullptr || list->refreshLength == 0 || list->refreshList == nullptr)
            return V_RES_LIST_INVALID;

        int bestIndex = -1;
        uint32_t bestScore = 0xFFFFFFFF; // lower score = better fit

        for (uint16_t i = 0; i < list->resLength; i++)
        {
            const VideoResolution &res = list->resList[i];

            // Rule 1: Skip if the resolution is smaller than requested (would crop)
            if (res.width < reqW || res.height < reqH)
                continue;

            // Compute how well it fits: prioritize matching exactly, then closeness
            uint16_t dw = reqW - res.width;
            uint16_t dh = reqH - res.height;
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
                uint32_t area = (uint32_t)res.width * res.height;
                if (area > largestArea)
                {
                    bestIndex = i;
                    largestArea = area;
                }
            }
        }

        // Handle refresh rate similarly: pick the closest below or equal, else lowest available
        int bestRefreshIndex = -1;
        uint16_t bestRefreshDiff = 0xFFFF;
        for (uint16_t i = 0; i < list->refreshLength; i++)
        {
            uint16_t diff = (reqR >= list->refreshList[i]) ? (reqR - list->refreshList[i]) : 0xFFFF;
            if (diff < bestRefreshDiff)
            {
                bestRefreshDiff = diff;
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
            return V_RES_UNSUPPORTED;
        }

        if (bestRefreshIndex >= 0)
        {
            result |= (bestRefreshIndex << 8);
            if (list->refreshList[bestRefreshIndex] != reqR)
                result |= V_REFRESH_MODIFIED;
        }

        return result;
    }
} // namespace System::PSX::GPU