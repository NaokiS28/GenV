/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * gpu-metal.mm - Hello World example for macOS Metal video driver
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
 * HELLO WORLD EXAMPLE - gpu-metal.mm
 * ====================================
 * This is an Objective-C++ file (.mm).  The compiler treats it as C++ but
 * also allows Objective-C syntax ([ ] message sends, @import, id<Protocol>
 * types, etc.).  All Metal and Cocoa objects live in this .mm file; the .hpp
 * header only sees void* handles so it remains includable from plain C++.
 *
 * What this file demonstrates
 * ----------------------------
 * 1. Acquiring a MTLDevice (the GPU object).
 * 2. Attaching a CAMetalLayer to a Cocoa NSView so Metal can render into it.
 * 3. The per-frame render loop:  nextDrawable → command buffer → render pass
 *    descriptor → render command encoder → clear the screen → end encode →
 *    present → commit.
 * 4. Minimal shader compilation (inline MSL source) to get a pipeline state
 *    that clears the screen.
 *
 * Metal frame structure (called every game tick)
 * -----------------------------------------------
 *   beginRender()
 *     ┌─ [_layer nextDrawable]               <- asks the OS for the next
 *     │                                         available back-buffer texture
 *     ├─ MTLRenderPassDescriptor              <- describes the render target
 *     │    loadAction  = MTLLoadActionClear   <- clear the texture at the start
 *     │    clearColor  = (r, g, b, a)
 *     │    storeAction = MTLStoreActionStore  <- keep result for presentation
 *     ├─ [queue commandBuffer]               <- new command buffer
 *     └─ [cmdBuf renderCommandEncoderWithDescriptor:]
 *                                            <- begin encoding draw calls
 *
 *   ... draw calls go here (currently all stubs) ...
 *
 *   endRender()
 *     ├─ [encoder endEncoding]               <- finalise the render pass
 *     ├─ [cmdBuf presentDrawable:_drawable]  <- schedule presentation at vsync
 *     └─ [cmdBuf commit]                     <- submit to GPU
 *
 * Required frameworks (add to CMakeLists.txt target_link_libraries)
 * ------------------------------------------------------------------
 *   "-framework Metal"
 *   "-framework QuartzCore"    (for CAMetalLayer)
 *   "-framework Cocoa"
 *   "-framework Foundation"
 */

// Objective-C++ headers first
#import <Cocoa/Cocoa.h>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>
#import <dispatch/dispatch.h>

// Then the GenV C++ header for this class
#include "gpu-metal.hpp"
#include "common/logger/log.hpp"

// ============================================================================
// Convenience macros to cast void* <-> strong ObjC types
// ============================================================================
// We store ObjC objects as void* in the header to keep it C++-compatible.
// These macros make casting readable without spraying __bridge everywhere.

// Cast a void* slot to a strongly-typed ObjC pointer (for reading)
#define AS_DEVICE(p)   ((__bridge id<MTLDevice>)(p))
#define AS_QUEUE(p)    ((__bridge id<MTLCommandQueue>)(p))
#define AS_LAYER(p)    ((__bridge CAMetalLayer*)(p))
#define AS_DRAWABLE(p) ((__bridge id<CAMetalDrawable>)(p))
#define AS_CMDBUF(p)   ((__bridge id<MTLCommandBuffer>)(p))
#define AS_ENCODER(p)  ((__bridge id<MTLRenderCommandEncoder>)(p))
#define AS_WINDOW(p)   ((__bridge NSWindow*)(p))
#define AS_SEMA(p)     ((__bridge dispatch_semaphore_t)(p))

// Number of drawables the layer is allowed to have in-flight simultaneously.
// 2 = double-buffered: lowest latency, GPU and CPU fully pipelined.
// Apple's Metal Best Practices Guide recommends 2 or 3.
static constexpr int kMaxFramesInFlight = 2;

// Store a strong ObjC object into a void* slot (ARC bridge retain)
// Note: __bridge_retained transfers ownership to the void*.  You MUST call
//       CFRelease() (or the ObjC equivalent) when you are done with it.
//       For a hello-world stub that never releases, __bridge is fine.
// In a production driver use __bridge_retained / CFBridgingRelease properly.
#define STORE(slot, expr)  (slot) = (__bridge_retained void*)(expr)

// ============================================================================
// MetalTexture destructor
// ============================================================================
MetalTexture::~MetalTexture()
{
    // If a MTLTexture was uploaded it must be released.  ARC doesn't track
    // void*, so we bridge-transfer back to ARC here.
    if (mtlTexture)
    {
        id<MTLTexture> tex = CFBridgingRelease(mtlTexture);
        (void)tex; // ARC releases it at end of scope
        mtlTexture = nullptr;
    }
}

// ============================================================================
// MetalGPU
// ============================================================================

MetalGPU::MetalGPU(void *window)
    : _window(window)
{
}

MetalGPU::~MetalGPU()
{
    shutdown();
}

// ----------------------------------------------------------------------------
// init
// ----------------------------------------------------------------------------
// 1. Get the system default GPU (MTLCreateSystemDefaultDevice).
// 2. Set up the CAMetalLayer on the window's content view.
// 3. Create the command queue.
// 4. Compile minimal shaders and build a render pipeline state.
// ----------------------------------------------------------------------------
bool MetalGPU::init()
{
    // --- 1. Acquire the GPU ---------------------------------------------------
    // MTLCreateSystemDefaultDevice() returns the primary GPU on the machine.
    // On Apple Silicon this is always the integrated GPU; on Intel Macs with a
    // dGPU it picks the higher-performance one.
    id<MTLDevice> device = MTLCreateSystemDefaultDevice();
    if (!device)
    {
        LOG("metalgpu","MTLCreateSystemDefaultDevice() returned nil. "
               "Metal is not supported on this machine.");
        return false;
    }
    STORE(_device, device);

    // --- 2. CAMetalLayer -----------------------------------------------------
    // Metal does not render into an NSWindow directly.  Instead we attach a
    // CAMetalLayer to the NSView that fills the window.  Each frame we ask the
    // layer for a "drawable" (a MTLTexture we can render into), and after
    // rendering we present it.
    if (!setupMetalLayer())
        return false;

    // --- 3. Command queue -----------------------------------------------------
    // The command queue serialises command buffers (frames) and submits them
    // to the GPU in order.  One queue per device is typical for a game.
    id<MTLCommandQueue> queue = [device newCommandQueue];
    if (!queue)
    {
        LOG("metalgpu","Failed to create MTLCommandQueue.");
        return false;
    }
    STORE(_queue, queue);

    // --- 4. Default render pipeline (clear-only for the hello world stub) -----
    if (!createPipeline())
        return false;

    // --- 5. In-flight frame semaphore ----------------------------------------
    // Initialise to kMaxFramesInFlight so the CPU can begin encoding the next
    // frame before the GPU has finished the previous one (pipelining), while
    // still preventing the CPU from running more than kMaxFramesInFlight frames
    // ahead.  The semaphore is waited in beginRender() and signalled in the
    // command buffer completion handler in endRender(), which fires on the GPU
    // timeline – i.e. at vsync when presentDrawable: is used.
    // See: Metal Best Practices Guide – Triple Buffering
    _frameSemaphore = (__bridge_retained void *)dispatch_semaphore_create(kMaxFramesInFlight);

    LOG("metalgpu","init() succeeded on device: %@", device.name);
    return true;
}

// ----------------------------------------------------------------------------
// setupMetalLayer  (private)
// ----------------------------------------------------------------------------
// Replaces the NSView's default layer with a CAMetalLayer so Metal can
// render into the window.
// ----------------------------------------------------------------------------
bool MetalGPU::setupMetalLayer()
{
    NSWindow *window = AS_WINDOW(_window);
    if (!window)
    {
        LOG("metalgpu","setupMetalLayer: window is nil.");
        return false;
    }

    NSView *contentView = window.contentView;

    // Enable layer-backing on the view so we can swap its layer.
    contentView.wantsLayer = YES;

    // Create the Metal layer and configure it.
    CAMetalLayer *metalLayer = [CAMetalLayer layer];
    metalLayer.device        = AS_DEVICE(_device);
    metalLayer.pixelFormat   = MTLPixelFormatBGRA8Unorm; // Standard 8-bit BGRA

    // Match the drawable size to the view's backing store (handles Retina).
    CGSize backingSize        = [contentView convertSizeToBacking:contentView.bounds.size];
    metalLayer.drawableSize   = backingSize;

    // framebufferOnly = YES is a Metal optimisation hint: the texture will only
    // be used as a render target, not for reading back or sampling.
    metalLayer.framebufferOnly = YES;

    // Cap the number of drawables to match kMaxFramesInFlight so the semaphore
    // and the layer's internal pool stay in sync.  Available since macOS 10.13.2.
    metalLayer.maximumDrawableCount = kMaxFramesInFlight;

    // Swap in the Metal layer.
    contentView.layer = metalLayer;

    STORE(_layer, metalLayer);

    // Record initial resolution.
    _screen.res.width  = (int)contentView.bounds.size.width;
    _screen.res.height = (int)contentView.bounds.size.height;

    return true;
}

// ----------------------------------------------------------------------------
// createPipeline  (private)
// ----------------------------------------------------------------------------
// Compiles a minimal pair of Metal shaders (vertex + fragment) from inline
// MSL source and builds a MTLRenderPipelineState.
//
// For the hello world stub the shaders do nothing useful – we just need a
// valid pipeline object so endRender() doesn't assert.  Once you add real
// draw-call support you will want to load shaders from .metal files or a
// precompiled .metallib in your app bundle.
// ----------------------------------------------------------------------------
bool MetalGPU::createPipeline()
{
    id<MTLDevice> device = AS_DEVICE(_device);

    // ---- Inline MSL shader source -------------------------------------------
    // MSL (Metal Shading Language) is a subset of C++14.
    //
    //  vertex_main   – pass-through: returns the input position unchanged.
    //  fragment_main – solid red for the hello-world (change to taste).
    //
    // In production you would put shaders in a .metal file and let Xcode /
    // CMake compile them to a .metallib at build time, then load them with
    // [device newDefaultLibrary] or [device newLibraryWithURL:error:].
    // -------------------------------------------------------------------------
    NSString *shaderSrc = @
        "#include <metal_stdlib>\n"
        "using namespace metal;\n"
        "\n"
        "struct VertexIn {\n"
        "    float4 position [[attribute(0)]];\n"
        "};\n"
        "\n"
        "vertex float4 vertex_main(uint vid [[vertex_id]]) {\n"
        "    // No geometry yet – this vertex shader is never called in the stub.\n"
        "    return float4(0, 0, 0, 1);\n"
        "}\n"
        "\n"
        "fragment float4 fragment_main() {\n"
        "    return float4(1, 0, 0, 1); // solid red\n"
        "}\n";

    NSError *error = nil;
    id<MTLLibrary> library = [device newLibraryWithSource:shaderSrc
                                                  options:nil
                                                    error:&error];
    if (!library)
    {
        LOG("metalgpu","Shader compilation failed: %@", error.localizedDescription);
        return false;
    }

    id<MTLFunction> vertFn = [library newFunctionWithName:@"vertex_main"];
    id<MTLFunction> fragFn = [library newFunctionWithName:@"fragment_main"];

    // Build a render pipeline descriptor.
    MTLRenderPipelineDescriptor *pipeDesc = [[MTLRenderPipelineDescriptor alloc] init];
    pipeDesc.vertexFunction                    = vertFn;
    pipeDesc.fragmentFunction                  = fragFn;
    pipeDesc.colorAttachments[0].pixelFormat   = MTLPixelFormatBGRA8Unorm;

    // We don't actually use this pipeline in the stub (no draw calls), but
    // creating it proves the device + shaders are working.
    id<MTLRenderPipelineState> pipeline =
        [device newRenderPipelineStateWithDescriptor:pipeDesc error:&error];
    if (!pipeline)
    {
        LOG("metalgpu","Pipeline creation failed: %@", error.localizedDescription);
        return false;
    }

    // In a real driver you would store the pipeline state and bind it in
    // beginRender().  Here we just discard it to keep the stub simple.
    LOG("metalgpu","Render pipeline created successfully.");
    return true;
}

// ----------------------------------------------------------------------------
// reset
// ----------------------------------------------------------------------------
// Called after a resolution or fullscreen-mode change.  Resize the layer's
// drawableSize to match the new window dimensions.
// ----------------------------------------------------------------------------
bool MetalGPU::reset()
{
    CAMetalLayer *layer = AS_LAYER(_layer);
    if (!layer) return false;

    NSWindow *window     = AS_WINDOW(_window);
    NSView   *view       = window.contentView;
    CGSize    backSize   = [view convertSizeToBacking:view.bounds.size];
    layer.drawableSize   = backSize;

    return true;
}

// ----------------------------------------------------------------------------
// beginRender
// ----------------------------------------------------------------------------
// Retrieve the next drawable and begin a render command encoder that clears
// the screen to a dark background colour.
// ----------------------------------------------------------------------------
bool MetalGPU::beginRender()
{
    CAMetalLayer *layer = AS_LAYER(_layer);
    if (!layer) return false;

    id<CAMetalDrawable> drawable = [layer nextDrawable];
    if (!drawable)
    {
        LOG("metalgpu", "nextDrawable returned nil - skipping frame.");
        return false;
    }
    STORE(_drawable, drawable);

    // Create a fresh command buffer for this frame.
    id<MTLCommandBuffer> cmdBuf = [AS_QUEUE(_queue) commandBuffer];
    STORE(_cmdBuffer, cmdBuf);

    // Describe the render pass: clear colour + store the result.
    MTLRenderPassDescriptor *rpd = [MTLRenderPassDescriptor renderPassDescriptor];
    rpd.colorAttachments[0].texture    = drawable.texture;
    rpd.colorAttachments[0].loadAction = MTLLoadActionClear;

    // Clear to a nice dark blue (change to your preferred background colour).
    rpd.colorAttachments[0].clearColor  = MTLClearColorMake(0.05, 0.05, 0.2, 1.0);
    rpd.colorAttachments[0].storeAction = MTLStoreActionStore;

    // Begin the render pass and store the encoder.
    id<MTLRenderCommandEncoder> encoder =
        [cmdBuf renderCommandEncoderWithDescriptor:rpd];
    STORE(_encoder, encoder);

    return true;
}

// ----------------------------------------------------------------------------
// doWaitForVSync
// ----------------------------------------------------------------------------
// Blocks the calling thread until the GPU has finished one in-flight frame,
// pacing the main loop to vsync cadence.  Called by the engine main loop after
// coroutines are drained (see main.cpp) – this is the canonical GenV hook for
// vsync stalls.
//
// The semaphore is signalled by the addCompletedHandler: block registered in
// endRender(), which fires at vsync because presentDrawable: ties the command
// buffer's completion to the display refresh boundary.
// ----------------------------------------------------------------------------
void MetalGPU::doWaitForVSync()
{
    if (!_frameSemaphore)
        return;

    _waitingForVSync = true;
    dispatch_semaphore_wait(AS_SEMA(_frameSemaphore), DISPATCH_TIME_FOREVER);
    _waitingForVSync = false;
}

// ----------------------------------------------------------------------------
// endRender
// ----------------------------------------------------------------------------
// Finish encoding, schedule the drawable for presentation at the next vsync,
// and commit the command buffer to the GPU.
// ----------------------------------------------------------------------------
bool MetalGPU::endRender()
{
    if (!_encoder || !_cmdBuffer || !_drawable)
        return false;

    // End the render command encoder (no more draw calls after this).
    [AS_ENCODER(_encoder) endEncoding];

    // Register a completion handler that signals the semaphore once the GPU
    // has finished this command buffer.  Because presentDrawable: schedules
    // presentation at the next vsync, the completion fires at vsync cadence,
    // which means the semaphore_wait in the next beginRender() call naturally
    // paces the CPU to the display refresh rate.
    dispatch_semaphore_t sema = AS_SEMA(_frameSemaphore);
    [AS_CMDBUF(_cmdBuffer) addCompletedHandler:^(id<MTLCommandBuffer>) {
        dispatch_semaphore_signal(sema);
    }];

    // Schedule the drawable to appear on screen when the GPU finishes.
    // presentDrawable: uses vsync-aligned presentation automatically on macOS.
    [AS_CMDBUF(_cmdBuffer) presentDrawable:AS_DRAWABLE(_drawable)];

    // Submit all recorded commands to the GPU.
    [AS_CMDBUF(_cmdBuffer) commit];

    // Null out per-frame state so we don't accidentally use it next frame.
    // Note: because we used __bridge_retained the void* slots now own a
    // reference.  In a production driver you would CFBridgingRelease() each
    // one here.  For this stub we leak one ref per frame (acceptable for
    // educational purposes, fix before shipping!).
    _encoder   = nullptr;
    _cmdBuffer = nullptr;
    _drawable  = nullptr;

    ++_frameCount;
    return true;
}

// ----------------------------------------------------------------------------
// shutdown
// ----------------------------------------------------------------------------
void MetalGPU::shutdown()
{
    // In a real driver: CFBridgingRelease each void* slot to let ARC clean up.
    // Here we just null everything out for the stub.
    _encoder   = nullptr;
    _cmdBuffer = nullptr;
    _drawable  = nullptr;
    _layer     = nullptr;
    _queue     = nullptr;
    _device    = nullptr;

    if (_frameSemaphore)
    {
        // Transfer ownership back to ARC so it releases the dispatch object.
        (void)CFBridgingRelease(_frameSemaphore);
        _frameSemaphore = nullptr;
    }
}

// ----------------------------------------------------------------------------
// setResolution
// ----------------------------------------------------------------------------
int MetalGPU::setResolution(int w, int h, bool updateWindow)
{
    IVideo::setResolution(w, h, updateWindow);

    if (updateWindow)
    {
        NSWindow *window = AS_WINDOW(_window);
        if (window)
        {
            // Resize the Cocoa window.  NSWindow sizes are in points; on Retina
            // displays the backing-store pixel count is 2× or 3× this.
            NSRect frame = window.frame;
            frame.size   = NSMakeSize(w, h);
            [window setFrame:frame display:YES animate:NO];
        }
    }

    return reset() ? 0 : -1;
}

// ----------------------------------------------------------------------------
// setFullscreen
// ----------------------------------------------------------------------------
bool MetalGPU::setFullscreen(Video::FullscreenMode mode, int w, int h)
{
    NSWindow *window = AS_WINDOW(_window);
    if (!window) return false;

    bool isCurrentlyFullscreen =
        (window.styleMask & NSWindowStyleMaskFullScreen) != 0;

    bool wantFullscreen = (mode == Video::FullscreenMode::Fullscreen ||
                           mode == Video::FullscreenMode::Borderless);

    if (wantFullscreen != isCurrentlyFullscreen)
    {
        // toggleFullScreen: is the standard macOS way to enter/leave full-screen.
        // The transition is animated unless you set window.animationBehavior.
        [window toggleFullScreen:nil];
    }

    _fsMode = mode;
    return true;
}

// ----------------------------------------------------------------------------
// getSupportedResolutions
// ----------------------------------------------------------------------------
const Video::VideoModeList *MetalGPU::getSupportedResolutions()
{
    // TODO: enumerate CGDisplayCopyAllDisplayModes() and build a VideoModeList.
    return &Video::BlankVideoList;
}

// ----------------------------------------------------------------------------
// drawText  (stub)
// ----------------------------------------------------------------------------
int MetalGPU::drawText(const char *str, int x, int y, int w, int h,
                       Video::Color color, uint8_t mode)
{
    // TODO: render text using a sprite font or CoreText.
    // For the hello world example, just log to the console.
    //LOG("metalgpu","drawText(\"%s\", %d, %d)", str, x, y);
    return 0;
}

// ----------------------------------------------------------------------------
// Texture management (stubs)
// ----------------------------------------------------------------------------
Textures::TextureObject *MetalGPU::createTexture(util::Hash id)
{
    return new MetalTexture(id);
}

int MetalGPU::uploadTexture(Textures::TextureObject *tObj)
{
    // TODO: create a MTLTexture from tObj->data and store in _texMap.
    return 0;
}

int MetalGPU::releaseTexture(Textures::TextureObject *tObj)
{
    if (!tObj) return -1;
    auto it = _texMap.find(tObj->getObjectID());
    if (it != _texMap.end())
    {
        // CFBridgingRelease(it->second); // would release the MTLTexture
        _texMap.erase(it);
    }
    return 0;
}
