cmake_minimum_required(VERSION 3.1.0)
project(xb2at CXX)

include(CheckCXXSourceCompiles)
find_package(Threads)

# For people who want to import xb2core
option(XB2AT_XB2CORE_ONLY "Only include the xb2core library. For 3rd-party projects using xb2core." OFF)

# defaulted to off
option(XB2AT_XB2CORE_EASYMODE "Include the Easymode API with xb2core" OFF)

option(XB2CORE_DO_NOT_INSTALL "Do not install xb2core. Default on due to inclusion in xb2at" ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(MSVC)
	# It's 2020. Get with the program and stop faking C++98.
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus /D_CRT_SECURE_NO_WARNINGS /EHsc")
endif()

# clang-cl & gcc optimizations
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" AND MSVC)
	if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
		# use /clang: to pass in options from godbolt analysis
		set(CMAKE_CXX_FLAGS_RELEASE "/MD /Oi /DNDEBUG /clang:-O3 /clang:-ffast-math")
		set(CMAKE_C_FLAGS_RELEASE "/MD /Oi /DNDEBUG /clang:-O3 /clang:-ffast-math")
	endif()
elseif("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang" OR "${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
	if("${CMAKE_BUILD_TYPE}" MATCHES "Release")
		set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ffast-math")
		set(CMAKE_C_FLAGS_RELEASE "-O3 -ffast-math")
	endif()
elseif(MSVC)
	if(NOT DEFINED CACHE{_XB2AT_MSVC_WARNING})
		set(_XB2AT_MSVC_WARNING "shown" CACHE STRING "")
		message(WARNING "MSVC builds may perform worse than Clang-cl ones.")
	endif()
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
		set(STANDARD_LIBRARY "gnu")
	elseif(__LIBRARY_LIBCXX)
		set(STANDARD_LIBRARY "llvm")
	elseif(__LIBRARY_MSVC)
		set(STANDARD_LIBRARY "msvc")
	else()
		message(FATAL_ERROR "Can't find which C++ runtime is in use")
	endif()

	set(XB2AT_STDLIB ${STANDARD_LIBRARY} CACHE STRING "The C++ standard library in use; possible values: gnu, llvm, msvc")

	message(STATUS "Detected ${XB2AT_STDLIB}")
endif()

add_subdirectory(vendor)

add_custom_target(__xb2at_gittag COMMAND ${CMAKE_COMMAND} -P ${PROJECT_SOURCE_DIR}/scripts/gittag.cmake WORKING_DIRECTORY ${PROJECT_BINARY_DIR} SOURCES ${PROJECT_BINARY_DIR}/version.h)
set_source_files_properties(${PROJECT_BINARY_DIR}/version.h PROPERTIES GENERATED TRUE)

add_subdirectory(src/core)

if(NOT XB2AT_XB2CORE_ONLY)
	add_subdirectory(src/ui)
endif()