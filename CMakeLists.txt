cmake_minimum_required(VERSION 3.1.0)
project(xb2at CXX)

set(PROJECT_ROOT ${PROJECT_SOURCE_DIR})

if(CMAKE_VERSION VERSION_LESS "3.7.0")
	set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

if(MSVC)
	# It's 2020. Get with the program and stop faking C++98
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Zc:__cplusplus")
endif()

add_subdirectory(vendor)
add_subdirectory(src/core)
add_subdirectory(src/ui)