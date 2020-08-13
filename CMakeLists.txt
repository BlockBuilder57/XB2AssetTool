cmake_minimum_required(VERSION 3.1.0)
project(xb2at CXX)

include(CheckCXXSourceCompiles)
find_package(Threads)

# For people who want to import xb2core
option(XB2AT_XB2CORE_ONLY "Only build/include xb2core" OFF)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(MSVC)
	# It's 2020. Get with the program and stop faking C++98
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /D_CRT_SECURE_NO_WARNINGS")
endif()

# Detect the particular C++ standard library.
if(NOT DEFINED CACHE{XB2AT_STDLIB})
	message(STATUS "Detecting C++ standard library")
	set(CMAKE_REQUIRED_QUIET ON)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(__GLIBCXX__)
		#error \"not libstdc++\"
		#endif
		int main() {}"
	__LIBRARY_LIBSTDCXX)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(_LIBCPP_VERSION)
		#error \"not libc++\"
		#endif
		int main() {}"
	__LIBRARY_LIBCXX)

	check_cxx_source_compiles(
		"#include <ciso646>
		#if !defined(_MSVC_STL_VERSION)
		#error \"not MSVC\"
		#endif
		int main() {}"
	__LIBRARY_MSVC)

	if(__LIBRARY_LIBSTDCXX)
		set(STANDARD_LIBRARY "libstdc++")
	elseif(__LIBRARY_LIBCXX)
		set(STANDARD_LIBRARY "libc++")
	elseif(__LIBRARY_MSVC)
		set(STANDARD_LIBRARY "msvc")
	else()
		message(FATAL_ERROR "Can't find which C++ runtime is in use")
	endif()

	set(XB2AT_STDLIB ${STANDARD_LIBRARY} CACHE STRING "The C++ standard library in use; possible values: libstdc++, libc++, msvc")

	message(STATUS "Detected ${XB2AT_STDLIB}")
endif()

add_subdirectory(vendor)

add_custom_target(__xb2at_gittag COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/scripts/gittag.cmake WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${PROJECT_BINARY_DIR}/version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/version.h PROPERTIES GENERATED TRUE)

add_subdirectory(src/core)

if(NOT XB2AT_XB2CORE_ONLY)
	add_subdirectory(src/ui)
endif()