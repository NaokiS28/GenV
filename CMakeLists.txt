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

cmake_minimum_required(VERSION 3.29)

#execute_process(COMMAND ${CMAKE_COMMAND} -E environment)

project(
	GenV
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

if(GENV_PLATFORM STREQUAL "PSX")
    # Set up compiler flags and initialize the Python environment used to run the
    # scripts in the tools directory.
	message(STATUS "files are being included?")
    include(${CMAKE_SOURCE_DIR}/cmake/psx/setup.cmake)
    include(${CMAKE_SOURCE_DIR}/cmake/psx/tools.cmake)
endif()

# Store current platform from preset
if(NOT DEFINED GENV_PLATFORM)
    message(FATAL_ERROR "GENV_PLATFORM is not set! Please use a CMake preset.")
endif()

## Main executable

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
	message(STATUS "BIG_ENDIAN")
else()
	message(STATUS "LITTLE_ENDIAN")
endif()

# GenV shared core headers
add_library(genv_common STATIC
	src/common/util/hash.cpp
	src/common/util/ifloat.cpp
	src/common/util/log.cpp
	src/common/util/misc.cpp
	src/common/util/text.cpp
	src/common/util/tween.cpp
	src/common/objects/file.cpp
	src/common/objects/sound.cpp
	src/common/objects/tile.cpp
	src/common/objects/sprite.cpp
	src/common/objects/texture.cpp
	src/common/objects/texture/missingtex.c
	src/common/formats/soundfile.cpp
	src/common/formats/riff.cpp
	src/common/formats/wav.cpp
	src/common/services/services.cpp
	src/common/services/storage.cpp
	src/common/services/appmgr.cpp
	src/common/services/sys/timer.cpp
	src/app/app.cpp
	src/app/builtin/errorscr/errorscr.cpp
	src/app/builtin/loadscr/loadscr.cpp
	src/app/builtin/textload/textload.cpp	
	src/app/builtin/tmss/tmss.cpp	
)
target_include_directories(genv_common PUBLIC
    ${CMAKE_SOURCE_DIR}/src/
)

add_subdirectory(src/hw)

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(STATUS "Building default GenV test app")

	add_library(game_code OBJECT
		src/example/example.cpp		# Demo example
	)

	target_include_directories(
		game_code PRIVATE
		${CMAKE_SOURCE_DIR}/src/
	)

    add_executable(
		genv
        src/main.cpp                      # Engine main
    )

	target_compile_features(
		genv PRIVATE
		cxx_std_20
	)

	target_compile_definitions(
		genv PRIVATE
		VERSION="${PROJECT_VERSION}"
		EXTERNAL_DATA_DIR="hdd:/${PROJECT_NAME}"
		ENABLE_APP_LOGGING=1
		ENABLE_FS_LOGGING=1
		ENABLE_LOG_BUFFER=1
	)

    target_link_libraries(genv PRIVATE game_code genv_common)

	message(STATUS "Building for: ${GENV_PLATFORM}")

	if(GENV_PLATFORM STREQUAL "PC")
    	target_link_libraries(genv PRIVATE hw_win32)
	elseif(GENV_PLATFORM STREQUAL "PSX")
		add_subdirectory(${CMAKE_SOURCE_DIR}/lib/psx)
    	target_link_libraries(genv PRIVATE hw_psx psx_common)
	else()
        message(FATAL_ERROR "No supported platform backend selected.")
    endif()

    #target_include_directories(genv_main PRIVATE ../include)
endif()