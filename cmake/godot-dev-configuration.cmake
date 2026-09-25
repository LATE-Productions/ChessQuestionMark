set(GDEXTENSION_LIB_NAME ${PROJECT_NAME})
set(GDEXTENSION_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/project/bin")
set(GODOT_PROJECT_DIR "project" CACHE STRING "The directory of a Godot project folder")

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/extern/godot-cpp/src")
	message(NOTICE "godot-cpp bindings source not found")
	message(NOTICE "initializing/updating the godot-cpp submodule...")

	# update the c++ bindings submodule to populate it with the necessary source for the library
	execute_process(
		COMMAND git submodule update --init godot-cpp/extern
		WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/extern
		COMMAND_ERROR_IS_FATAL ANY
	)
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/extern/godot-cpp SYSTEM)

get_target_property(GODOTCPP_SUFFIX godot::cpp GODOTCPP_SUFFIX)
get_target_property(GODOTCPP_PLATFORM godot::cpp GODOTCPP_PLATFORM)

file(
	MAKE_DIRECTORY
	"${CMAKE_CURRENT_BINARY_DIR}/extern/godot-cpp/gen/include"
)

list(APPEND CMAKE_MODULE_PATH
	"${godot-cpp_SOURCE_DIR}/cmake")
include(GodotCPPModule)

file(GLOB_RECURSE
	DOC_XML
	LIST_DIRECTORIES
	NO CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/doc_classes/*.xml"
)

# conditionally add doc data to compile output
if(DOC_XML)
	if(GODOTCPP_TARGET MATCHES "editor|template_debug")
		target_doc_sources(${PROJECT_NAME} "${DOC_XML}")
	endif()
endif()

