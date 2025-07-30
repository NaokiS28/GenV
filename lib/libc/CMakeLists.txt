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
	PS1-Bare-Metal
	LANGUAGES    C CXX ASM
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

## Bare Metal

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(
	libc STATIC
	libc/clz.s
	libc/precrt0.s
	libc/crt0.c
	libc/cxxsupport.cpp
	libc/malloc.c
	libc/serial.c
	libc/setjmp.s
	libc/string.c
	libc/string.s
	libc/random.c
	vendor/printf.c
)

target_include_directories(
    libc PUBLIC
	${CMAKE_SOURCE_DIR}/lib/libc
	${CMAKE_SOURCE_DIR}/lib/libc/libc
	${CMAKE_SOURCE_DIR}/lib/libc/vendor
)