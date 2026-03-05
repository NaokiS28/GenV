/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * gpu-metal.hpp - Hello World example for macOS Metal video driver
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

/*
 * HELLO WORLD EXAMPLE - macOS Metal video driver skeleton
 * =========================================================
 * This file shows how to implement a Video::IVideo subclass for macOS 11+
 * using Metal (via the Objective-C++ bridge).  It is intentionally minimal:
 * every draw call is a no-op stub so it compiles and links cleanly, giving
 * you a working baseline to flesh out one function at a time.
 *
 * Key macOS concepts used here
 * ----------------------------
 *  - NSWindow / NSView        : Cocoa window & content-view (AppKit)
 *  - CAMetalLayer             : Core Animation layer that backs a Metal surface
 *  - id<MTLDevice>            : The GPU object obtained from MTLCreateSystemDefaultDevice()
 *  - id<MTLCommandQueue>      : Serialised stream of GPU work submitted to the device
 *  - id<CAMetalDrawable>      : One frame's renderable texture, retrieved each frame
 *  - MTLRenderPassDescriptor  : Describes the render target(s) for a single pass
 *  - id<MTLCommandBuffer>     : Records one frame's GPU commands; committed to the queue
 *  - id<MTLRenderCommandEncoder> : Encodes draw calls into the command buffer
 *
 * Relationship to the rest of GenV
 * ---------------------------------
 *  Video::IVideo  (iface_video.hpp)   <- abstract interface every GPU driver implements
 *  Video::NullVideo (nullvideo.hpp)   <- no-op reference implementation
 *  This class  MetalGPU               <- macOS-specific Metal implementation (stub)
 *
 * The OSXSystem (system.hpp / system.cpp) calls:
 *   1. initVideo()  ->  creates MetalGPU, calls init(), registers with ServiceManager
 *   2. update()     ->  pumps NSApp event queue (like WndProc / PeekMessage on Win32)
 *   3. shutdown()   ->  tears everything down in reverse order
 *
 * File layout (what you still need to create)
 * --------------------------------------------
 *   gpu-metal.mm        Objective-C++ implementation of MetalGPU (see gpu-metal.mm)
 *   system.hpp          OSXSystem class declaration (see system.hpp)
 *   system.mm           OSXSystem implementation
 *   osx_platform.mm     NSApp delegate & event pump (like win_platform.cpp)
 */

#pragma once

// Objective-C / Cocoa headers must be included before C++ standard headers
// when mixing Objective-C++ (.mm) and plain C++ (.hpp).  In a pure .hpp that
// is only included from .mm files this is fine; if you ever include this from
// a .cpp file you will need a forward-declaration wrapper instead.
#ifdef __OBJC__
#import <Cocoa/Cocoa.h>             // NSWindow, NSView, NSApplication …
#import <Metal/Metal.h>             // id<MTLDevice>, MTLRenderPassDescriptor …
#import <QuartzCore/CAMetalLayer.h> // CAMetalLayer
#endif

#include <unordered_map>

#include "common/objects/texture.hpp"
#include "common/objects/sprite.hpp"
#include "common/objects/tile.hpp"
#include "common/services/video/iface_video.hpp"

// ---------------------------------------------------------------------------
// MetalTexture
// ---------------------------------------------------------------------------
// Platform-specific texture object that wraps an id<MTLTexture>.
// Mirrors D3DTexture in gpu-d3d.hpp.
// ---------------------------------------------------------------------------
class MetalTexture : public Textures::TextureObject
{
public:
    // The Metal texture handle.  void* lets us avoid Objective-C types in the
    // header when it is included from plain C++.  Cast to id<MTLTexture> in .mm.
    void *mtlTexture = nullptr; // id<MTLTexture>

    explicit MetalTexture(util::Hash id) : Textures::TextureObject(id) {}
    ~MetalTexture();
};

// ---------------------------------------------------------------------------
// MetalGPU
// ---------------------------------------------------------------------------
// Minimal IVideo implementation for macOS 11+ using Metal.
//
// Lifetime
//   1. Constructed by OSXSystem::initVideo() with a pointer to the NSWindow.
//   2. init()       – acquires MTLDevice, creates CAMetalLayer, command queue.
//   3. beginRender()– nextDrawable(), begins command buffer + render encoder.
//   4. endRender()  – ends encoder, presents drawable, commits command buffer.
//   5. shutdown()   – releases all Metal objects.
// ---------------------------------------------------------------------------
class MetalGPU : public Video::IVideo
{
public:
    // window is an NSWindow* passed as void* so this header is includable from
    // plain C++.  Cast it to NSWindow* inside the .mm implementation.
    explicit MetalGPU(void *window);
    ~MetalGPU();

    // -----------------------------------------------------------------------
    // IVideo lifecycle
    // -----------------------------------------------------------------------
    bool init() override;        // Acquire MTLDevice, set up CAMetalLayer
    bool reset() override;       // Recreate swapchain after resize / mode change
    bool beginRender() override; // Get next drawable, begin render command encoder
    bool endRender() override;   // End encoder, present, commit
    void shutdown() override;    // Release all Metal objects

    bool waitingForVSync() override { return _waitingForVSync; }
    void doWaitForVSync() override;

    // -----------------------------------------------------------------------
    // Resolution / fullscreen
    // -----------------------------------------------------------------------
    int setResolution(int w, int h, bool updateWindow = true) override;
    bool setFullscreen(Video::FullscreenMode mode, int w = 0, int h = 0) override;
    Video::FullscreenMode getFullscreenMode() override { return _fsMode; }

    const Video::VideoModeList *getSupportedResolutions() override;

    // -----------------------------------------------------------------------
    // Draw primitives (stubs – implement one at a time)
    // -----------------------------------------------------------------------
    void drawAlpha(int x, int y, int w, int h, int sx, int sy, uint8_t a) const override {}

    void drawLine(int x1, int y1, int x2, int y2, int width, Video::Color color) override {}
    void drawGradientLine(int x1, int y1, int x2, int y2, int width, Video::Color c1, Video::Color c2) override {}

    void drawRect(int x, int y, int w, int h, Video::Color color) override {}
    void drawGradientRect(int x, int y, int w, int h, Video::GPUGradientMode m) override {}
    void drawGradientRectH(int x, int y, int w, int h, Video::Color left, Video::Color right) override {}
    void drawGradientRectV(int x, int y, int w, int h, Video::Color top, Video::Color bottom) override {}
    void drawGradientRectD(int x, int y, int w, int h,
                           Video::Color top, Video::Color middle, Video::Color bottom) override {}
    void drawGradientRectHVar(int x, int y, int w, int h,
                              Video::Color left, Video::Color right, int s, int e) override {}
    void drawGradientRectVVar(int x, int y, int w, int h,
                              Video::Color top, Video::Color bottom, int s, int e) override {}

    // -----------------------------------------------------------------------
    // Text
    // -----------------------------------------------------------------------
    int drawText(const char *str, int x, int y, int w, int h,
                 Video::Color color = Video::Colors::White, uint8_t mode = Video::TALIGN_LEFT) override;

    int drawText(Fonts::FontObject *fObj, const char *str, int x, int y, int w, int h,
                 Video::Color color = Video::Colors::White, uint8_t mode = Video::TALIGN_LEFT) override
    {
        return drawText(str, x, y, w, h, color, mode);
    }

    // -----------------------------------------------------------------------
    // Textures
    // -----------------------------------------------------------------------
    Textures::TextureObject *createTexture(util::Hash id) override;
    int uploadTexture(Textures::TextureObject *tObj) override;
    int releaseTexture(Textures::TextureObject *tObj) override;

    // -----------------------------------------------------------------------
    // Sprites / tiles / textured quads (stubs)
    // -----------------------------------------------------------------------
    void drawSpriteObject(Sprites::SpriteObject *sObj, int x, int y, int w, int h) override {}
    void drawTileObject(Sprites::TileObject *sObj, int x, int y, int w, int h) override {}

    int drawTextureObject(const Textures::TextureObject *tObj,
                          int x, int y, int w, int h,
                          ifloat u1, ifloat v1, ifloat u2, ifloat v2) override { return 0; }

    int drawTextureObject(const Textures::TextureObject *tObj,
                          int x, int y, Video::Vertex v[]) override { return 0; }

private:
    // -----------------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------------
    bool setupMetalLayer(); // Attaches CAMetalLayer to the NSWindow's content view
    bool createPipeline();  // Compiles default shaders, builds render pipeline state

    // -----------------------------------------------------------------------
    // Metal state (stored as void* to avoid ObjC types in this header)
    // -----------------------------------------------------------------------
    void *_window          = nullptr; // NSWindow*
    void *_device          = nullptr; // id<MTLDevice>
    void *_queue           = nullptr; // id<MTLCommandQueue>
    void *_layer           = nullptr; // CAMetalLayer*
    void *_drawable        = nullptr; // id<CAMetalDrawable>  – valid between begin/endRender
    void *_cmdBuffer       = nullptr; // id<MTLCommandBuffer> – valid between begin/endRender
    void *_encoder         = nullptr; // id<MTLRenderCommandEncoder>
    void *_frameSemaphore  = nullptr; // dispatch_semaphore_t – gates CPU to drawable count
    bool  _waitingForVSync = false;

    Video::FullscreenMode _fsMode = Video::FullscreenMode::Windowed;

    // Texture registry – maps GenV hash -> id<MTLTexture> (stored as void*)
    std::unordered_map<uint32_t, void *> _texMap;
};
