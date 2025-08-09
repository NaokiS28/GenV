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

#include "common/services/video/iface_video.hpp"
#include "common/util/log.hpp"

#include "texmgr.hpp"
#include "gpudef.hpp"
#include "psxtex.hpp"

#define LOG_GPU(fmt, ...) LOG("PSXGPU", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace System::PSX::GPU
{
    using namespace Video;

    class PSXGPU : public IVideo
    {
    protected:
        typedef struct
        {
            uint32_t data[iPSXDMAListSize];
            uint32_t *nextPacket;
        } DMAChain;

        TextureManager _texmgr;

        bool screenBufferPage = 0;
        uint16_t dmaPtrIdx = 0;
        DMAChain dmaChains[2];
        bool useDMA = false;
        GP1VideoMode gpuMode = GP1_MODE_NTSC;

        int frameX = 0;
        int frameY = 0;

        void waitForGP0Ready(void);
        void waitForVSync(void);
        void waitForDMADone(void);

        void swapFrameBuffer();
        void sendLinkedList(const void *data);

        uint32_t *gpuListPtr = nullptr;
        DMAChain *chain = nullptr;
        uint32_t *allocatePacket(DMAChain *chain, int numCommands);

        void enableDMA(bool state);

        void directWrite(uint32_t cmd);
        void addToDMAList(uint32_t cmd);
        void (PSXGPU::*GPUCMD)(uint32_t) = &PSXGPU::directWrite;

        void sendVRAMData(
            const void *data,
            int x,
            int y,
            int width,
            int height);

        int uploadIndexedTexture(Textures::TextureObject *tObj);

    public:
        PSXGPU();
        PSXGPU(uint8_t vram_size);
        ~PSXGPU() override;

        bool init() override;
        bool reset() override { return false; }
        bool beginRender() override;
        bool endRender() override;
        bool shutdown() override { return 0; }

        const VideoModeList *getSupportedResolutions()
        {
            return &PSX_Video_Modes;
        }

        int setResolution(int w, int h, bool updateWindow = true) override;
        bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0) override
        {
            return true;
        }

        void fillScreen(Color color) override;

        void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const override {
        };

        void drawLine(int x1, int y1, int x2, int y2, int width, Color color) override
        {
        }

        void drawRect(int x, int y, int w, int h, Color c) override
        {
        }
        void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) override
        {
        }
        void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) override
        {
        }
        void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) override
        {
        }

        void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) override
        {
        }
        void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) override
        {
        }
        void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) override
        {
        }
        void drawText(const char *str, int len, int x, int y, int w, int h, Color color, uint8_t mode) override
        {
        }

        Textures::TextureObject *createTexture();
        Textures::TextureObject *createTexture(const char *filePath);
        int uploadTexture(Textures::TextureObject *tObj) override;
        int releaseTexture(Textures::TextureObject *tObj) override;

        void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) override
        {
        }

        void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) override
        {
        }

        int drawTextureObject(
            Textures::TextureObject *tObj,
            int x, int y, int w, int h,
            ifloat u1, ifloat v1,
            ifloat u2, ifloat v2) override
        {
            return 0;
        }

        int drawTextureObject(
            Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]) override
        {
            return 0;
        }
    };
}