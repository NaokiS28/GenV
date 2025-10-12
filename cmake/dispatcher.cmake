# Define a "dictionary" mapping platform names to directories
set_property(GLOBAL PROPERTY PLATFORM_PATHS
    psx       "${CMAKE_SOURCE_DIR}/src/genv_hw/psx"
    573       "${CMAKE_SOURCE_DIR}/src/genv_hw/psx/573"
    win32     "${CMAKE_SOURCE_DIR}/src/genv_hw/windows"
)

function(resolve_platform platform out_var)
    get_property(paths GLOBAL PROPERTY PLATFORM_PATHS)

    # Find the key in the list
    list(FIND paths "${platform}" index)
    if(index EQUAL -1)
        message(FATAL_ERROR "Unknown platform: ${platform}")
    endif()

    # Value is the next list entry after the key
    math(EXPR val_index "${index} + 1")
    list(GET paths ${val_index} result)

    set(${out_var} "${result}" PARENT_SCOPE)
endfunction()
