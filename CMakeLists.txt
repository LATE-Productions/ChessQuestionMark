cmake_minimum_required(VERSION 3.20)
project(ChessQuestionMark
	LANGUAGES	C CXX
	VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/$<CONFIG>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib/$<CONFIG>")

include(CTest)

file(GLOB_RECURSE source_files
	CONFIGURE_DEPENDS
		"${PROJECT_SOURCE_DIR}/src/*.[hc]"
		"${PROJECT_SOURCE_DIR}/src/*.[hc]pp"
)

add_library(${PROJECT_NAME} SHARED ${source_files})

include(cmake/godot-dev-configuration.cmake)
include(cmake/gdextension.cmake)
include(cmake/compile-options.cmake)

add_subdirectory(libs/marker_pipeline)

target_include_directories(ChessQuestionMark
	PRIVATE
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(UNIX AND NOT APPLE)
	target_link_options(${PROJECT_NAME} PUBLIC -Wl,-Bsymbolic)
endif()

target_link_libraries(ChessQuestionMark
	PUBLIC
		godot::cpp
	PRIVATE
		${OpenCV_LIBS}
		MarkerPipeline
	)

# Assume the executable target is ChessQuestionMark and the calibration
# is written to build/bin/<CONFIG>/camera_calibration.yaml by the calibrator.
set(CALIB_SRC "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/camera_calibration.yaml")
set(CALIB_DST "${CMAKE_CURRENT_SOURCE_DIR}/project/camera_calibration.yaml")
add_custom_command(TARGET ChessQuestionMark POST_BUILD
	COMMAND ${CMAKE_COMMAND}
		-D CALIB_SRC=${CALIB_SRC}
		-D CALIB_DST=${CALIB_DST}
		-P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy-camera-calibration.cmake
	COMMENT "Syncing camera calibration into project/"
)


gdextension(${PROJECT_NAME})
