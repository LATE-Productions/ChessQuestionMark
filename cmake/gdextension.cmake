# copied and modified from https://github.com/yehuthi/godot_cpp_cmaker/

set(GODOTCPP_SUFFIX ${GODOTCPP_SUFFIX} CACHE INTERNAL "Godot CPP Suffix")

function(gdextension library_target)
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/project")
		set(OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project/bin")
	else()
		set(OUT_DIR "${CMAKE_BINARY_DIR}/build")
	endif()

	set_target_properties(${library_target} PROPERTIES
		PREFIX "lib"
		OUTPUT_NAME "${library_target}${GODOTCPP_SUFFIX}"
		# Linux
		LIBRARY_OUTPUT_DIRECTORY "${OUT_DIR}"
		# Windows
		RUNTIME_OUTPUT_DIRECTORY "${OUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_DEBUG "${OUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_RELEASE "${OUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${OUT_DIR}"
		RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${OUT_DIR}"
	)


	add_custom_command(TARGET ${library_target} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
								"$<TARGET_FILE:${library_target}>"
								"${CMAKE_CURRENT_SOURCE_DIR}/project/bin/$<TARGET_FILE_NAME:${library_target}>"
				COMMENT "Copying GDExtension library to project/bin"
	)

	configure_file(
		"${CMAKE_CURRENT_SOURCE_DIR}/gdextension.in"
		"${OUT_DIR}/${library_target}.gdextension"
		@ONLY
	)
endfunction()
