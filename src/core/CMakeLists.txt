# xb2core library

# we have header files here so that when they're changed
# they trigger a rebuild of xb2core source files
set(XB2CORE_SOURCES

	# Core library things
	core.h
	endian_swap.h
	streamhelper.h
	ivstream.h
	base_progress_reporter.h

	# Data structs
	structs.h
	structs/xbc1.h
	structs/msrd.h
	structs/mesh.h
	structs/mxmd.h
	structs/lbim.h

	# Readers

	readers/base_reader.h

	readers/xbc1_reader.h
	readers/xbc1_reader.cpp

	readers/msrd_reader.h
	readers/msrd_reader.cpp

	readers/mesh_reader.h
	readers/mesh_reader.cpp

	readers/mxmd_reader.h
	readers/mxmd_reader.cpp

	readers/lbim_reader.h
	readers/lbim_reader.cpp
	
	# Serializers

	serializers/base_serializer.h

	serializers/model_serializer.h
	serializers/model_serializer.cpp

	deswizzle.cpp
)

add_library(xb2core STATIC ${XB2CORE_SOURCES})
set_property(TARGET xb2core PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2core PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(xb2core PRIVATE ${PROJECT_SOURCE_DIR}/vendor/zlib)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR}/vendor/zlib)

# Allow projects that import xb2core to immediately get includes of core
target_include_directories(xb2core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xb2core zlibstatic nlohmann_json::nlohmann_json fx-gltf)

# Link additional libraries required for using the 
# filesystem library with libc++ and libstdc++.
if(XB2AT_STDLIB STREQUAL "libc++")
	message(STATUS "Linking c++fs library")
	target_link_libraries(xb2core c++fs)
elseif(XB2AT_STDLIB STREQUAL "libstdc++")
	message(STATUS "Linking stdc++fs library")
	target_link_libraries(xb2core stdc++fs)
endif()

target_link_libraries(xb2core ${CMAKE_THREAD_LIBS_INIT})


add_dependencies(xb2core __xb2at_gittag)

target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR})