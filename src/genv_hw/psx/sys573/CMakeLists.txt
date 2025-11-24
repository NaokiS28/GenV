# Konami 573

cmake_minimum_required(VERSION 3.25)

## Konami System 573 Library

# Always generate compile_commands.json when building. This allows some IDEs and
# tools (such as clangd) to automatically configure include directories and
# other options.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(../core ../core)

add_library(genv_hw_sys573 STATIC)

target_sources(genv_hw_sys573 PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/system.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/rtc.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/jamma/jamma.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/jamma/adc083x.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/jvs/jvs.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/aio/aio.cpp
	#${CMAKE_CURRENT_SOURCE_DIR}/dio/dio.cpp
)

target_compile_definitions(
	genv_hw_sys573 PRIVATE
	GENV_PSX_VRAM_SIZE=2
)

target_include_directories(
    genv_hw_sys573 PRIVATE
	${CMAKE_SOURCE_DIR}/src/hw/psx
	${CMAKE_SOURCE_DIR}/src/genv_common
	${LIBC_INCLUDE}
)

target_link_libraries(genv_hw_sys573 PRIVATE genv_hw_psx genv_common genv_util genv_terminal)
