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

#pragma once

#include <stdint.h>

#include "common/services/video/screen.hpp"
#include "common/services/video/vesa.hpp"
#include "psx/common/system/registers.h"
#include "texmgr.hpp"
#include "resolutions.hpp"
#include "psxtex.hpp"
#include "gpucmd.hpp"

#include "common/util/rect.hpp"
#include "common/objects/font.hpp"
#include "common/services/services.hpp"
#include "common/services/video/basevideo.hpp"

/*

*/

namespace PS1
{
    class BasePS1System;
}

namespace PS1::GPU
{
    using namespace Video;

    uint32_t findNearestVideoMode(const VideoModeList *list, uint16_t w, uint16_t h, uint16_t r = 60);

    class PS1GPU : public Video::BaseVideoDriver
    {
        friend class PS1::BasePS1System;

    protected:
        typedef struct DMAChain
        {
            uint32_t data[iPS1DMAListSize] = {gp0_endTag(0)};
            uint32_t *nextPacket           = nullptr;
        } DMAChain;

        bool screenBufferPage = 0;
        uint16_t dmaPtrIdx    = 0;
        DMAChain dmaChains[2];

        Screen _screen;
        VideoResolution _res;

        GP1VRAMSize vramSize = GP1_VRAM_1MB;
        GP1VideoMode gpuMode = GP1_MODE_NTSC;
        TextureManager _texmgr;

        int frameX = 0;
        int frameY = 0;

        inline void __attribute__((always_inline)) _waitForGP0Ready(void)
        {
            while (!(GPU_GP1 & GP1_STAT_CMD_READY))
                __asm__ volatile("");
        }
        inline void __attribute__((always_inline)) _waitForDMADone(void)
        {
            while (DMA_CHCR(DMA_GPU) & DMA_CHCR_ENABLE)
                __asm__ volatile("");
        }
        volatile bool _waitingForVsync = false;

        void _swapFrameBuffer();
        void _sendLinkedList(const void *data);

        DMAChain *chain      = nullptr;
        uint32_t *gpuListPtr = nullptr;
        // NOTE: Set true when the DMA chain is full. Draw calls become no-ops
        // until the next frame when _swapFrameBuffer resets it.
        bool _dmaOverflow = false;
        uint32_t *_allocatePacket(DMAChain *chain, int numCommands);

        void _enableDMA(bool state);

        void _sendVRAMData(const void *data, int length, RectWH);
        int _uploadPalette(PS1TextureObject *ptObj);

    public:
        PS1GPU(System::ISystem &sys);
        PS1GPU(System::ISystem &sys, GP1VRAMSize vram_size);
        ~PS1GPU() override = default;

        int init() override;
        bool reset() override
        {
            return false;
        }
        bool update() override { return true; }
        bool beginRender(Screen &screen) override;
        bool endRender(Screen &screen) override;
        void shutdown() override
        {
        }

        bool waitingForVSync() override;
        void doWaitForVSync() override;

        const VESA::VideoModeList *getSupportedResolutions(Video::Screen &screen) override { return &PS1_Video_Modes; }

        // PS1 buffers to DMA must be aligned to the chunk size and be null terminated
        inline size_t getBufferSize(size_t length) override
        {
            constexpr const int DMABytesPerChunk = (bPS1DMAChunkSize * sizeof(uint32_t)) - 1;
            return (length + DMABytesPerChunk) & ~DMABytesPerChunk;
        }
        GraphicsData allocate(size_t length) override;

        const VideoModeList *getSupportedResolutions()
        {
            return &PS1_Video_Modes;
        }

        inline int setResolution(Screen &screen, VideoResolution mode, bool updateWindow = true) override
        {
            return setResolution(screen, mode.width, mode.height, updateWindow);
        }
        int setResolution(Screen &screen, int w, int h, bool updateWindow = true) override;
        bool setFullscreen(Screen &screen, System::FullscreenMode mode, int w = 0, int h = 0) override
        {
            return false;
        }

        void fillScreen(Screen &screen, Color color);

        void drawAlpha(Screen &screen, int x, int y, int w, int h, int sx, int sy, uint8_t a) const override {
        };

        void drawLine(Screen &screen, int x1, int y1, int x2, int y2, int width, Color color) override;
        void drawGradientLine(Screen &screen, int x1, int y1, int x2, int y2, int width, Color c1, Color c2) override;

        void drawRect(Screen &screen, int x, int y, int w, int h, Color c) override;
        void drawGradientRectH(Screen &screen, int x, int y, int w, int h, Color left, Color right) override;
        void drawGradientRectV(Screen &screen, int x, int y, int w, int h, Color top, Color bottom) override;
        void drawGradientRectD(Screen &screen, int x, int y, int w, int h, Color top, Color middle, Color bottom) override;

        void drawGradientRect(Screen &screen, int x, int y, int w, int h, GPUGradientMode m) override {};
        void drawGradientRectHVar(Screen &screen, int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) override;
        void drawGradientRectVVar(Screen &screen, int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) override;

        inline int drawText(Screen &screen, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) override
        {
            Fonts::FontObject *fObj = getServiceManager()->getFontManager()->getCurrentFont();
            return drawText(screen, fObj, str, x, y, w, h, color, mode);
        }
        int drawText(Screen &screen, Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h, Color color = Colors::White, uint8_t mode = TALIGN_LEFT) override;

        int setDefaultFont(Fonts::FontObject *fObj) override;

        Textures::TextureObject *createTexture(util::Hash objectID) override;
        Textures::TextureObject *createTexture(util::Hash objectID, const char *filePath) override;
        int uploadTexture(Textures::TextureObject *tObj) override;
        int releaseTexture(Textures::TextureObject *tObj) override;

        void drawSpriteObject(Screen &screen, Sprites::SpriteObject *sObj, int x, int y, int w, int h) override;
        void drawTileObject(Screen &screen, Sprites::TileObject *sObj, int x, int y, int w, int h) override
        {
        }

        int drawTextureObject(
            Screen &screen,
            const Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2) override;

        int drawTextureObject(
            Screen &screen,
            const Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]) override
        {
            return 0;
        }
    };
} // namespace PS1::GPU