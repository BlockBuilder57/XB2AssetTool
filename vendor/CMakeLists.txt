# stop zlib from trying to install anything (we use zlib static)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_FILES ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_HEADERS ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_LIBRARIES ON CACHE BOOL "" FORCE)

add_subdirectory(zlib)

set(JSON_BuildTests OFF CACHE BOOL "" FORCE)
set(JSON_Install OFF CACHE BOOL "" FORCE)
set(JSON_MultipleHeaders ON CACHE BOOL "" FORCE)

# This will also make anything using ctest not build tests
set(BUILD_TESTING OFF CACHE BOOL "no tests" FORCE)
add_subdirectory(json)

option(FX_GLTF_INSTALL OFF)
option(FX_GLTF_USE_INSTALLED_DEPS OFF)
add_subdirectory(fx-gltf)

add_subdirectory(glm)

set(MCOMMON_DO_NOT_INSTALL ON)
add_subdirectory(mcommon)