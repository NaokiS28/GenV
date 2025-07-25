/*
 * GenV - Copyright (C) 2025 NaokiS, spicyjpeg
 * Example.cpp - Created on 05-05-2025
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

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

// Vertex format
struct CUSTOMVERTEX {
    float x, y, z;
    float u, v;
};
#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_TEX1)

// Globals
LPDIRECT3D9 d3d = nullptr;
LPDIRECT3DDEVICE9 d3dDevice = nullptr;
LPDIRECT3DTEXTURE9 texture = nullptr;
LPDIRECT3DVERTEXBUFFER9 vbo = nullptr;

// Vertex data
CUSTOMVERTEX quad[] = {
    { -1.0f,  1.0f, 0.0f, 0.0f, 0.0f },
    {  1.0f,  1.0f, 0.0f, 1.0f, 0.0f },
    { -1.0f, -1.0f, 0.0f, 0.0f, 1.0f },
    {  1.0f, -1.0f, 0.0f, 1.0f, 1.0f }
};

// Initialize Direct3D
void InitD3D(HWND hWnd) {
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    D3DPRESENT_PARAMETERS pp = {};
    pp.Windowed = TRUE;
    pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    pp.BackBufferFormat = D3DFMT_UNKNOWN;

    d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                      &pp, &d3dDevice);

    // Create vertex buffer
    d3dDevice->CreateVertexBuffer(sizeof(quad), 0, D3DFVF_CUSTOMVERTEX,
                                  D3DPOOL_MANAGED, &vbo, NULL);

    void* ptr = nullptr;
    vbo->Lock(0, 0, &ptr, 0);
    memcpy(ptr, quad, sizeof(quad));
    vbo->Unlock();

    // Load PNG texture with alpha
    D3DXCreateTextureFromFile(d3dDevice, L"texture.png", &texture);

    // Alpha blending setup
    d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    d3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
    d3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
    d3dDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
    d3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

    // View transform
    D3DXMATRIX matView;
    D3DXMatrixLookAtLH(&matView,
        &D3DXVECTOR3(0.0f, 0.0f, -5.0f),
        &D3DXVECTOR3(0.0f, 0.0f, 0.0f),
        &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    d3dDevice->SetTransform(D3DTS_VIEW, &matView);

    // Projection transform
    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI / 4, 640.0f / 480.0f, 1.0f, 100.0f);
    d3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);
}

// Render frame
void RenderFrame() {
    d3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(30, 30, 30), 1.0f, 0);
    d3dDevice->BeginScene();

    // Time-based rotation
    float t = GetTickCount() / 1000.0f;
    D3DXMATRIX matRot, matTrans, matWorld;
    D3DXMatrixRotationZ(&matRot, t);
    D3DXMatrixTranslation(&matTrans, 0.0f, 0.0f, 2.0f);
    matWorld = matRot * matTrans;
    d3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

    d3dDevice->SetStreamSource(0, vbo, 0, sizeof(CUSTOMVERTEX));
    d3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
    d3dDevice->SetTexture(0, texture);
    d3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    d3dDevice->EndScene();
    d3dDevice->Present(NULL, NULL, NULL, NULL);
}

// Cleanup
void CleanD3D() {
    if (texture) texture->Release();
    if (vbo) vbo->Release();
    if (d3dDevice) d3dDevice->Release();
    if (d3d) d3d->Release();
}

// Window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_DESTROY) {
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// Entry point
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int nCmdShow) {
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0, 0,
                      hInst, NULL, NULL, NULL, NULL, L"DX9Wnd", NULL };
    RegisterClassEx(&wc);

    HWND hWnd = CreateWindow(L"DX9Wnd", L"DX9 Rotating Quad",
                             WS_OVERLAPPEDWINDOW, 100, 100, 640, 480,
                             NULL, NULL, hInst, NULL);

    InitD3D(hWnd);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg); DispatchMessage(&msg);
        } else {
            RenderFrame();
        }
    }

    CleanD3D();
    UnregisterClass(L"DX9Wnd", hInst);
    return 0;
}
