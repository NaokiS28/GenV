/*
 * GenV - Copyright (C) 2025 - 2026 NaokiS, spicyjpeg
 * system.mm - Hello World example for macOS system manager
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
 * HELLO WORLD EXAMPLE - system.mm
 * =================================
 * This is the Objective-C++ implementation of the OSXSystem class declared in
 * system.hpp.  It is the macOS counterpart of src/genv_hw/win32/system.cpp.
 *
 * Responsibilities
 * ----------------
 *  init()       – Create the NSApplication, NSWindow, and all hardware drivers.
 *  update()     – Pump the NSApp event queue so the window stays responsive.
 *  shutdown()   – Tear down drivers and destroy the window.
 *  millis()     – High-resolution time using mach_absolute_time().
 *  micros()     – Same, scaled to microseconds.
 *  getTime()    – Wall-clock time via localtime_r().
 *
 * NSApplication / NSWindow primer
 * --------------------------------
 * On macOS, unlike Win32, an application *must* create an NSApplication
 * singleton before any UI can appear.  GenV's approach (mirroring Win32):
 *
 *   [NSApplication sharedApplication]     <- creates the singleton
 *   [NSApp setActivationPolicy:...]        <- show in Dock / App Switcher
 *   [NSApp activateIgnoringOtherApps:YES]  <- bring to front
 *
 * We do NOT call [NSApp run] because that would block forever in a Cocoa
 * event loop.  Instead we call [NSApp nextEventMatchingMask:...] manually
 * each frame – exactly the equivalent of PeekMessage() / DispatchMessage()
 * in Win32 (see update() below and osx_platform.mm).
 *
 * NSWindow creation
 * -----------------
 *   NSRect frame = NSMakeRect(x, y, width, height);  // origin, size in points
 *   NSWindowStyleMask mask = NSWindowStyleMaskTitled
 *                          | NSWindowStyleMaskClosable
 *                          | NSWindowStyleMaskResizable
 *                          | NSWindowStyleMaskMiniaturizable;
 *   NSWindow *win = [[NSWindow alloc] initWithContentRect:frame
 *                                              styleMask:mask
 *                                                backing:NSBackingStoreBuffered
 *                                                  defer:NO];
 *   [win setTitle:@"GenV"];
 *   [win makeKeyAndOrderFront:nil];  // show the window
 *
 * Timing on macOS
 * ---------------
 * mach_absolute_time() returns a tick count in CPU-dependent units.
 * mach_timebase_info() provides the numer/denom fraction to convert ticks to
 * nanoseconds.  This is the lowest-overhead high-resolution timer on macOS and
 * is the recommended alternative to QueryPerformanceCounter on Win32.
 */

// Objective-C / macOS headers
#import <Cocoa/Cocoa.h>
#import <mach/mach_time.h>

// GenV headers
#include "system.hpp"
#include "video/gpu-metal.hpp"

#include "common/services/services.hpp"
#include "common/logger/log.hpp"

namespace System
{

// ============================================================================
// OSXSystem::OSXSystem / ~OSXSystem
// ============================================================================
OSXSystem::OSXSystem()
    : _smState(System::SM_NORMAL)
{
    // Capture the start time and the conversion factor from ticks -> ns.
    mach_timebase_info(&_tbInfo);
    _startTick = mach_absolute_time();

    _sysInfo.type   = System::SYS_Computer;
    _sysInfo.make   = "Apple";
    _sysInfo.name   = "macOS";
    _sysInfo.osname = "macOS 11+";
}

OSXSystem::~OSXSystem()
{
    shutdown();
}

// ============================================================================
// OSXSystem::initCore
// ============================================================================
// Set up the NSApplication singleton and create the main window.
// This is the macOS equivalent of WinSystem::initWindowClass() +
// WindowManager::NewWindow().
// ============================================================================
int OSXSystem::initCore()
{
    // --- NSApplication -------------------------------------------------------
    // sharedApplication creates the singleton if it does not exist yet.
    // We use NSApplicationActivationPolicyRegular so the app appears in the
    // Dock and Command-Tab switcher like any normal macOS app.
    NSApplication *app = [NSApplication sharedApplication];
    [app setActivationPolicy:NSApplicationActivationPolicyRegular];

    // We act as our own delegate (defined in osx_platform.mm).
    // The delegate receives applicationShouldTerminate: when the user quits.
    // [app setDelegate: ...];   // TODO: set a delegate to handle Quit

    // --- NSWindow ------------------------------------------------------------
    NSRect frame   = NSMakeRect(100, 100, 800, 600); // x, y, width, height (points)
    NSWindowStyleMask mask =
        NSWindowStyleMaskTitled       |  // title bar
        NSWindowStyleMaskClosable     |  // close button
        NSWindowStyleMaskResizable    |  // resize handle
        NSWindowStyleMaskMiniaturizable; // minimise button

    NSWindow *window = [[NSWindow alloc]
        initWithContentRect:frame
                  styleMask:mask
                    backing:NSBackingStoreBuffered  // double-buffered
                      defer:NO];                   // create now, not lazily

    [window setTitle:@"GenV"];
    [window makeKeyAndOrderFront:nil]; // show window and bring to front

    // Activate the application so the window gets focus immediately.
    [app activateIgnoringOtherApps:YES];

    // Store the window as void* so we can hand it to MetalGPU without
    // the header needing ObjC types.
    _window = (__bridge_retained void *)window;

    LOG_APP("OSXSystem: window created (800x600)");
    return 0;
}

// ============================================================================
// OSXSystem::initVideo
// ============================================================================
int OSXSystem::initVideo()
{
    if (!_window)
        return -1;

    // Create and initialise the Metal GPU driver.
    // We pass _window as void* – MetalGPU casts it back to NSWindow* internally.
    MetalGPU *gpu = new MetalGPU(_window);
    if (!gpu->init())
    {
        delete gpu;
        LOG_APP("OSXSystem: MetalGPU init failed");
        return -2;
    }

    // Register the driver with the global ServiceManager.
    getServiceManager()->setVideo(adminKey, gpu);
    LOG_APP("OSXSystem: Metal video driver registered");
    return 0;
}

// ============================================================================
// OSXSystem::initAudio
// ============================================================================
int OSXSystem::initAudio()
{
    // TODO: create and register an Audio::IAudio driver (e.g. CoreAudio).
    // For the hello world stub, do nothing.
    LOG_APP("OSXSystem: audio not implemented (stub)");
    return 0;
}

// ============================================================================
// OSXSystem::initIO
// ============================================================================
int OSXSystem::initIO()
{
    // TODO: create and register IO drivers (keyboard, gamepad via IOKit / GCController).
    LOG_APP("OSXSystem: IO not implemented (stub)");
    return 0;
}

// ============================================================================
// OSXSystem::initStorage
// ============================================================================
int OSXSystem::initStorage()
{
    // TODO: initialise file/storage manager for macOS paths.
    LOG_APP("OSXSystem: storage not implemented (stub)");
    return 0;
}

// ============================================================================
// OSXSystem::update
// ============================================================================
// Called once per game tick from main.cpp.
// Drains the NSApp event queue so the window stays responsive (window moves,
// resizes, close-button clicks, key events, etc. are all processed here).
//
// This is the macOS analogue of Win32's PeekMessage / TranslateMessage /
// DispatchMessage loop.
// ============================================================================
int OSXSystem::update()
{
    _smState = System::SM_NORMAL;

    // Process all pending Cocoa events without blocking.
    // NSDefaultRunLoopMode covers the standard events (mouse, keyboard, system).
    @autoreleasepool
    {
        NSEvent *event;
        while ((event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                          untilDate:nil              // don't block
                                             inMode:NSDefaultRunLoopMode
                                            dequeue:YES]) != nil)
        {
            // Let the default machinery handle most events (keyboard, mouse, etc.)
            // You can intercept specific event types here before passing them on.
            switch ([event type])
            {
            case NSEventTypeKeyDown:
                // Example: F11 toggles fullscreen (mirrors Win32 WM_KEYDOWN handler)
                if ([event keyCode] == 0x67 /* F11 */)
                {
                    NSWindow *window = (__bridge NSWindow *)_window;
                    [window toggleFullScreen:nil];
                    _smState = System::SM_RESIZE;
                }
                break;

            default:
                break;
            }

            // Pass the event to NSApplication for default handling.
            [NSApp sendEvent:event];
        }

        // Update NSApp's display if anything changed.
        [NSApp updateWindows];
    }

    // If the window has been closed, signal the main loop to quit.
    if (_window)
    {
        NSWindow *window = (__bridge NSWindow *)_window;
        if (!window.isVisible)
            return System::SM_QUIT;
    }

    return _smState;
}

// ============================================================================
// OSXSystem::shutdown
// ============================================================================
void OSXSystem::shutdown()
{
    // Video driver shutdown is handled by the ServiceManager destructor.
    // We just need to release the window reference here.
    if (_window)
    {
        NSWindow *window = CFBridgingRelease(_window);
        [window close];
        _window = nullptr;
    }
}

// ============================================================================
// OSXSystem::enterCriticalSection / leaveCriticalSection
// ============================================================================
void OSXSystem::enterCriticalSection()
{
    // TODO: use an NSLock or os_unfair_lock for thread safety.
}

void OSXSystem::leaveCriticalSection()
{
    // TODO: unlock the above.
}

// ============================================================================
// OSXSystem::millis / micros
// ============================================================================
// mach_absolute_time() returns a monotonically increasing tick value.
// mach_timebase_info gives us { numer, denom } such that:
//
//   nanoseconds = ticks * numer / denom
//
// We convert to ms and µs accordingly.
// ============================================================================
size_t OSXSystem::millis()
{
    uint64_t elapsed = mach_absolute_time() - _startTick;
    // (ticks * numer / denom) / 1,000,000  =  milliseconds
    return (size_t)(elapsed * _tbInfo.numer / _tbInfo.denom / 1000000ULL);
}

size_t OSXSystem::micros()
{
    uint64_t elapsed = mach_absolute_time() - _startTick;
    // (ticks * numer / denom) / 1,000  =  microseconds
    return (size_t)(elapsed * _tbInfo.numer / _tbInfo.denom / 1000ULL);
}

// ============================================================================
// OSXSystem::getTime
// ============================================================================
bool OSXSystem::getTime(tm &outTime)
{
    time_t now = time(nullptr);
    return localtime_r(&now, &outTime) != nullptr;
}

// ============================================================================
// OSXSystem::getWorkingDirectory
// ============================================================================
const char *OSXSystem::getWorkingDirectory()
{
    // On macOS the "working directory" for a bundled app is the Resources
    // folder inside the .app bundle.  For a non-bundled binary it is the
    // directory from which the binary was launched.
    static char buf[PATH_MAX] = {};
    if (buf[0] == '\0')
        getcwd(buf, sizeof(buf));
    return buf;
}

// ============================================================================
// OSXSystem::registerTimerFunc / unregisterTimerFunc
// ============================================================================
bool OSXSystem::registerTimerFunc(TFunc func, TChannel timer, uint8_t freq)
{
    if (timer == TChannel::TIMER1)
        return _sysTimer.registerFunction(func, freq);
    return false;
}

bool OSXSystem::unregisterTimerFunc(TFunc func, TChannel timer)
{
    if (timer == TChannel::TIMER1)
        return _sysTimer.unregisterFunction(func);
    return false;
}

} // namespace System
