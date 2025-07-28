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

# Override the default file extensions for executables and libraries. This is
# not strictly required, but it makes CMake's behavior more consistent.
set(CMAKE_EXECUTABLE_SUFFIX     .exe)
set(CMAKE_STATIC_LIBRARY_PREFIX lib)
set(CMAKE_STATIC_LIBRARY_SUFFIX .a)

# Add libgcc.a (-lgcc) to the set of libraries linked to all executables by
# default. This library ships with GCC and must be linked to anything compiled
# with it.
link_libraries(-lgcc)

# Create a dummy "flags" library that is not made up of any files, but adds the
# appropriate compiler and linker flags for PS1 executables to anything linked
# to it. The library is then added to the default set of libraries.
add_library   (flags INTERFACE)
link_libraries(flags)

target_compile_features(
	flags INTERFACE
	c_std_17
	cxx_std_20
)
target_compile_options(
	flags INTERFACE
	-Wall
	-Wextra
	-Wno-unused-parameter
	$<$<COMPILE_LANGUAGE:CXX>:
		-Wno-pmf-conversions		
    >
	-g
		-DGENV_WIN32
	$<IF:$<CONFIG:Debug>,
		# These options will only be added if CMAKE_BUILD_TYPE is set to Debug.
		-Og
	,
		# These options will be added if CMAKE_BUILD_TYPE is not set to Debug.
		#-O3
		#-flto
		-mno-check-zero-division
	>
)