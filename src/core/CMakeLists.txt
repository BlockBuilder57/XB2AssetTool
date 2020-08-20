# xb2core CMakeLists.txt

# we have header files here so that when they're changed
# they trigger a rebuild of xb2core source files
set(XB2CORE_SOURCES

# Core library nitty-gritty things
	core.h
	lowlevelmath.h
	streamhelper.h
	ivstream.h
	
# Cross-platform/cross-environment logging utility
	Logger.h
	Logger.cpp

# Data structures

	structs/xbc1.h
	structs/msrd.h
	structs/mesh.h
	structs/mxmd.h
	structs/mibl.h
	structs/sar1.h
	structs/skel.h

# File Readers

	readers/base_reader.h

	readers/xbc1_reader.h
	readers/xbc1_reader.cpp

	readers/msrd_reader.h
	readers/msrd_reader.cpp

	readers/mesh_reader.h
	readers/mesh_reader.cpp

	readers/mxmd_reader.h
	readers/mxmd_reader.cpp

	readers/mibl_reader.h
	readers/mibl_reader.cpp

	readers/sar1_reader.h
	readers/sar1_reader.cpp

	readers/skel_reader.h
	readers/skel_reader.cpp
	
# Serializers

	serializers/base_serializer.h

	serializers/model_serializer.h
	serializers/model_serializer.cpp
	
# Texture stuff.
# TODO move to serializers
	tegra_texture.h
	tegra_texture.cpp
)

# easy mode api is optional, 
# turned off by default since UI doesn't need it at all (it has its own mechanism)
if(XB2AT_XB2CORE_EASYMODE)
	message(STATUS "[xb2core] Including Easymode API surface")
	#set(XB2CORE_SOURCES
	#	${XB2CORE_SOURCES}
	#	easyapi/file.h
	#	easyapi/file.cpp
	#)
endif()

add_library(xb2core STATIC ${XB2CORE_SOURCES})
set_property(TARGET xb2core PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2core PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(xb2core PRIVATE ${PROJECT_SOURCE_DIR}/vendor/zlib)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR}/vendor/zlib)

# Allow projects that import xb2core to immediately get includes of core
target_include_directories(xb2core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xb2core zlibstatic nlohmann_json::nlohmann_json fx-gltf glm::glm)

# Link additional libraries required for using the filesystem library with libc++ and libstdc++.
#
# As a bonus, if this project is linked with a CMake project
# this will automatically be linked into the project depending on xb2core!
if(XB2AT_STDLIB STREQUAL "llvm")
	message(STATUS "Linking c++fs library")
	target_link_libraries(xb2core c++fs)
elseif(XB2AT_STDLIB STREQUAL "gnu")
	message(STATUS "Linking stdc++fs library")
	target_link_libraries(xb2core stdc++fs)
endif()

target_link_libraries(xb2core ${CMAKE_THREAD_LIBS_INIT})

# We use the version tag so depend on it and add a private include path to where it's generated
add_dependencies(xb2core __xb2at_gittag)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR})

# precompile core.h
target_precompile_headers(xb2core PUBLIC core.h)