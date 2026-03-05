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

# ------------------------------------------------------------------------------
# Virtual Environment Configuration (MUST be before project())
# ------------------------------------------------------------------------------

# User-configurable virtual environment path
set(
    VENV_PATH "${CMAKE_HOME_DIRECTORY}/env"
    CACHE PATH "Directory containing Python virtual environment"
)

# Determine platform-specific python path
if(WIN32)
    set(VENV_PYTHON "${VENV_PATH}/Scripts/python.exe")
else()
    set(VENV_PYTHON "${VENV_PATH}/bin/python")
endif()

# Ensure the virtual environment python exists
if(NOT EXISTS "${VENV_PYTHON}")
    message(FATAL_ERROR
        "Python not found in virtual environment at ${VENV_PYTHON}.\n"
        "Create it first (python -m venv env) or pass -DVENV_PATH=..."
    )
endif()

# Clear any previously cached Python detection
unset(Python3_EXECUTABLE CACHE)
unset(Python3_ROOT_DIR CACHE)

# Force CMake to ONLY use this interpreter
set(Python3_EXECUTABLE "${VENV_PYTHON}" CACHE FILEPATH "" FORCE)
set(Python3_ROOT_DIR "${VENV_PATH}" CACHE PATH "" FORCE)

set(Python3_FIND_VIRTUALENV ONLY)
set(Python3_FIND_STRATEGY LOCATION)
set(Python3_FIND_REGISTRY NEVER)
set(Python3_FIND_FRAMEWORK NEVER)

# Now find Python (will be locked to VENV_PATH)
find_package(Python3 3.10 REQUIRED COMPONENTS Interpreter)

message(STATUS "Using Python interpreter: ${Python3_EXECUTABLE}")

# ------------------------------------------------------------------------------
# Project Definition
# ------------------------------------------------------------------------------

project(
    GenV
    LANGUAGES    C CXX ASM
    VERSION      0.1.0
    DESCRIPTION  ""
    HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 17)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# HARD disable modules
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

set(TODO_OUTPUT_DIR ${CMAKE_SOURCE_DIR})

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ------------------------------------------------------------------------------
# Platform Validation
# ------------------------------------------------------------------------------

if(NOT DEFINED GENV_PLATFORM)
    message(FATAL_ERROR "GENV_PLATFORM is not set! Please use a CMake preset.")
elseif(NOT DEFINED GENV_SUBPLATFORM)
    message(FATAL_ERROR "GENV_SUBPLATFORM is not set! Please use a CMake preset.")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DENABLE_APP_LOGGING)
endif()

message(STATUS "Building for: ${GENV_PLATFORM}/${GENV_SUBPLATFORM}")

include(cmake/${GENV_PLATFORM}/setup.cmake)
include(cmake/${GENV_PLATFORM}/tools.cmake)
include(cmake/util.cmake)

# ------------------------------------------------------------------------------
# Output Directories
# ------------------------------------------------------------------------------

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG
    "${CMAKE_SOURCE_DIR}/bin/debug/${GENV_PLATFORM}/${GENV_SUBPLATFORM}"
)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
    "${CMAKE_SOURCE_DIR}/bin/release/${GENV_PLATFORM}/${GENV_SUBPLATFORM}"
)

# ------------------------------------------------------------------------------
# Endianness Detection
# ------------------------------------------------------------------------------

include(TestBigEndian)
TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

if(IS_BIG_ENDIAN)
    message(STATUS "Target is Big Endian")
else()
    message(STATUS "Target is Little Endian")
endif()

# ------------------------------------------------------------------------------
# Default addGenVExecutable fallback
# ------------------------------------------------------------------------------

if(NOT COMMAND addGenVExecutable)
    function(addGenVExecutable name)
        add_executable(${name} ${ARGN})
    endfunction()
endif()

# ------------------------------------------------------------------------------
# Core Subdirectories
# ------------------------------------------------------------------------------

add_subdirectory(src/genv_common/terminal)
add_subdirectory(src/genv_common/halt_screen)

if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    add_subdirectory(lib/libc)
    add_subdirectory(lib/libbmcxx)
    link_libraries(libc)
    link_libraries(libbmcxx)
endif()

# ------------------------------------------------------------------------------
# Platform Dispatcher
# ------------------------------------------------------------------------------

if(IS_READABLE "${CMAKE_SOURCE_DIR}/cmake/${GENV_PLATFORM}/${GENV_SUBPLATFORM}.cmake")
    include("cmake/${GENV_PLATFORM}/${GENV_SUBPLATFORM}.cmake")
else()
    message(FATAL_ERROR "Unknown platform: ${GENV_PLATFORM}/${GENV_SUBPLATFORM}")
endif()

set(PLATFORM_PATH
    "${CMAKE_SOURCE_DIR}/src/genv_hw/${GENV_PLATFORM}/${GENV_SUBPLATFORM}"
)

add_subdirectory(${PLATFORM_PATH})

add_subdirectory(src/genv_common/common)
add_subdirectory(src/genv_common/app)
add_subdirectory(src/genv_common/common/objects/fonts)

# ------------------------------------------------------------------------------
# Default Test App
# ------------------------------------------------------------------------------

if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)

    message(STATUS "Building default GenV test app")

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(exe_folder "debug")
        set(exe_name "${CMAKE_PROJECT_NAME}-debug")
        set(MAP_PATH
            "${CMAKE_SOURCE_DIR}/bin/debug/${GENV_PLATFORM}/${GENV_SUBPLATFORM}"
        )
    else()
        set(exe_folder "release")
        set(exe_name "${CMAKE_PROJECT_NAME}")
        set(MAP_PATH
            "${CMAKE_SOURCE_DIR}/bin/release/${GENV_PLATFORM}/${GENV_SUBPLATFORM}"
        )
    endif()

    add_subdirectory(src/example)

    target_link_libraries(game_code PRIVATE
        genv_common
        genv_util
        genv_app_core
        genv_logger
    )

    addGenVExecutable(${exe_name} src/main.cpp)

    target_link_libraries(${exe_name} PRIVATE
        genv_app_manager
    )

    target_compile_definitions(${exe_name} PRIVATE
        VERSION="${PROJECT_VERSION}"
    )

	if(CMAKE_SYSTEM_NAME STREQUAL "Generic")
    target_link_options(${exe_name} PUBLIC
        LINKER:-Map=${MAP_PATH}/${exe_name}.map
    )
	endif()

endif()

# ------------------------------------------------------------------------------
# Copy compile_commands.json to build/ root for clangd fallback
# ------------------------------------------------------------------------------

# cmake writes compile_commands.json to CMAKE_BINARY_DIR. Copy it one level up
# to build/ so that the root .clangd fallback block always reflects the most
# recently configured platform.
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_BINARY_DIR}/compile_commands.json
        ${CMAKE_SOURCE_DIR}/build/compile_commands.json
    COMMENT "Updating build/compile_commands.json for clangd"
)

# ------------------------------------------------------------------------------
# Graphviz Build Graph
# ------------------------------------------------------------------------------

find_program(DOT_EXECUTABLE dot)

if (DOT_EXECUTABLE)
	add_custom_target(
		generate_build_graph ALL
        COMMAND ${DOT_EXECUTABLE}
                -Tpng
                ${CMAKE_BINARY_DIR}/graph/build_graph.dot
                -o
                ${CMAKE_BINARY_DIR}/build_graph.png
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
		COMMENT "Generating build_graph.dot and build_graph.png in ${CMAKE_BINARY_DIR}"
    )
endif()

# ------------------------------------------------------------------------------
# TODO Generator (Forced to use venv Python)
# ------------------------------------------------------------------------------

add_custom_target(generate_todos ALL
    COMMAND
        ${Python3_EXECUTABLE}
        ${CMAKE_SOURCE_DIR}/tools/generate_todos.py
        --ignore "lib/"
        --ignore "tools/"
        --ignore "TODO.md"
        ${TODO_OUTPUT_DIR}/TODO.md
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Compiling TODO list to ${TODO_OUTPUT_DIR}/TODO.md"
)