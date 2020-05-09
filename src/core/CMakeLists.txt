# xb2core library

# we have header files here so that when they're changed
# they trigger a rebuild of xb2core source files
set(XB2CORE_SOURCES

	# core library stuff
	core.h
	structs.h
	endian_swap.h
	streamhelper.h

	# readers

	# base reader interface
	base_reader.h

	xbc1_reader.h
	xbc1_reader.cpp

	msrd_reader.h
	msrd_reader.cpp
)

add_library(xb2core STATIC ${XB2CORE_SOURCES})
set_property(TARGET xb2core PROPERTY CXX_STANDARD 17)
set_property(TARGET xb2core PROPERTY CXX_STANDARD_REQUIRED ON)

target_include_directories(xb2core PRIVATE ../../vendor/zlib)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR}/vendor/zlib)

# make so projects that reference xb2core get everything to include
target_include_directories(xb2core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(xb2core zlib)