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

// Not thread safe, but probably not *too* much of a problem.
#pragma once

#include <d3d9.h>
#include <d3dx9.h>

#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "video.hpp"
#include <windows.h>
#include <cstdio>
#include <vector>

struct D3DAdaptor
{
    UINT uiAdaptorIdx;
    D3DADAPTER_IDENTIFIER9 d3dIdentifier;
    D3DCAPS9 d3dCapabilities;
    HMONITOR hMon;
    D3DDISPLAYMODE d3dMode;

    const char *szMonitorName = nullptr;

    ~D3DAdaptor()
    {
        if (szMonitorName)
            delete szMonitorName;
    }
};

class D3DTexture : public TextureObject
{
private:
    LPDIRECT3DDEVICE9 d3dDevice;
    LPDIRECT3DTEXTURE9 texture;

public:
    D3DTexture(LPDIRECT3DDEVICE9 dev);
    ~D3DTexture();

    void loadTextureFromMem(void *data, int len);
    void loadTexture(const char *path);
};

class DirectXGPU : public IWinVideo
{
private:
    LPDIRECT3D9 d3d;
    LPDIRECT3DDEVICE9 d3ddev;
    LPD3DXFONT d3dfont;
    LPDIRECT3DTEXTURE9 d3dtexture;
    LPDIRECT3DSURFACE9 backBuffer;
    LPDIRECT3DVERTEXBUFFER9 vbo;

    WindowObject *gpuWnd;

    D3DAdaptor *adaptorList;

    inline RECT rectangle(int x, int y, int w, int h)
    {
        return RECT(x, y, x + w, y + h);
    }

    struct D3DVertex
    {
        float x, y, z, rhw; // Position (rhw = reciprocal of homogeneous w)
        DWORD color;        // Color (ARGB)

        static constexpr DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
    };

    struct D3DSpriteVertex
    {
        float x, y, z, rhw;
        float u, v; // texture coordinates
        static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
    };

public:
    DirectXGPU();
    ~DirectXGPU();

    void setWindow(WindowObject *wObj) override;

    bool init() override;
    bool reset() override;
    bool beginRender() override;
    bool endRender() override;
    bool shutdown() override;

    int getSupportedResolutions(VideoModeList &list);
    int setResolution(int w, int h, bool updateWindow = true) override;
    bool setFullscreen(FullscreenMode mode, int w = 0, int h = 0) override;
    inline FullscreenMode getFullscreenMode() override { return gpuWnd->fsMode; }

    void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const override {}

    void drawLine(int x1, int y1, int x2, int y2, int width, Color color);

    void drawRect(int x, int y, int w, int h, Color c) override;
    void drawGradientRect(int x, int y, int w, int h, GPUGradientMode m) override {}
    void drawGradientRectH(int x, int y, int w, int h, Color left, Color right) override;
    void drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom) override;
    void drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom) override;
    void drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint) override;
    void drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint) override;

    void drawText(const char *str, int len, int x, int y, int w, int h, Color color);

    void drawSpriteObject(SpriteObject *obj);

    void releaseResources();

    // Direct3D Specific
    LPDIRECT3DDEVICE9 getDirectXDevice() { return d3ddev; }
};
