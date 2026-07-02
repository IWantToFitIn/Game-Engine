find_program(GLSLC_EXE NAMES glslc REQUIRED)
find_package(Python COMPONENTS Interpreter REQUIRED)

function(add_shader_headers SHADER_FILES)
	set(GENERATED_HEADERS "")
	foreach(SHADER_FILE ${SHADER_FILES})
		get_filename_component(SHADER_FILE_ABS ${SHADER_FILE} ABSOLUTE)
		get_filename_component(FILE_NAME ${SHADER_FILE} NAME_WE)
		get_filename_component(EXT ${SHADER_FILE} EXT)
		if(EXT STREQUAL ".vert")
			set(SUFFIX "Vertex")
		elseif(EXT STREQUAL ".frag")
			set(SUFFIX "Fragment")
		elseif(EXT STREQUAL ".comp")
			set(SUFFIX "Compute")
		else()
			message(FATAL_ERROR "Unknown shader extension ${EXT}")
		endif()

		set(SPV_FILE "${GEN_OUT_DIR}/${FILE_NAME}${SUFFIX}.spv")
		set(HPP_FILE "${GEN_OUT_DIR}/${FILE_NAME}${SUFFIX}.hpp")
		
		add_custom_command(
			OUTPUT ${SPV_FILE}
			COMMAND ${GLSLC_EXE} ${SHADER_FILE_ABS} -o ${SPV_FILE}
			DEPENDS ${SHADER_FILE_ABS}
			COMMENT "Compiling shader ${FILE_NAME}${EXT} to spirv"
			VERBATIM
		)
		add_custom_command(
			OUTPUT ${HPP_FILE}
			COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/generateShaders.py ${SPV_FILE} ${HPP_FILE}
			DEPENDS ${SPV_FILE} ${CMAKE_CURRENT_SOURCE_DIR}/generateShaders.py
			COMMENT "Converting ${FILE_NAME}${SUFFIX}.spv to header"
			VERBATIM
		)

		list(APPEND GENERATED_HEADERS ${HPP_FILE})
	endforeach()
	set(GENERATOR_TARGET shader_generator)
	add_custom_target(${GENERATOR_TARGET} DEPENDS ${GENERATED_HEADERS})
	add_library(shaders INTERFACE)
	add_dependencies(shaders ${GENERATOR_TARGET})
endfunction()

file(GLOB SHADERS CONFIGURE_DEPENDS
	"./*.vert"
	"./*.frag"
	"./*.comp"
)
add_shader_headers("${SHADERS}")