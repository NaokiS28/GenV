# TODO List

This file is auto-generated.
Do not edit manually.

_Generated on 2026-02-21 22:34:13.103294_

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

- [90](src/genv_common/common/objects/font.cpp?plain=1#L90): Change this so it prefers non-bold/italic fonts when requested

#### [src/genv_common/common/objects/tile.cpp](src/genv_common/common/objects/tile.cpp)

- [121](src/genv_common/common/objects/tile.cpp?plain=1#L121): This just assumes you're using a 2D array.

#### [src/genv_common/common/services/genv_sys.cpp](src/genv_common/common/services/genv_sys.cpp)

- [77](src/genv_common/common/services/genv_sys.cpp?plain=1#L77): Allow setting custom startup baud

#### [src/genv_common/common/services/io/vpad.hpp](src/genv_common/common/services/io/vpad.hpp)

- [63](src/genv_common/common/services/io/vpad.hpp?plain=1#L63): This should update when any inputs on any controller change.
- [64](src/genv_common/common/services/io/vpad.hpp?plain=1#L64): Make test to test specific player's inputs changed - usefull?

#### [src/genv_common/common/services/system/arcade/arcade.cpp](src/genv_common/common/services/system/arcade/arcade.cpp)

- [26](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L26): Is this needing any extra logic?
- [125](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L125): Check IO service?
- [131](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L131): Check IO service?
- [136](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L136): Check IO service?
- [141](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L141): Check IO service?
- [146](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L146): Check IO service?
- [151](src/genv_common/common/services/system/arcade/arcade.cpp?plain=1#L151): Check IO service?

#### [src/genv_common/common/services/system/arcade/arcade.hpp](src/genv_common/common/services/system/arcade/arcade.hpp)

- [120](src/genv_common/common/services/system/arcade/arcade.hpp?plain=1#L120): Is this macro of any real benefit now? GetArcadeInterface does the important thing.

#### [src/genv_common/common/services/video/fontman.cpp](src/genv_common/common/services/video/fontman.cpp)

- [44](src/genv_common/common/services/video/fontman.cpp?plain=1#L44): if (loadFontsetFromMemory(spice_data, spice_len) == GV_OK) Configure by header rather than hardcoded here
- [195](src/genv_common/common/services/video/fontman.cpp?plain=1#L195): Unload texture from VRAM. It became too unpopular to be notable.
- [199](src/genv_common/common/services/video/fontman.cpp?plain=1#L199): TESTING ONLY
- [230](src/genv_common/common/services/video/fontman.cpp?plain=1#L230): Unload fonts based on hashed name/details, remove from poplist and unload from VRAM if uploaded

#### [src/genv_common/common/services/video/fontman.hpp](src/genv_common/common/services/video/fontman.hpp)

- [31](src/genv_common/common/services/video/fontman.hpp?plain=1#L31): Support loading from raw data - We need to have a safe-fall back font.

#### [src/genv_common/common/util/misc.hpp](src/genv_common/common/util/misc.hpp)

- [45](src/genv_common/common/util/misc.hpp?plain=1#L45): implement
- [49](src/genv_common/common/util/misc.hpp?plain=1#L49): implement

#### [src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp)

- [61](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L61): Add support for PSX mouse in VMouse.
- [225](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L225): Trim pad polling code down and use per-frame pacing
- [226](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L226): If controller in multitap with controller 1:B is connected whilst Port 2:A also connected to a controller prior, both are assigned player 2. Does not happen in reverse
- [262](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L262): Mouse "button" bits 8&9 are signs for rotary?
- [263](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L263): Mouse rotary inputs aren't working. Either it's here or in vpad.
- [270](src/genv_hw/psx/common/drivers/sio0/psx_joy.cpp?plain=1#L270): JogCon will not enable the dial functions and stays in compatability mode until you enable it with a config command, even if you press the mode button. GenV doesn't do this yet.

#### [src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp)

- [30](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp?plain=1#L30): Implement memory card as a virtual file system complete with directory listing.

#### [src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp](src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp)

- [26](src/genv_hw/psx/common/drivers/sio0/psx_sio0.cpp?plain=1#L26): SIO0 Driver needs to be rewritten both to handle multitaps better but also to run controllers at 1MHz when in a multitap else there is *severe* slowdown.

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

- [398](src/genv_hw/psx/common/drivers/video/video.cpp?plain=1#L398): Cleanup?
- [258](src/genv_hw/psx/common/drivers/video/video.cpp?plain=1#L258): Overflow protection — if the requested allocation would exceed

#### [src/genv_hw/psx/common/drivers/video/video.hpp](src/genv_hw/psx/common/drivers/video/video.hpp)

- [83](src/genv_hw/psx/common/drivers/video/video.hpp?plain=1#L83): Set true when the DMA chain is full. Draw calls become no-ops

#### [src/genv_hw/psx/common/halt/halt.c](src/genv_hw/psx/common/halt/halt.c)

- [222](src/genv_hw/psx/common/halt/halt.c?plain=1#L222): Somewhere, somehow, font height is not being stored in the font file correctly
- [233](src/genv_hw/psx/common/halt/halt.c?plain=1#L233): There is no font!

#### [src/genv_hw/psx/common/halt/halt.h](src/genv_hw/psx/common/halt/halt.h)

- [20](src/genv_hw/psx/common/halt/halt.h?plain=1#L20): Currently relies on a existing font from GenV, but trying to assume

#### [src/genv_hw/psx/common/system.cpp](src/genv_hw/psx/common/system.cpp)

- [78](src/genv_hw/psx/common/system.cpp?plain=1#L78): SIO1 driver will require interrupts in future, so this will need to change.
- [149](src/genv_hw/psx/common/system.cpp?plain=1#L149): int error = 0; How to handle multiple driver failures?
- [163](src/genv_hw/psx/common/system.cpp?plain=1#L163): Allow setting custom startup baud

#### [src/genv_hw/psx/common/system/sys.h](src/genv_hw/psx/common/system/sys.h)

- [157](src/genv_hw/psx/common/system/sys.h?plain=1#L157): // void psx_delayMicroseconds(int time);	Make this function save the state of the Timer 2 variable so that it wont break things.

#### [src/genv_hw/psx/psx/system.cpp](src/genv_hw/psx/psx/system.cpp)

- [50](src/genv_hw/psx/psx/system.cpp?plain=1#L50): int error = 0; How to handle multiple driver failures?

#### [src/genv_hw/win32/README.md](src/genv_hw/win32/README.md)

- [4](src/genv_hw/win32/README.md?plain=1#L4): Add documentation and support status

#### [src/genv_hw/win32/system.cpp](src/genv_hw/win32/system.cpp)

- [64](src/genv_hw/win32/system.cpp?plain=1#L64): setFullscreen(Video::Windowed); Fullscreen and Borderless are fucked up.

#### [src/genv_hw/win32/video/gpu-d3d.cpp](src/genv_hw/win32/video/gpu-d3d.cpp)

- [482](src/genv_hw/win32/video/gpu-d3d.cpp?plain=1#L482): Narrowing of int to float
- [576](src/genv_hw/win32/video/gpu-d3d.cpp?plain=1#L576): buff.second->Release();     Segfault on close due to race conditions and deleting non-existant textures

