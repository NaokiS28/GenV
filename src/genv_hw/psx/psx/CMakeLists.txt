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
    ${CMAKE_CURRENT_SOURCE_DIR}/audio.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/file/file.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/file/memcard.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/cpu/cache.s
	${CMAKE_CURRENT_SOURCE_DIR}/system/sys.c
	${CMAKE_CURRENT_SOURCE_DIR}/system/sys.s
	${CMAKE_CURRENT_SOURCE_DIR}/system/unhandledexc.c
	${CMAKE_CURRENT_SOURCE_DIR}/system/serial.c
	${CMAKE_CURRENT_SOURCE_DIR}/system/timer.c
	${CMAKE_CURRENT_SOURCE_DIR}/file/pcdrv.s
	${CMAKE_CURRENT_SOURCE_DIR}/audio/spu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/video/video.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/video/texmgr.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/video/psxtex.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/sio0/psx_joy.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/sio0/psx_mc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/sio0/psx_sio0.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/psx_cdrom.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/drivers/psx_pcdrv.cpp
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

target_link_libraries(genv_hw_psx PRIVATE genv_common genv_util genv_terminal)