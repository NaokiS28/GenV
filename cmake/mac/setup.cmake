# ps1-bare-metal - (C) 2023-2024 spicyjpeg
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
# OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

cmake_minimum_required(VERSION 3.25)

# Copy the macOS clangd config to the workspace root so clangd picks it up.
file(
    COPY_FILE
    "${CMAKE_CURRENT_LIST_DIR}/../clangd/mac.clangd"
    "${CMAKE_SOURCE_DIR}/.clangd"
)

add_library   (flags INTERFACE)
link_libraries(flags)

# Link required Apple frameworks for Metal/AppKit/CoreFoundation/QuartzCore.
find_library(FW_APPKIT        AppKit        REQUIRED)
find_library(FW_COREFOUNDATION CoreFoundation REQUIRED)
find_library(FW_METAL         Metal         REQUIRED)
find_library(FW_QUARTZCORE    QuartzCore    REQUIRED)
link_libraries(${FW_APPKIT} ${FW_COREFOUNDATION} ${FW_METAL} ${FW_QUARTZCORE})

target_compile_options(
	flags INTERFACE
	-Wall
	-Wextra
	-Wno-unused-parameter
	$<$<COMPILE_LANGUAGE:CXX>:
		-Wno-pmf-conversions
	>
	$<$<COMPILE_LANGUAGE:OBJCXX>:
		-fobjc-arc
	>
	-g
	-DGENV_MACOSX
	-DGENV_COMPUTER
	# Inform the SDK that we target macOS 11+ so that API_AVAILABLE /
	# __API_UNAVAILABLE guards apply to anything older than 11.0.
	-DMAC_OS_X_VERSION_MIN_REQUIRED=110000
	-DMAC_OS_X_VERSION_MAX_ALLOWED=110000
	$<IF:$<CONFIG:Debug>,
		-Og
	,
		#-O3
		#-flto
	>
)