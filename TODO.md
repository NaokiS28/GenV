# TODO List

This file is auto-generated.
Do not edit manually.

_Generated on 2026-01-25 23:34:39.364397_

#### [src/genv_common/app/appmgr.cpp](src/genv_common/app/appmgr.cpp)

- [33](src/genv_common/app/appmgr.cpp#33): Rewrite app life cycle management
- [34](src/genv_common/app/appmgr.cpp#34): Better defined API for app management (app should request to close itself and report when it can be quit)
- [35](src/genv_common/app/appmgr.cpp#35): Critical errors should halt the app execution flow (or rather only allow test mode).
- [36](src/genv_common/app/appmgr.cpp#36): Pause the running apps (and sounds). Apps should still render the last frame when the error occurs.

#### [src/genv_common/common/formats/image_file.cpp](src/genv_common/common/formats/image_file.cpp)

- [85](src/genv_common/common/formats/image_file.cpp#85): Pallete loading might be borked.
- [110](src/genv_common/common/formats/image_file.cpp#110): Dont do this
- [121](src/genv_common/common/formats/image_file.cpp#121): Dont do this
- [172](src/genv_common/common/formats/image_file.cpp#172): Animated image support

#### [src/genv_common/common/formats/wav.cpp](src/genv_common/common/formats/wav.cpp)

- [52](src/genv_common/common/formats/wav.cpp#52): SoundObject::sampleData = (file.getRawData() + waveData.data); Change from being hardcoded

#### [src/genv_common/common/objects/font.cpp](src/genv_common/common/objects/font.cpp)

- [90](src/genv_common/common/objects/font.cpp#90): Change this so it prefers non-bold/italic fonts when requested

#### [src/genv_common/common/objects/tile.cpp](src/genv_common/common/objects/tile.cpp)

- [121](src/genv_common/common/objects/tile.cpp#121): This just assumes you're using a 2D array.

#### [src/genv_common/common/services/system/arcade/arcade.cpp](src/genv_common/common/services/system/arcade/arcade.cpp)

- [26](src/genv_common/common/services/system/arcade/arcade.cpp#26): Is this needing any extra logic?
- [125](src/genv_common/common/services/system/arcade/arcade.cpp#125): Check IO service?
- [131](src/genv_common/common/services/system/arcade/arcade.cpp#131): Check IO service?
- [136](src/genv_common/common/services/system/arcade/arcade.cpp#136): Check IO service?
- [141](src/genv_common/common/services/system/arcade/arcade.cpp#141): Check IO service?
- [146](src/genv_common/common/services/system/arcade/arcade.cpp#146): Check IO service?
- [151](src/genv_common/common/services/system/arcade/arcade.cpp#151): Check IO service?

#### [src/genv_common/common/services/system/arcade/arcade.hpp](src/genv_common/common/services/system/arcade/arcade.hpp)

- [120](src/genv_common/common/services/system/arcade/arcade.hpp#120): Is this macro of any real benefit now? GetArcadeInterface does the important thing.

#### [src/genv_common/common/services/video/fontman.cpp](src/genv_common/common/services/video/fontman.cpp)

- [44](src/genv_common/common/services/video/fontman.cpp#44): if (loadFontsetFromMemory(spice_data, spice_len) == GV_OK) Configure by header rather than hardcoded here
- [195](src/genv_common/common/services/video/fontman.cpp#195): Unload texture from VRAM. It became too unpopular to be notable.
- [199](src/genv_common/common/services/video/fontman.cpp#199): TESTING ONLY
- [230](src/genv_common/common/services/video/fontman.cpp#230): Unload fonts based on hashed name/details, remove from poplist and unload from VRAM if uploaded

#### [src/genv_common/common/services/video/fontman.hpp](src/genv_common/common/services/video/fontman.hpp)

- [31](src/genv_common/common/services/video/fontman.hpp#31): Support loading from raw data - We need to have a safe-fall back font.

#### [src/genv_common/common/util/misc.hpp](src/genv_common/common/util/misc.hpp)

- [41](src/genv_common/common/util/misc.hpp#41): implement
- [45](src/genv_common/common/util/misc.hpp#45): implement

#### [src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp)

- [30](src/genv_hw/psx/common/drivers/sio0/psx_mc.hpp#30): Implement memory card as a virtual file system complete with directory listing.

#### [src/genv_hw/psx/common/drivers/video/psxtex.hpp](src/genv_hw/psx/common/drivers/video/psxtex.hpp)

- [44](src/genv_hw/psx/common/drivers/video/psxtex.hpp#44): Allow CLUT to exist independantly of texture

#### [src/genv_hw/psx/common/drivers/video/texmgr.cpp](src/genv_hw/psx/common/drivers/video/texmgr.cpp)

- [18](src/genv_hw/psx/common/drivers/video/texmgr.cpp#18): Code works but has unnaccounted edge cases and an O(-3) problem as spicy puts it.
- [374](src/genv_hw/psx/common/drivers/video/texmgr.cpp#374): UGLY. UGLY hack.

#### [src/genv_hw/psx/common/drivers/video/texmgr.hpp](src/genv_hw/psx/common/drivers/video/texmgr.hpp)

- [18](src/genv_hw/psx/common/drivers/video/texmgr.hpp#18): Bugs in CLUT allocation. Is only returning free clut lines every 32 pixels instead of 16.
- [19](src/genv_hw/psx/common/drivers/video/texmgr.hpp#19): Rewrite for better efficiency.
- [220](src/genv_hw/psx/common/drivers/video/texmgr.hpp#220): Allow CLUT to exist independantly of texture

#### [src/genv_hw/psx/common/drivers/video/video.cpp](src/genv_hw/psx/common/drivers/video/video.cpp)

- [439](src/genv_hw/psx/common/drivers/video/video.cpp#439): Cleanup?
- [865](src/genv_hw/psx/common/drivers/video/video.cpp#865): Somewhere, somehow, font height is not being stored in the font file correctly

#### [src/genv_hw/psx/common/halt/halt.c](src/genv_hw/psx/common/halt/halt.c)

- [222](src/genv_hw/psx/common/halt/halt.c#222): Somewhere, somehow, font height is not being stored in the font file correctly
- [233](src/genv_hw/psx/common/halt/halt.c#233): There is no font!

#### [src/genv_hw/psx/common/halt/halt.h](src/genv_hw/psx/common/halt/halt.h)

- [20](src/genv_hw/psx/common/halt/halt.h#20): Currently relies on a existing font from GenV, but trying to assume

#### [src/genv_hw/psx/common/system.cpp](src/genv_hw/psx/common/system.cpp)

- [142](src/genv_hw/psx/common/system.cpp#142): int error = 0; How to handle multiple driver failures?

#### [src/genv_hw/psx/common/system/sys.h](src/genv_hw/psx/common/system/sys.h)

- [157](src/genv_hw/psx/common/system/sys.h#157): // void psx_delayMicroseconds(int time);	Make this function save the state of the Timer 2 variable so that it wont break things.

#### [src/genv_hw/psx/psx/system.cpp](src/genv_hw/psx/psx/system.cpp)

- [63](src/genv_hw/psx/psx/system.cpp#63): int error = 0; How to handle multiple driver failures?

#### [src/genv_hw/win32/system.cpp](src/genv_hw/win32/system.cpp)

- [64](src/genv_hw/win32/system.cpp#64): setFullscreen(Video::Windowed); Fullscreen and Borderless are fucked up.

#### [src/genv_hw/win32/video/gpu-d3d.cpp](src/genv_hw/win32/video/gpu-d3d.cpp)

- [482](src/genv_hw/win32/video/gpu-d3d.cpp#482): Narrowing of int to float
- [576](src/genv_hw/win32/video/gpu-d3d.cpp#576): buff.second->Release();     Segfault on close due to race conditions and deleting non-existant textures

