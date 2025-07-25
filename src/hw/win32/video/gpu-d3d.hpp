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
#include "common/objects/tile.hpp"
#include "common/objects/sprite.hpp"
#include "video.hpp"
#include <windows.h>
#include <cstdio>
#include <vector>
#include <unordered_map>

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

class D3DTexture : public Textures::TextureObject
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
    WindowObject *gpuWnd = nullptr;

    LPDIRECT3D9 d3d = nullptr;
    LPDIRECT3DDEVICE9 d3ddev = nullptr;
    LPD3DXFONT d3dfont = nullptr;
    LPDIRECT3DSURFACE9 backBuffer = nullptr;
    LPDIRECT3DVERTEXBUFFER9 vbo = nullptr;

    std::unordered_map<uint32_t, LPDIRECT3DTEXTURE9> d3dTexMap;
    uint32_t nextTextureID = 1; // start from 1 to reserve 0 for "invalid"

    LPDIRECT3DTEXTURE9 getTexBuffer(Textures::TextureObject *tObj);

    D3DAdaptor *adaptorList = nullptr;

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
        public:
        D3DSpriteVertex(){}
        D3DSpriteVertex(Vertex v) : x(v.pos.x), y(v.pos.y), z(v.pos.z), rhw(v.rhw), u(v.tex.u), v(v.tex.v){}
        D3DSpriteVertex(float _x, float _y, float _z, float _r, float _u, float _v) :
            x(_x), y(_y), z(_z), rhw(_r), u(_u), v(_v){}
        float x, y, z, rhw;
        float u, v; // texture coordinates
        static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1;
    };

public:
    DirectXGPU(WindowObject *wObj);
    ~DirectXGPU();

    bool setWindow(WindowObject *wObj) override;

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

    void drawText(const char *str, int len, int x, int y, int w, int h, Color color, uint8_t mode = TALIGN_LEFT) override;

    void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) override;
    void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) override;

    int drawTextureObject(
        Textures::TextureObject *tObj,
        int x, int y, int w, int h,
        ifloat u1, ifloat v1,
        ifloat u2, ifloat v2);
    
        int drawTextureObject(
            Textures::TextureObject *tObj,
            int x, int y,
            Vertex v[]);

    void releaseResources();

    int uploadTexture(Textures::TextureObject *tObj);
    int releaseTexture(Textures::TextureObject *tObj);

    // Direct3D Specific
    LPDIRECT3DDEVICE9 getDirectXDevice() { return d3ddev; }
};
