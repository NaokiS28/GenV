# Konami GV Sytem

cmake_minimum_required(VERSION 3.25)

## Konami GV System Library
add_subdirectory(../common ../common)

add_library(genv_hw_gv999 STATIC)

target_sources(genv_hw_gv999 PRIVATE
    system.cpp
	io/io.cpp
	io/jamma/jamma.cpp
	storage/scsi.cpp
	halt/halt.c
)

target_compile_definitions(
	genv_hw_gv999 PRIVATE
)

target_include_directories(
    genv_hw_gv999 PUBLIC
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_gv999 PRIVATE genv_psx_core genv_common genv_util genv_terminal)
