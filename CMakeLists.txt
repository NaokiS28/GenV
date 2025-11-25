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
	VERSION      0.1.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_C_STANDARD 23)

find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Store current platform from preset
if(NOT DEFINED GENV_PLATFORM)
    message(FATAL_ERROR "GENV_PLATFORM is not set! Please use a CMake preset.")
elseif(NOT DEFINED GENV_SUBPLATFORM)
    message(FATAL_ERROR "GENV_SUBPLATFORM is not set! Please use a CMake preset.")
endif()

if(CONFIG EQUAL "Debug")
	add_definitions(ENABLE_APP_LOGGING)
endif()

message(STATUS "Building for: ${GENV_PLATFORM}/${GENV_SUBPLATFORM}")
include(cmake/${GENV_PLATFORM}/setup.cmake)
include(cmake/${GENV_PLATFORM}/tools.cmake)
include(cmake/util.cmake)

## Main executable
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/debug/${GENV_PLATFORM}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/release/${GENV_PLATFORM}")

include (TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)
if(IS_BIG_ENDIAN)
	message(STATUS "Target is Big Endian")
else()
	message(STATUS "Target is Little Endian")
endif()


if(NOT COMMAND addGenVExecutable)
    # use default add_executable() if it hasn't been overridden by the toolchain script
    function(addGenVExecutable name)
        add_executable(${name} ${ARGN})
    endfunction()
endif()

add_subdirectory(src/genv_common/terminal)

if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    # add global dependency on bare metal libc (must be done here)
    add_subdirectory(lib/libc)
    add_subdirectory(lib/libbmcxx)
    link_libraries(libc)
    link_libraries(libbmcxx)
    set(LIBC_INCLUDE 
		${CMAKE_SOURCE_DIR}/lib/libc
		${CMAKE_SOURCE_DIR}/lib/libbmcxx
	)
endif()

# System dispatcher
if(IS_READABLE "${CMAKE_SOURCE_DIR}/cmake/${GENV_PLATFORM}/${GENV_SUBPLATFORM}.cmake")
	include("cmake/${GENV_PLATFORM}/${GENV_SUBPLATFORM}.cmake")
else()
	message(FATAL_ERROR "Unknown platform: ${GENV_PLATFORM}/${GENV_SUBPLATFORM}")
endif()

set(PLATFORM_PATH "${CMAKE_SOURCE_DIR}/src/genv_hw/${GENV_PLATFORM}/${GENV_SUBPLATFORM}")
add_subdirectory(${PLATFORM_PATH})

add_subdirectory(src/genv_common/common)
add_subdirectory(src/genv_common/app)
add_subdirectory(src/genv_common/common/objects/fonts)


if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    message(STATUS "Building default GenV test app")

	add_subdirectory(src/example)
	target_link_libraries(game_code PRIVATE genv_common genv_util genv_app_core genv_services genv_logger)

	addGenVExecutable(genv src/main.cpp)
	target_link_libraries(genv PRIVATE genv_services genv_app_manager)

	target_compile_definitions(
		genv PRIVATE
		VERSION="${PROJECT_VERSION}"
	)

endif()

# Find Graphviz 'dot' executable
find_program(DOT_EXECUTABLE dot)

# Automatically generate dependency graph when CMake runs
if (DOT_EXECUTABLE)
    execute_process(
        COMMAND ${DOT_EXECUTABLE} -Tpng ${CMAKE_BINARY_DIR}/graph/build_graph.dot -o ${CMAKE_BINARY_DIR}/build_graph.png
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    )
    message(STATUS "Generated build_graph.dot and build_graph.png in ${CMAKE_BINARY_DIR}")
endif()
