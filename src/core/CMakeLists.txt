# xb2core CMakeLists.txt


set(XB2CORE_SOURCES
	IoStreamReadStream.cpp

# File Readers

	readers/xbc1_reader.cpp
	readers/msrd_reader.cpp
	readers/mesh_reader.cpp
	readers/mxmd_reader.cpp
	readers/mibl_reader.cpp
	readers/sar1_reader.cpp
	readers/skel_reader.cpp
	
# Serializers
	serializers/model_serializer.cpp

# Texture stuff
	serializers/MIBLDeswizzler.cpp
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

target_include_directories(xb2core PRIVATE ${PROJECT_SOURCE_DIR}/vendor/zlib)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR}/vendor/zlib)

# Allow projects that import xb2core to immediately get includes of core
target_include_directories(xb2core PUBLIC ${PROJECT_SOURCE_DIR}/include)
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

target_link_libraries(xb2core ${CMAKE_THREAD_LIBS_INIT} mcommon)

# We use the version tag so depend on it and add a private include path to where it's generated
add_dependencies(xb2core __xb2at_gittag)
target_include_directories(xb2core PRIVATE ${PROJECT_BINARY_DIR})

# Allow installation of xb2core in a standalone manner.
# todo fix include copying
if(NOT XB2CORE_DO_NOT_INSTALL)
	install(TARGETS xb2core
		ARCHIVE DESTINATION "${INSTALL_LIB_DIR}"
		LIBRARY DESTINATION "${INSTALL_LIB_DIR}")

	install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/xb2at/"
		DESTINATION "${CMAKE_INSTALL_PREFIX}/include/xb2at")

	# Instga
	install()
endif()