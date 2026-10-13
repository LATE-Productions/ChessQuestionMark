cmake_minimum_required(VERSION 3.20)
if(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	if(NOT DEFINED ENV{VCPKG_ROOT})
		message(FATAL_ERROR "VCPKG_ROOT environment variable not defined")
	endif()
	set(VCPKG_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake")
	if(NOT EXISTS "${VCPKG_TOOLCHAIN_FILE}")
		message(FATAL_ERROR "VCPKG_ROOT defined but toolchain not found")
	endif()
	set(
		CMAKE_TOOLCHAIN_FILE "${VCPKG_TOOLCHAIN_FILE}"
		CACHE STRING "vcpkg toolchain file"
	)
endif()

if(UNIX AND NOT APPLE)
	set(VCPKG_OVERLAY_TRIPLETS "${CMAKE_CURRENT_SOURCE_DIR}/triplets")
	set(VCPKG_TARGET_TRIPLET "x64-linux-pic")
	list(APPEND CMAKE_MODULE_PATH "$ENV{VCPKG_ROOT}/installed/x64-linux/share/ffmpeg")
endif()

project(MarkerPipeline
	LANGUAGES CXX
	VERSION 0.0.1
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

find_package(OpenCV REQUIRED)

file(GLOB_RECURSE source_files
	CONFIGURE_DEPENDS
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

add_library(${PROJECT_NAME} STATIC )
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_sources(${PROJECT_NAME} PRIVATE ${source_files})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

add_executable(marker_pipeline)
target_sources(marker_pipeline
	PRIVATE
		main.cpp
)
target_link_libraries(marker_pipeline PRIVATE ${PROJECT_NAME})


