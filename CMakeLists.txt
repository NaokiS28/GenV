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

find_package(Python3 REQUIRED COMPONENTS Interpreter)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


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
include(cmake/util.cmake)


## Main executable

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin/${GENV_PLATFORM}")

# Enable compile commands for completeness (optional)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_subdirectory(src/genv_common/terminal)

if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    # add global dependency on bare metal libc (must be done here)
    add_subdirectory(lib/libc)
    link_libraries(libc)
    set(LIBC_INCLUDE ${CMAKE_SOURCE_DIR}/lib/libc)
endif()

add_subdirectory(src/genv_common/common)
add_subdirectory(src/genv_common/app)
add_subdirectory(src/genv_common/common/objects/fonts)

# System dispatcher
include(cmake/dispatcher.cmake)
resolve_platform(${GENV_PLATFORM} PLATFORM_PATH)
add_subdirectory(${PLATFORM_PATH})

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