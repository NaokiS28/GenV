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
add_library(genv_hw_psx STATIC)

target_sources(genv_hw_psx PUBLIC
    audio.cpp
    system.cpp
	#file/file.cpp
	file/memcard.cpp
	cpu/cache.s
	system/sys.c
	system/sys.s
	system/unhandledexc.c
	system/serial.c
	system/timer.c
	file/pcdrv.s
	audio/spu.cpp
    video/video.cpp
    video/texmgr.cpp
    video/psxtex.cpp
	halt/halt.c
	drivers/sio0/psx_joy.cpp
	drivers/sio0/psx_mc.cpp
	drivers/sio0/psx_sio0.cpp
	drivers/psx_cdrom.cpp
	drivers/psx_pcdrv.cpp
)

target_compile_definitions(
	genv_hw_psx PRIVATE
	GENV_PSX_VRAM_SIZE=1	# I know this is a problem as the 573 overwrites it or tries to.
)

target_include_directories(
    genv_hw_psx PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_psx PRIVATE genv_common genv_util genv_terminal genv_halt_screen genv_font_glyph)