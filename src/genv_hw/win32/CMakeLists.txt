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
	GenV-Win32-Core
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

## Win32 Core Library

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(genv_hw_win32 STATIC)

target_sources(
    genv_hw_win32 PRIVATE
	video/gpu-d3d.cpp
	video/gpu-sdl.cpp
	video/video.cpp
	video/vidutil.cpp
	audio/audio.cpp
	audio/directsound.cpp
	file/file.cpp
	res/icon.rc
	res/info.rc
	window-mgr.cpp
	system.cpp
	win_platform.cpp
)

target_include_directories(
	genv_hw_win32 PUBLIC
	${CMAKE_SOURCE_DIR}/src/genv_hw/win32
	${CMAKE_SOURCE_DIR}/src/genv_common
)
	
set(COMMON_LIBS winmm user32 genv_common)

if(MINGW)
    target_link_libraries(genv_hw_win32 PRIVATE ${COMMON_LIBS} dsound dxguid d3d9 d3dx9 gdi32 winmm)
elseif(MSVC)
    target_link_libraries(genv_hw_win32 PRIVATE ${COMMON_LIBS} dsound dxguid d3d9 d3dx9 Msimg32 winmm)
endif()