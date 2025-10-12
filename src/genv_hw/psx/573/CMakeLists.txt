# Konami 573

cmake_minimum_required(VERSION 3.25)

## PSX Core Library

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(genv_hw_sys573 STATIC)

target_sources(genv_hw_sys573 PRIVATE
    audio.cpp
	#file/file.cpp
	cpu/cache.s
	system/sys.c
	system/sys.s
	system/unhandledexc.c
	system/serial.c
	system/timer.c
	file/pcdrv.s
	audio/spu.cpp
    video/video.cpp
    video/texmgr.cpp
    video/psxtex.cpp
    573/sys573.cpp
	573/rtc.cpp
	573/jamma.cpp
	#573/jvs.cpp
	#573/aio.cpp
	#573/dio.cpp
)

target_compile_definitions(
	genv_hw_sys573 PRIVATE
	GENV_PSX_VRAM_SIZE=2
)

target_include_directories(
    genv_hw_sys573 PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_sys573 PRIVATE genv_common genv_util genv_terminal)
