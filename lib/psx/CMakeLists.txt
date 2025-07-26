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
	LANGUAGES    C CXX
	VERSION      1.0.0
	DESCRIPTION  ""
	HOMEPAGE_URL "https://github.com/NaokiS28/GenV"
)

## PS1 Bare Metal

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


include(cmake/setup.cmake)
include(cmake/tools.cmake)
add_library(psx_common OBJECT)

target_sources(
	libc/clz.s
	libc/crt0.c
	libc/cxxsupport.cpp
	libc/malloc.c
	libc/misc.c
	libc/setjmp.s
	libc/string.c
	libc/string.s
	ps1/cache.s
	vendor/printf.c
)

target_include_directories(
    psx_common PUBLIC
	src
	libc
)