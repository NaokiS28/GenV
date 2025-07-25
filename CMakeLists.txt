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

execute_process(COMMAND ${CMAKE_COMMAND} -E environment)

project(
	GenV
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

## Main executable

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

if(PSX)
	set(GFX_NO_FLOAT)
endif()

if(WIN32)
	add_executable(
		main WIN32
		src/main.cpp
		src/hw/win32/video/gpu-d3d.cpp
		src/hw/win32/video/gpu-sdl.cpp
		src/hw/win32/video/video.cpp
		src/hw/win32/video/vidutil.cpp
		src/hw/win32/audio/audio.cpp
		src/hw/win32/audio/directsound.cpp
		src/hw/win32/file/file.cpp
		src/hw/win32/res/icon.rc
		src/hw/win32/res/info.rc
		src/hw/win32/window-mgr.cpp
		src/hw/win32/system.cpp
		src/hw/win32/win_platform.cpp
		src/common/util/hash.cpp
		src/common/util/log.cpp
		src/common/util/misc.cpp
		src/common/util/text.cpp
		src/common/util/tween.cpp
		src/common/services/services.cpp
		src/common/services/storage.cpp
		src/common/services/appmgr.cpp
		src/common/services/sys/timer.cpp
		src/app/app.cpp
		src/app/builtin/errorscr/errorscr.cpp
		src/app/builtin/loadscr/loadscr.cpp

		src/app/ps1/biosldr.cpp
		src/app/ps1/shellapp.cpp
	)
else()
    add_executable(

	)
endif()

target_include_directories(
	main PRIVATE
	src
)
target_compile_options(
	main PRIVATE
	-Wall
	-Wextra
	-Wno-unused-parameter
	$<$<COMPILE_LANGUAGE:CXX>:
		-Wno-pmf-conversions
	>
	-g
)
target_compile_features(
	main PRIVATE
	cxx_std_20
)
target_compile_definitions(
	main PRIVATE
	VERSION="${PROJECT_VERSION}"
	EXTERNAL_DATA_DIR="hdd:/${PROJECT_NAME}"
	ENABLE_APP_LOGGING=1
	ENABLE_FS_LOGGING=1
	ENABLE_LOG_BUFFER=1
)

if(WIN32)
    set(COMMON_LIBS winmm user32)

    if(MINGW)
        target_link_libraries(main PRIVATE ${COMMON_LIBS} dsound dxguid d3d9 d3dx9 gdi32 winmm)
    elseif(MSVC)
        target_link_libraries(main PRIVATE ${COMMON_LIBS} dsound dxguid d3d9 d3dx9 Msimg32 winmm)
    endif()
endif()

