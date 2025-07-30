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

#include "common/services/video/video.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"
#include "gpucmd.h"

using namespace Video;

#define LOG_GPU(fmt, ...) LOG("PSXGPU", fmt __VA_OPT__(, ) __VA_ARGS__)

namespace Video
{
    constexpr const VideoResolution PSX_Resolutions[] = {
        {"PSX 256x240p", 256, 240, AspectRatio::R16_15, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 320x240p", 320, 240, AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 368x240p", 368, 240, AspectRatio::R4_3, PORTABLE | SDTV | PROGRESSIVE},
        {"PSX 512x240p", 512, 240, AspectRatio::R16_9, PORTABLE | SDTV | PROGRESSIVE | WIDESCREEN},
        {"PSX 640x240p", 640, 240, AspectRatio::R16_9, PORTABLE | SDTV | PROGRESSIVE | WIDESCREEN},
        {"PSX 256x240i", 256, 240, AspectRatio::R16_15, PORTABLE | SDTV | INTERLACED},
        {"PSX 320x240i", 320, 240, AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 368x240i", 368, 240, AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 512x240i", 512, 240, AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED | WIDESCREEN},
        {"PSX 640x240i", 640, 240, AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED | WIDESCREEN},
        {"PSX 256x480i", 256, 480, AspectRatio::R16_15, PORTABLE | SDTV | INTERLACED},
        {"PSX 320x480i", 320, 480, AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 368x480i", 368, 480, AspectRatio::R4_3, PORTABLE | SDTV | INTERLACED},
        {"PSX 512x480i", 512, 480, AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED},
        {"PSX 640x480i", 640, 480, AspectRatio::R16_9, PORTABLE | SDTV | INTERLACED}};

    constexpr const uint16_t PSX_Refresh_Rates[2] = {50, 60};

    constexpr const VideoModeList PSX_Video_Modes = {
        .resLength = (sizeof(PSX_Resolutions) / sizeof(VideoResolution)),
        .resList = PSX_Resolutions,
        .refreshLength = 2,
        .refreshList = PSX_Refresh_Rates};

    constexpr const int iPSXDMAListSize = 1024;
    constexpr const uint8_t bPSXDMAChunkSize = 16;
}

class PSXGPU : public IVideo
{
private:
    typedef struct
    {
        uint32_t data[Video::iPSXDMAListSize];
        uint32_t *nextPacket;
    } DMAChain;

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
    uint32_t *allocatePacket(DMAChain *chain, int numCommands);
    uint32_t *gpuListPtr = nullptr;
    DMAChain *chain = nullptr;

    void addToDMAList(uint32_t cmd);
    void directWrite(uint32_t cmd);
    void (PSXGPU::*GPUCMD)(uint32_t) = &PSXGPU::directWrite;
    void enableDMA(bool state);

    int PSXGPU::uploadTexture(
        Textures::TextureObject *tObj,
        int x,
        int y
    );

public:
    PSXGPU();
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
