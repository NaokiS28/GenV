/*
 * DXUX - Copyright (C) 2025 NaokiS, spicyjpeg
 * gpu-d3d.cpp - Created on 24-04-2025
 *
 * DXUX is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * DXUX is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * DXUX. If not, see <https://www.gnu.org/licenses/>.
 */

#include "gpu-d3d.hpp"
#include "vidutil.hpp"

D3DTexture::D3DTexture(LPDIRECT3DDEVICE9 dev)
{
    d3dDevice = dev;
}

D3DTexture::~D3DTexture()
{
    if (texture)
    {
        texture->Release();
    }
}

void D3DTexture::loadTextureFromMem(void *data, int len)
{
    if (!data || len < 1)
        return;
    // D3DXCreateTexture(d3dDevice)
}

void D3DTexture::loadTexture(const char *path)
{
    if (!path)
        return;
    D3DXCreateTextureFromFileExA(
        d3dDevice,
        path,
        D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0,
        D3DFMT_A8R8G8B8,
        D3DPOOL_MANAGED,
        D3DX_DEFAULT, D3DX_DEFAULT,
        0, nullptr, nullptr, &texture);
}

DirectXGPU::DirectXGPU()
{
    d3d = nullptr;
    d3ddev = nullptr;
    d3dfont = nullptr;
    d3dtexture = nullptr;
    backBuffer = nullptr;
    vbo = nullptr;
    gpuWnd = nullptr;
    adaptorList = nullptr;
}

bool DirectXGPU::init()
{
    if (d3d)
        return true;
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    UINT iAdapators = 0;
    iAdapators = d3d->GetAdapterCount();
    adaptorList = new D3DAdaptor[iAdapators];
    if(!adaptorList) return false;

    for(UINT idx = 0; idx < iAdapators; idx++){
        MONITORINFOEX MonitorInfo;
        ZeroMemory(&MonitorInfo, sizeof(MonitorInfo));
        ZeroMemory(&adaptorList[idx], sizeof(D3DAdaptor));
        MonitorInfo.cbSize = sizeof(MonitorInfo);
        
        adaptorList[idx].uiAdaptorIdx = idx;
        adaptorList[idx].hMon = d3d->GetAdapterMonitor(idx);
        d3d->GetAdapterIdentifier(idx, 0, &adaptorList[idx].d3dIdentifier);
        d3d->GetAdapterDisplayMode(idx, &adaptorList[idx].d3dMode);
        d3d->GetDeviceCaps(idx, D3DDEVTYPE_HAL, &adaptorList[idx].d3dCapabilities);
        GetMonitorInfo(adaptorList[idx].hMon, &MonitorInfo);   
        adaptorList[idx].szMonitorName = GetDisplayName(MonitorInfo.szDevice, 0);
    }
    
    return d3d != nullptr;
}

void DirectXGPU::setWindow(WindowObject *wObj)
{
    if (!wObj)
        return;

    gpuWnd = wObj;

    D3DPRESENT_PARAMETERS d3dpp = {};
    ZeroMemory(&d3dpp, sizeof(d3dpp));

    d3dpp.Windowed = (wObj->fsMode != Video::Fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;

    if (d3ddev)
    {
        releaseResources();
        d3ddev->Release();
    }

    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, gpuWnd->hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &d3ddev);

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3ddev->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3ddev->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3ddev->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    // d3ddev->SetRenderState(D3DRS_LIGHTING, FALSE);

    // Create a simple font
    D3DXCreateFont(d3ddev, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   TEXT("Unispace"), &d3dfont);
}

int DirectXGPU::getSupportedResolutions(VideoModeList &list){

}

int DirectXGPU::setResolution(int w, int h, bool updateWindow)
{
    if (gpuWnd->fsMode != Video::Windowed)
        return setFullscreen(gpuWnd->fsMode, w, h);

    this->IVideo::setResolution(w, h, updateWindow);
    gpuWnd->Resize(w, h);
    D3DXMATRIX ortho;
    D3DXMatrixOrthoOffCenterLH(&ortho, 0, this->getHorizontalRes(), this->getVerticalRes(), 0, 0, 1);
    d3ddev->SetTransform(D3DTS_PROJECTION, &ortho);
    return 0;
}

bool DirectXGPU::setFullscreen(FullscreenMode mode, int w, int h)
{
    HRESULT hr;

    gpuWnd->SetWindowMode(mode, w, h, 0, 0);
    D3DPRESENT_PARAMETERS d3dpp = {};
    d3dpp.Windowed = (mode != Video::Fullscreen);
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = D3DFMT_X8B8G8R8;
    d3dpp.BackBufferWidth = w;
    d3dpp.BackBufferHeight = h;
    d3dpp.hDeviceWindow = gpuWnd->hWnd;
    releaseResources();
    hr = d3ddev->Reset(&d3dpp);
    D3DXCreateFont(d3ddev, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET,
                   OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                   TEXT("Unispace"), &d3dfont);

    return SUCCEEDED(hr);
}

bool DirectXGPU::setFullscreen(FullscreenMode mode)
{
    HRESULT hr;
    if (gpuWnd->fsMode != mode)
    {
        gpuWnd->SetWindowMode(mode, this->getHorizontalRes(), this->getVerticalRes());
        D3DPRESENT_PARAMETERS d3dpp = {};
        d3dpp.Windowed = (mode != Video::Fullscreen);
        d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        d3dpp.BackBufferFormat = D3DFMT_X8B8G8R8;
        if(mode == Video::Fullscreen){
            d3dpp.BackBufferWidth = this->getHorizontalRes();
            d3dpp.BackBufferHeight = this->getVerticalRes();
            d3dpp.FullScreen_RefreshRateInHz = 60;
        }
        d3dpp.hDeviceWindow = gpuWnd->hWnd;
        releaseResources();
        hr = d3ddev->Reset(&d3dpp);
        D3DXCreateFont(d3ddev, 20, 0, 0, 1, FALSE, DEFAULT_CHARSET,
                       OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                       TEXT("Unispace"), &d3dfont);
    }
    return SUCCEEDED(hr);
}

void DirectXGPU::releaseResources()
{
    if(d3dfont) {
        d3dfont->Release();
        d3dfont = nullptr;
    }
    // vbo->Release();
}

bool DirectXGPU::reset()
{
    releaseResources();
    if(backBuffer){
        backBuffer->Release();
        backBuffer = nullptr;
    }
    if(d3ddev) {
        d3ddev->Release();
        d3ddev = nullptr;
    }
    if(d3d){
        d3d->Release();
        d3d = nullptr;
    }
    return 0;
}

bool DirectXGPU::beginRender()
{
    if (!d3ddev)
        return false;

    d3ddev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 128), 1.0f, 0);

    if (FAILED(d3ddev->BeginScene()))
    {
        return false;
    }

    d3ddev->GetRenderTarget(0, &backBuffer);
    // if (SUCCEEDED(hr) && backBuffer)
    //{
    //  d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);

    return true;
    //}
    // return 1;
}

void DirectXGPU::drawLine(int x1, int y1, int x2, int y2, int width, Color color)
{
    int midPoint;

    if (width < 2)
    {
        width = 1;
        midPoint = 1;
    }
    else
    {
        midPoint = width / 2;
    }

    // This will probably break and not render if the user asks for a vertical line or if x2/y2 is less than x1/y1.
    // Direct3D wants to render left to right, then down. Top to bottom, then right or similar will cause nothing to be drawn.

    D3DVertex vertices[] = {
        {(float)x1, (float)(width > 1 ? y1 - midPoint : y1), 0.0f, 1.0f, color.toARGB()}, // Top-left
        {(float)x2, (float)(width > 1 ? y2 - midPoint : y2), 0.0f, 1.0f, color.toARGB()}, // Bottom-left
        {(float)x1, (float)y1 + midPoint, 0.0f, 1.0f, color.toARGB()},                    // Top-right
        {(float)x2, (float)y2 + midPoint, 0.0f, 1.0f, color.toARGB()},                    // Bottom-right
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawRect(int x, int y, int w, int h, Color c)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        {(float)x, (float)y, 0.0f, 1.0f, c.toARGB()},             // Top-left
        {(float)(x + w), (float)y, 0.0f, 1.0f, c.toARGB()},       // Top-right
        {(float)x, (float)(y + h), 0.0f, 1.0f, c.toARGB()},       // Bottom-left
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, c.toARGB()}, // Bottom-right
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawGradientRectH(int x, int y, int w, int h, Color left, Color right)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        {(float)x, (float)y, 0.0f, 1.0f, left.toARGB()},              // Top-left
        {(float)(x + w), (float)y, 0.0f, 1.0f, right.toARGB()},       // Top-right
        {(float)x, (float)(y + h), 0.0f, 1.0f, left.toARGB()},        // Bottom-left
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, right.toARGB()}, // Bottom-right
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawGradientRectV(int x, int y, int w, int h, Color top, Color bottom)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        {(float)x, (float)y, 0.0f, 1.0f, top.toARGB()},                // Top-left
        {(float)(x + w), (float)y, 0.0f, 1.0f, top.toARGB()},          // Top-right
        {(float)x, (float)(y + h), 0.0f, 1.0f, bottom.toARGB()},       // Bottom-left
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, bottom.toARGB()}, // Bottom-right
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawGradientRectD(int x, int y, int w, int h, Color top, Color middle, Color bottom)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        {(float)x, (float)y, 0.0f, 1.0f, top.toARGB()},                      // Top-left
        {(float)(x + w), (float)y, 0.0f, 1.0f, top.toARGB()},                // Top-right
        {(float)x, (float)(y + (h / 2)), 0.0f, 1.0f, middle.toARGB()},       // Middle-left
        {(float)(x + w), (float)(y + (h / 2)), 0.0f, 1.0f, middle.toARGB()}, // Middle-right
        {(float)x, (float)(y + h), 0.0f, 1.0f, bottom.toARGB()},             // Bottom-left
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, bottom.toARGB()},       // Bottom-right
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 4, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawGradientRectHVar(int x, int y, int w, int h, Color left, Color right, int startPoint, int endPoint)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        // Left segment
        {(float)x, (float)(y + h), 0.0f, 1.0f, left.toARGB()},
        {(float)x, (float)y, 0.0f, 1.0f, left.toARGB()},

        // Gradient segment
        {(float)(x + startPoint), (float)(y + h), 0.0f, 1.0f, left.toARGB()},
        {(float)(x + startPoint), (float)y, 0.0f, 1.0f, left.toARGB()},

        {(float)(x + endPoint), (float)(y + h), 0.0f, 1.0f, right.toARGB()},
        {(float)(x + endPoint), (float)y, 0.0f, 1.0f, right.toARGB()},

        // Right segment
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, right.toARGB()},
        {(float)(x + w), (float)y, 0.0f, 1.0f, right.toARGB()}};

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 6, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawGradientRectVVar(int x, int y, int w, int h, Color top, Color bottom, int startPoint, int endPoint)
{
    if (!d3d || !d3ddev)
        return;

    D3DVertex vertices[] = {
        // Top segment
        {(float)x, (float)y, 0.0f, 1.0f, top.toARGB()},
        {(float)(x + w), (float)y, 0.0f, 1.0f, top.toARGB()},

        // Gradient segment
        {(float)x, (float)(y + startPoint), 0.0f, 1.0f, top.toARGB()},
        {(float)(x + w), (float)(y + startPoint), 0.0f, 1.0f, top.toARGB()},
        {(float)x, (float)(y + endPoint), 0.0f, 1.0f, bottom.toARGB()},
        {(float)(x + w), (float)(y + endPoint), 0.0f, 1.0f, bottom.toARGB()},

        // Bottom segment
        {(float)x, (float)(y + h), 0.0f, 1.0f, bottom.toARGB()},
        {(float)(x + w), (float)(y + h), 0.0f, 1.0f, bottom.toARGB()},
    };

    d3ddev->SetFVF(D3DVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 6, vertices, sizeof(D3DVertex));
}

void DirectXGPU::drawText(const char *str, int len, int x, int y, int w, int h, Color color)
{
    RECT textRect = {x, y, x + w, y + h};
    d3dfont->DrawText(NULL, str, len, &textRect, 0, color.toARGB());
}

void DirectXGPU::drawSpriteObject(SpriteObject *obj)
{
    D3DSpriteVertex sprite[] = {
        {0, 0, 0.0f, 1.0f, 0.0f, 0.0f},     // Top-left
        {128, 0, 0.0f, 1.0f, 0.0f, 1.0f},   // Top-right
        {0, 128, 0.0f, 1.0f, 1.0f, 0.0f},   // Bottom-left
        {128, 128, 0.0f, 1.0f, 1.0f, 1.0f}, // Bottom-right
    };

    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3ddev->SetTexture(0, d3dtexture);
    d3ddev->SetFVF(D3DSpriteVertex::FVF);
    d3ddev->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, sprite, sizeof(D3DSpriteVertex));
    d3ddev->SetTexture(0, nullptr); // Reset texture
    d3ddev->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}

bool DirectXGPU::endRender()
{
    if (!d3d || !d3ddev)
        return 1;

    backBuffer->Release();
    d3ddev->EndScene();
    d3ddev->Present(nullptr, nullptr, nullptr, nullptr);
    return 0;
}

bool DirectXGPU::shutdown(){
    if (adaptorList) {
        delete adaptorList;
        adaptorList = nullptr;
    }
    reset();
    return 1;
}

DirectXGPU::~DirectXGPU()
{
    shutdown();
}