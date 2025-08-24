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

cmake_minimum_required(VERSION 3.25)

project(
	GenV-PSX-Core
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

## PSX Core Library

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add the directory where ps1_bare_metal is defined

add_library(genv_hw_psx STATIC)

target_sources(genv_hw_psx PRIVATE
    audio.cpp
    system.cpp
	#file/file.cpp
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
)

target_compile_definitions(
	genv_hw_psx PRIVATE
	GENV_PSX_VRAM_SIZE=1
)

target_include_directories(
    genv_hw_psx PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}

)

target_link_libraries(genv_hw_psx PRIVATE genv_common genv_util genv_terminal)

# Konami 573

add_library(genv_hw_sys573 STATIC)

target_sources(genv_hw_sys573 PRIVATE
    audio.cpp
	#file/file.cpp
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
    573/sys573.cpp
	573/rtc.cpp
	573/jamma.cpp
	#573/jvs.cpp
	#573/aio.cpp
	#573/dio.cpp
)

target_compile_definitions(
	genv_hw_sys573 PRIVATE
	GENV_PSX_VRAM_SIZE=2
)

target_include_directories(
    genv_hw_sys573 PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_sys573 PRIVATE genv_common genv_util genv_terminal)
