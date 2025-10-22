# Orginally part of ps1-bare-metal - (C) 2023-2025 spicyjpeg

cmake_minimum_required(VERSION 3.25)

# Define a helper function to embed binary data into executables and libraries.
function(addBinaryFile target name path)
	set(asmFile "${PROJECT_BINARY_DIR}/includes/${target}_${name}.s")
	cmake_path(ABSOLUTE_PATH path OUTPUT_VARIABLE fullPath)

	# Generate an assembly listing that uses the .incbin directive to embed the
	# file and add it to the executable's list of source files. This may look
	# hacky, but it works and lets us easily customize the symbol name (i.e. the
	# name of the "array" that will contain the file's data).
	file(
		CONFIGURE
		OUTPUT  "${asmFile}"
		CONTENT [[
.section .rodata.${name}, "a"
.balign 8

.global ${name}
.type ${name}, @object
.size ${name}, (${name}_end - ${name})

${name}:
	.incbin "${fullPath}"
${name}_end:
]]
		ESCAPE_QUOTES
		NEWLINE_STYLE LF
	)

	target_sources(${target} PRIVATE "${asmFile}")
	set_source_files_properties(
		"${asmFile}" PROPERTIES OBJECT_DEPENDS "${fullPath}"
	)
endfunction()

function(addBinaryFileWithSize target name sizeName path)
	set(asmFile "${PROJECT_BINARY_DIR}/includes/${target}_${name}.s")
	cmake_path(ABSOLUTE_PATH path OUTPUT_VARIABLE fullPath)

	file(
		CONFIGURE
		OUTPUT  "${asmFile}"
		CONTENT [[
.section .rodata.${name}, "a"
.balign 8

.global ${name}
.type ${name}, @object
.size ${name}, (${name}_end - ${name})

${name}:
	.incbin "${fullPath}"
${name}_end:

.section .rodata.${sizeName}, "a"
.balign 4

.global ${sizeName}
.type ${sizeName}, @object
.size ${sizeName}, 4

${sizeName}:
	.int (${name}_end - ${name})
]]
		ESCAPE_QUOTES
		NEWLINE_STYLE LF
	)

	target_sources(${target} PRIVATE "${asmFile}")
	set_source_files_properties(
		"${asmFile}" PROPERTIES OBJECT_DEPENDS "${fullPath}"
	)
endfunction()
