# GenV - Copyright (C) 2025 NaokiS, spicyjpeg
# CMakeLists.txt - Created on 27-02-2026
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

## PS1 Halt Screen Library
#
# This is a fully self-contained static library. It has no dependencies on
# the rest of GenV - it only touches hardware registers directly. It must
# remain that way so it can be called safely after a fatal error where the
# rest of the system state is considered invalid.
#
# Font data
# ---------
# halt.c expects two symbols to be linked in:
#   _halt_font_image   - raw 4bpp pixel data for the 96x56 font spritesheet
#   _halt_font_palette - raw 16-entry 16bpp CLUT (32 bytes)
#
# These are provided by an assembly stub that .incbin's the converted image
# and palette files. Convert the source PNG with a tool such as img2tim or
# mkpsxiso's mkpsxiso-lztools, or use ps1-bare-metal's python helper to
# produce the raw 4bpp + palette binary files, then point the variables below
# at the resulting files.
#
# TODO: add a CMake custom_command here to run the conversion automatically
# once the source image asset is checked in. For now the converted .bin files
# are expected to be pre-generated and placed alongside this CMakeLists.txt.

add_library(genv_halt_screen STATIC)

include(${CMAKE_SOURCE_DIR}/cmake/psx/tools.cmake)

target_sources(genv_halt_screen PRIVATE
    src/halt.c
    src/sio1.c
	src/unhandledexc.c
    src/ps1/sys.c
    src/ps1/sys.s
)

target_compile_options(
	genv_halt_screen PRIVATE
	#-Wl
	#--whole-archive
)

target_include_directories(genv_halt_screen PUBLIC
    # Expose this directory so consumers can #include "halt.h" via the path
    # "psx/common/halt/halt.h" relative to ${CMAKE_SOURCE_DIR}/src/genv_hw
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${VENDOR_INCLUDE}
    ${LIBC_INCLUDE}
)

convertImage(
	${CMAKE_CURRENT_SOURCE_DIR}/font.png 4
	${CMAKE_CURRENT_BINARY_DIR}/fontTexture.dat
	${CMAKE_CURRENT_BINARY_DIR}/fontPalette.dat
)

addBinaryFile(genv_halt_screen _halt_font_image "${CMAKE_CURRENT_BINARY_DIR}/fontTexture.dat")
addBinaryFile(genv_halt_screen _halt_font_palette "${CMAKE_CURRENT_BINARY_DIR}/fontPalette.dat")