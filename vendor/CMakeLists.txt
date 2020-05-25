add_subdirectory(zlib)
add_subdirectory(json)

option(FX_GLTF_INSTALL OFF)
option(FX_GLTF_USE_INSTALLED_DEPS OFF)
add_subdirectory(fx-gltf)