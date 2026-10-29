cmake_minimum_required(VERSION 3.20)
include(cmake/vcpkg_config.cmake)

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

add_library(${PROJECT_NAME} STATIC)
target_include_directories(${PROJECT_NAME}
	PUBLIC
		${OpenCV_INCLUDE_DIRS}
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_sources(${PROJECT_NAME} PRIVATE ${source_files})
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})

add_executable(marker_pipeline main.cpp)
target_link_libraries(marker_pipeline PRIVATE ${PROJECT_NAME})

add_executable(camera_calibration camera_calibration/main.cpp)
target_include_directories(${PROJECT_NAME}
	PUBLIC
		camera_calibration
		${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
