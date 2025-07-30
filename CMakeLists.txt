# Project: Vigfell - Copyright (C) 2025 NaokiS
# CMakeLists.txt - Created on 25-07-2025
# 

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
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

find_package(Python3 REQUIRED COMPONENTS Interpreter)

# Store current platform from preset
if(NOT DEFINED GENV_PLATFORM)
    message(FATAL_ERROR "GENV_PLATFORM is not set! Please use a CMake preset.")
endif()

if(CONFIG EQUAL "Debug")
	add_definitions(ENABLE_APP_LOGGING)
endif()

message(STATUS "Building for: ${GENV_PLATFORM}")
include(cmake/${GENV_PLATFORM}/setup.cmake)
include(cmake/${GENV_PLATFORM}/tools.cmake)


## Main executable

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${GENV_PLATFORM}")

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
	message(STATUS "BIG_ENDIAN")
else()
	message(STATUS "LITTLE_ENDIAN")
endif()

#set(GENV_PLATFORM win CACHE STRING "Target platform to build for (win or psx)")

if(NOT COMMAND addGenVExecutable)
    # use default add_executable() if it hasn't been overridden by the toolchain script
    function(addGenVExecutable name)
        add_executable(${name} ${ARGN})
    endfunction()
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    # add global dependency on bare metal libc (must be done here)
    add_subdirectory(lib/libc)
    link_libraries(libc)
endif()

add_subdirectory(src/genv_common)
add_subdirectory(src/genv_hw/${GENV_PLATFORM})
target_link_libraries(genv_common PRIVATE genv_hw_${GENV_PLATFORM})

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(STATUS "Building default GenV test app")

	add_subdirectory(src/example)
	target_link_libraries(game_code PRIVATE genv_common)

	addGenVExecutable(genv src/main.cpp ...)
	target_link_libraries(genv PRIVATE genv_common game_code)

	target_compile_definitions(
		genv PRIVATE
		VERSION="${PROJECT_VERSION}"
		EXTERNAL_DATA_DIR="hdd:/${PROJECT_NAME}"
		ENABLE_APP_LOGGING=1
		ENABLE_FS_LOGGING=1
		ENABLE_LOG_BUFFER=1
	)

endif()