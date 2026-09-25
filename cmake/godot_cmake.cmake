cmake_minimum_required(VERSION 3.20)

get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
set(GODOTCPP_SUFFIX ${GODOTCPP_SUFFIX} CACHE INTERNAL "Godot CPP Suffix")

set(GODOTCPPMAKER_DIR ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "Godot CPP Maker Directory")

function(gdextension library_target)
	if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/project")
		set(OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/project/bin")
	else()
		set(OUT_DIR "${CMAKE_BINARY_DIR}/bin")
	endif()

	target_link_libraries(${library_target} PRIVATE godot::cpp)
	set_target_properties(${library_target} PROPERTIES
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
	configure_file(
		"${GODOTCPPMAKER_DIR}/gdextension.in"
		"${OUT_DIR}/${library_target}.gdextension"
		@ONLY
	)
endfunction()
