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