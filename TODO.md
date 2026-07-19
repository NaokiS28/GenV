# TODO List

This file is auto-generated.
Do not edit manually.

_Generated on 2026-07-19 14:02:56.944747_

#### [src/example/input_page.cpp](src/example/input_page.cpp)

- [185](src/example/input_page.cpp?plain=1#L185): macOS - Sometimes this gets a random and invalid device pointer in the device list?

#### [src/genv_common/app/appmgr.cpp](src/genv_common/app/appmgr.cpp)

- [33](src/genv_common/app/appmgr.cpp?plain=1#L33): Rewrite app life cycle management
- [34](src/genv_common/app/appmgr.cpp?plain=1#L34): Better defined API for app management (app should request to close itself and report when it can be quit)
- [35](src/genv_common/app/appmgr.cpp?plain=1#L35): Critical errors should halt the app execution flow (or rather only allow test mode).
- [36](src/genv_common/app/appmgr.cpp?plain=1#L36): Pause the running apps (and sounds). Apps should still render the last frame when the error occurs.
- [313](src/genv_common/app/appmgr.cpp?plain=1#L313): Indirect pointers and function execution is insanely slow on PS1. (But not on debug?)

#### [src/genv_common/common/formats/image_file.cpp](src/genv_common/common/formats/image_file.cpp)

- [88](src/genv_common/common/formats/image_file.cpp?plain=1#L88): Pallete loading might be borked.
- [113](src/genv_common/common/formats/image_file.cpp?plain=1#L113): Dont do this
- [124](src/genv_common/common/formats/image_file.cpp?plain=1#L124): Dont do this
- [175](src/genv_common/common/formats/image_file.cpp?plain=1#L175): Animated image support
- [311](src/genv_common/common/formats/image_file.cpp?plain=1#L311): // case Genv_RAW_type: return loadRAW_memory(objectID, data, length);	Implement raw format loading (asks GPU core to handle this)

#### [src/genv_common/common/formats/wav.cpp](src/genv_common/common/formats/wav.cpp)

- [52](src/genv_common/common/formats/wav.cpp?plain=1#L52): SoundObject::sampleData = (file.getRawData() + waveData.data); Change from being hardcoded

#### [src/genv_common/common/objects/font.cpp](src/genv_common/common/objects/font.cpp)

- [88](src/genv_common/common/objects/font.cpp?plain=1#L88): Change this so it prefers non-bold/italic fonts when requested

#### [src/genv_common/common/objects/tile.cpp](src/genv_common/common/objects/tile.cpp)

- [127](src/genv_common/common/objects/tile.cpp?plain=1#L127): This just assumes you're using a 2D array.

#### [src/genv_common/common/services/genv_sys.cpp](src/genv_common/common/services/genv_sys.cpp)

- [66](src/genv_common/common/services/genv_sys.cpp?plain=1#L66): Allow setting custom startup baud

#### [src/genv_common/common/services/system/arcade/arcade.cpp](src/genv_common/common/services/system/arcade/arcade.cpp)

- [128](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L128): Check IO service?
- [134](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L134): Check IO service?
- [139](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L139): Check IO service?
- [144](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L144): Check IO service?
- [149](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L149): Check IO service?
- [154](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L154): Check IO service?

#### [src/genv_common/common/services/system/iface_videodrv.hpp](src/genv_common/common/services/system/iface_videodrv.hpp)

- [77](src/genv_common/common/services/system/iface_videodrv.hpp?plain=1#L77): Would be better to use tuples if possible?

#### [src/genv_common/common/services/video/fontman.cpp](src/genv_common/common/services/video/fontman.cpp)

- [44](src/genv_common/common/services/video/fontman.cpp?plain=1#L44): if (loadFontsetFromMemory(spice_data, spice_len) == GV_OK) Configure by header rather than hardcoded here
- [195](src/genv_common/common/services/video/fontman.cpp?plain=1#L195): Unload texture from VRAM. It became too unpopular to be notable.
- [199](src/genv_common/common/services/video/fontman.cpp?plain=1#L199): TESTING ONLY
- [230](src/genv_common/common/services/video/fontman.cpp?plain=1#L230): Unload fonts based on hashed name/details, remove from poplist and unload from VRAM if uploaded

#### [src/genv_common/common/services/video/fontman.hpp](src/genv_common/common/services/video/fontman.hpp)

- [31](src/genv_common/common/services/video/fontman.hpp?plain=1#L31): Support loading from raw data - We need to have a safe-fall back font.

#### [src/genv_common/common/services/video/screen.hpp](src/genv_common/common/services/video/screen.hpp)

- [163](src/genv_common/common/services/video/screen.hpp?plain=1#L163): Should this be a system call? Logically it's a screen function, but realistically all GPUs should go full screen

#### [src/genv_hw/mac/osx/README.md](src/genv_hw/mac/osx/README.md)

- [4](src/genv_hw/mac/osx/README.md?plain=1#L4): Add documentation and support status

#### [src/genv_hw/mac/osx/system.mm](src/genv_hw/mac/osx/system.mm)

- [263](src/genv_hw/mac/osx/system.mm?plain=1#L263): create and register an Audio::IAudioDriver driver (e.g. CoreAudio).
- [274](src/genv_hw/mac/osx/system.mm?plain=1#L274): create and register IO drivers (keyboard, gamepad via IOKit / GCController).
- [284](src/genv_hw/mac/osx/system.mm?plain=1#L284): initialise file/storage manager for macOS paths.
- [369](src/genv_hw/mac/osx/system.mm?plain=1#L369): use an NSLock or os_unfair_lock for thread safety.
- [374](src/genv_hw/mac/osx/system.mm?plain=1#L374): unlock the above.

#### [src/genv_hw/mac/osx/video/gpu-metal.mm](src/genv_hw/mac/osx/video/gpu-metal.mm)

- [517](src/genv_hw/mac/osx/video/gpu-metal.mm?plain=1#L517): enumerate CGDisplayCopyAllDisplayModes() and build a VideoModeList.
- [527](src/genv_hw/mac/osx/video/gpu-metal.mm?plain=1#L527): render text using a sprite font or CoreText.
- [543](src/genv_hw/mac/osx/video/gpu-metal.mm?plain=1#L543): create a MTLTexture from tObj->data and store in _texMap.

#### [src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp)

- [61](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L61): Add support for PS1 mouse in VMouse.
- [225](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L225): Trim pad polling code down and use per-frame pacing
- [226](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L226): If controller in multitap with controller 1:B is connected whilst Port 2:A also connected to a controller prior, both are assigned player 2. Does not happen in reverse
- [262](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L262): Mouse "button" bits 8&9 are signs for rotary?
- [263](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L263): Mouse rotary inputs aren't working. Either it's here or in vpad.
- [270](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L270): JogCon will not enable the dial functions and stays in compatability mode until you enable it with a config command, even if you press the mode button. GenV doesn't do this yet.

#### [src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp)

- [31](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp?plain=1#L31): Implement memory card as a virtual file system complete with directory listing.

#### [src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp](src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp)

- [25](src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp?plain=1#L25): SIO0 Driver needs to be rewritten both to handle multitaps better but also to run controllers at 1MHz when in a multitap else there is *severe* slowdown.

#### [src/genv_hw/psx/common/drivers/sio1/psx_sio1.cpp](src/genv_hw/psx/common/drivers/sio1/psx_sio1.cpp)

- [24](src/genv_hw/psx/common/drivers/sio1/psx_sio1.cpp?plain=1#L24): SIO1

#### [src/genv_hw/psx/common/drivers/video/gpucmd.hpp](src/genv_hw/psx/common/drivers/video/gpucmd.hpp)

- [22](src/genv_hw/psx/common/drivers/video/gpucmd.hpp?plain=1#L22): 4096 words (16KB per chain, 32KB total for double-buffer). Increased

#### [src/genv_hw/psx/common/drivers/video/psxtex.hpp](src/genv_hw/psx/common/drivers/video/psxtex.hpp)

- [44](src/genv_hw/psx/common/drivers/video/psxtex.hpp?plain=1#L44): Allow CLUT to exist independantly of texture

#### [src/genv_hw/psx/common/drivers/video/texmgr.cpp](src/genv_hw/psx/common/drivers/video/texmgr.cpp)

- [18](src/genv_hw/psx/common/drivers/video/texmgr.cpp?plain=1#L18): Code works but has unnaccounted edge cases and an O(-3) problem as spicy puts it.
- [374](src/genv_hw/psx/common/drivers/video/texmgr.cpp?plain=1#L374): UGLY. UGLY hack.

#### [src/genv_hw/psx/common/drivers/video/texmgr.hpp](src/genv_hw/psx/common/drivers/video/texmgr.hpp)

- [18](src/genv_hw/psx/common/drivers/video/texmgr.hpp?plain=1#L18): Bugs in CLUT allocation. Is only returning free clut lines every 32 pixels instead of 16.
- [19](src/genv_hw/psx/common/drivers/video/texmgr.hpp?plain=1#L19): Rewrite for better efficiency.
- [137](src/genv_hw/psx/common/drivers/video/texmgr.hpp?plain=1#L137): Redo VRAM allocation
- [227](src/genv_hw/psx/common/drivers/video/texmgr.hpp?plain=1#L227): Allow CLUT to exist independantly of texture

#### [src/genv_hw/psx/common/drivers/video/video.cpp](src/genv_hw/psx/common/drivers/video/video.cpp)

- [403](src/genv_hw/psx/common/drivers/video/video.cpp?plain=1#L403): Cleanup?
- [738](src/genv_hw/psx/common/drivers/video/video.cpp?plain=1#L738): Seperate font renderer from PS1 core to generic video service

#### [src/genv_hw/psx/common/drivers/video/video.hpp](src/genv_hw/psx/common/drivers/video/video.hpp)

- [92](src/genv_hw/psx/common/drivers/video/video.hpp?plain=1#L92): Set true when the DMA chain is full. Draw calls become no-ops

#### [src/genv_hw/psx/common/halt/src/ps1/sys.h](src/genv_hw/psx/common/halt/src/ps1/sys.h)

- [25](src/genv_hw/psx/common/halt/src/ps1/sys.h?plain=1#L25): Convert to C++
- [159](src/genv_hw/psx/common/halt/src/ps1/sys.h?plain=1#L159): // void psx_delayMicroseconds(int time);	Make this function save the state of the Timer 2 variable so that it wont break things.

#### [src/genv_hw/psx/common/system.cpp](src/genv_hw/psx/common/system.cpp)

- [80](src/genv_hw/psx/common/system.cpp?plain=1#L80): SIO1 driver will require interrupts in future, so this will need to change.
- [160](src/genv_hw/psx/common/system.cpp?plain=1#L160): Allow setting custom startup baud - This is a GenV common issue

#### [src/genv_hw/psx/common/system/serial.h](src/genv_hw/psx/common/system/serial.h)

- [26](src/genv_hw/psx/common/system/serial.h?plain=1#L26): Convert to proper C++ driver

#### [src/genv_hw/psx/common/system/sys.h](src/genv_hw/psx/common/system/sys.h)

- [25](src/genv_hw/psx/common/system/sys.h?plain=1#L25): Convert to C++
- [159](src/genv_hw/psx/common/system/sys.h?plain=1#L159): // void psx_delayMicroseconds(int time);	Make this function save the state of the Timer 2 variable so that it wont break things.

#### [src/genv_hw/psx/psx/system.cpp](src/genv_hw/psx/psx/system.cpp)

- [49](src/genv_hw/psx/psx/system.cpp?plain=1#L49): int error = 0; How to handle multiple driver failures?

#### [src/genv_hw/psx/sys573/io/jamma/adc083x.hpp](src/genv_hw/psx/sys573/io/jamma/adc083x.hpp)

- [27](src/genv_hw/psx/sys573/io/jamma/adc083x.hpp?plain=1#L27): Does not support differential mode. But is this worth supporting?
- [28](src/genv_hw/psx/sys573/io/jamma/adc083x.hpp?plain=1#L28): Critical: Implementation does not work. Require logic probing.

#### [src/genv_hw/win32/README.md](src/genv_hw/win32/README.md)

- [4](src/genv_hw/win32/README.md?plain=1#L4): Add documentation and support status

#### [src/genv_hw/win32/system.cpp](src/genv_hw/win32/system.cpp)

- [64](src/genv_hw/win32/system.cpp?plain=1#L64): setFullscreen(Video::Windowed); Fullscreen and Borderless are fucked up.

#### [src/genv_hw/win32/video/gpu-d3d.cpp](src/genv_hw/win32/video/gpu-d3d.cpp)

- [576](src/genv_hw/win32/video/gpu-d3d.cpp?plain=1#L576): buff.second->Release(); Segfault on close due to race conditions and deleting non-existant textures

