# GenV - (C) 2023-2024 NaokiS, spicyjpeg
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

# Create a user-editable variable to allow for a custom toolchain path to be
# specified by passing -DTOOLCHAIN_PATH=... to CMake.
set(
	TOOLCHAIN_PATH ""
	CACHE PATH "Directory containing GCC toolchain (if not listed in PATH)"
)

# Prevent CMake from using any host compiler by manually overriding the platform
# and setting it to "generic" (i.e. no defaults).
set(CMAKE_SYSTEM_NAME Darwin)

# Use the host's Clang from Xcode/CLT — no cross-compiler needed.
# CMAKE_OSX_ARCHITECTURES is set in the preset to "arm64;x86_64".
# CMAKE_OSX_DEPLOYMENT_TARGET is also set in the preset.

find_program(clangPath clang REQUIRED)
set(CMAKE_C_COMPILER   "${clangPath}" CACHE STRING "" FORCE)

find_program(clangxxPath clang++ REQUIRED)
set(CMAKE_CXX_COMPILER "${clangxxPath}" CACHE STRING "" FORCE)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Ensure OSX_ARCHITECTURES is set before any compiler tests run.
# The preset also sets this, but toolchain variables are needed earlier.
if(NOT CMAKE_OSX_ARCHITECTURES)
	set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64" CACHE STRING "" FORCE)
endif()

# Use the default macOS SDK provided by the active Xcode / CLT installation.
# x86_64 is still a supported target in current Xcode releases.
find_program(xcrunPath xcrun REQUIRED)
execute_process(
	COMMAND "${xcrunPath}" --show-sdk-path
	OUTPUT_VARIABLE osxSdkPath
	OUTPUT_STRIP_TRAILING_WHITESPACE
)
set(CMAKE_OSX_SYSROOT "${osxSdkPath}" CACHE PATH "" FORCE)