# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# CMakeLists.txt - Created on 21-04-2025
# 
# GenV is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
# 
# GenV is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
# A PARTICULAR PURPOSE. See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# GenV. If not, see <https://www.gnu.org/licenses/>.

## PSX Core Library
add_library(genv_psx_core STATIC)
target_sources(genv_psx_core PUBLIC
    system.cpp
	#file/file.cpp
	cpu/cache.s
	system/sys.c
	system/sys.s
	system/unhandledexc.c
	system/serial.c
	system/timer.c
	system/pcdrv.s
	halt/halt.c
    drivers/video/video.cpp
    drivers/video/texmgr.cpp
    drivers/video/psxtex.cpp
	drivers/spu.cpp
	drivers/sio0/memcard.cpp
	drivers/sio0/psx_joy.cpp
	drivers/sio0/psx_mc.cpp
	drivers/sio0/psx_sio0.cpp
	drivers/psx_pcdrv.cpp
)

target_include_directories(
    genv_psx_core PUBLIC
	${CMAKE_SOURCE_DIR}/src/genv_hw
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)