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
