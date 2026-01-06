# Konami 573

cmake_minimum_required(VERSION 3.25)

## Konami System 573 Library
add_subdirectory(../common ../common)

add_library(genv_hw_sys573 STATIC)

target_sources(genv_hw_sys573 PRIVATE
    system.cpp
	rtc.cpp
	asic.c
	io/io.c
	jvs/h8.c
	jamma/jamma.cpp
	jamma/adc083x.cpp
	storage/flash.c
	#storage/ide.c
	#jvs/jvs.cpp
	#aio/aio.cpp
	#dio/dio.cpp
	halt/halt.c
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

target_link_libraries(genv_hw_sys573 PRIVATE genv_psx_core genv_common genv_util genv_terminal)
