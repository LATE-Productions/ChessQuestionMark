cmake_minimum_required(VERSION 3.20)
project(ChessQuestionMark
	LANGUAGES
		C CXX
	VERSION
		0.0.1
)
set(GDEXTENSION_LIB_NAME ${PROJECT_NAME})
set(GDEXTENSION_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/project/bin")
set(GODOT_PROJECT_DIR "project" CACHE STRING "The directory of a Godot project folder")

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)


if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/extern/godot-cpp/src")
	message(NOTICE "godot-cpp bindings source not found")
	message(NOTICE "initializing/updating the godot-cpp submodule...")

	# update the c++ bindings submodule to populate it with the necessary source for the library
	execute_process(
								COMMAND git submodule update --init godot-cpp/extern
								WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/extern
								COMMAND_ERROR_IS_FATAL ANY
				)
endif()
add_subdirectory(extern/godot-cpp SYSTEM)

file(
	MAKE_DIRECTORY
	"${CMAKE_CURRENT_BINARY_DIR}/extern/godot-cpp/gen/include"
)

list(APPEND CMAKE_MODULE_PATH
	"${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

set(compiler_is_clang "$<OR:$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:Clang>>")
set(compiler_is_gnu "$<CXX_COMPILER_ID:GNU>")
set(compiler_is_msvc "$<CXX_COMPILER_ID:MSVC>")
set(GODOTCPP_TARGET "$<IF:$<CONFIG:Debug>,template_debug,template_release>")

file(GLOB_RECURSE source_files
					CONFIGURE_DEPENDS
					"${PROJECT_SOURCE_DIR}/src/*.[hc]"
					"${PROJECT_SOURCE_DIR}/src/*.[hc]pp"
)


add_library(${PROJECT_NAME} SHARED ${source_files})

file(GLOB_RECURSE DOC_XML LIST_DIRECTORIES NO CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/doc_classes/*.xml")

# conditionally add doc data to compile output
if(DOC_XML)
	if(GODOTCPP_TARGET MATCHES "editor|template_debug")
		target_doc_sources(${PROJECT_NAME} "${DOC_XML}")
	endif()
endif()

target_include_directories(ChessQuestionMark
	PRIVATE
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/src
)
target_link_libraries(ChessQuestionMark
	PUBLIC
		godot::cpp
	PRIVATE
		${OpenCV_LIBS}
	)

target_compile_options(${PROJECT_NAME} PRIVATE
				$<${compiler_is_msvc}:
								/EHsc
								/utf-8
								/Zc:preprocessor
				>
				$<$<NOT:${compiler_is_msvc}>:
								-Wno-unused-value
								$<${compiler_is_gnu}:
												-Wno-attributes
												-Wno-attributes=rl::
								>
								$<${compiler_is_clang}:
												-Wno-ignored-attributes
												-Wno-unknown-attributes
								>
								$<$<CONFIG:Debug>:
												-g
												-fno-omit-frame-pointer
												-O0
								>
								$<$<CONFIG:Release>:
												-O3
								>
				>
)

set(GODOT_PROJECT_BINARY_DIR "${PROJECT_SOURCE_DIR}/${GODOT_PROJECT_DIR}/bin/${GODOTCPP_PLATFORM}")

set_target_properties(${PROJECT_NAME} PROPERTIES
				LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
				RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
				OUTPUT_NAME "${PROJECT_NAME}${GODOTCPP_SUFFIX}"
				PREFIX "lib"
)

# Copy from build/Debug/ or build/Release/ into project/bin/
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
				COMMAND ${CMAKE_COMMAND} -E copy_if_different
								"$<TARGET_FILE:${PROJECT_NAME}>"
								"${PROJECT_SOURCE_DIR}/project/bin/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
				COMMENT "Copying GDExtension library to project/bin"
)

include(cmake/godot_cmake.cmake)
gdextension(${PROJECT_NAME})

