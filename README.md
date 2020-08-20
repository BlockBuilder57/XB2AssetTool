
## XB2AssetTool

A rewrite of [XBC2ModelDecomp](https://github.com/BlockBuilder57/XBC2ModelDecomp) in C++ using the Qt UI library, aiming for performance, 
cross platform compatibility, and code reuse.

### Features

* Dump models (including bones and flexes) to glTF
* Dump maps (including props) to glTF
* Save specified LOD values for both props and maps
* Dump all textures from files (including mesh textures/main chunk textures)
* Dumps uncompressed raw files and animations for research

### Thanks to

PredatorCZ/Lukas Cone - [3DSMax Xenoblade Import Tools](https://lukascone.wordpress.com/2018/05/06/xenoblade-chronicles-import-tool/)

Thealexbarney - [XbTool](https://github.com/Thealexbarney/XbTool) (Texture swizzling)

Turk645 - Map format & ideas

### Running

Simply run the executable, and pick an input file. An output folder will be created in the path you choose your file(s) in, but you can override this by picking a output folder manually. Each file will have its own folder in the output folder. Then, configure your output settings at the bottom and hit Extract. The file should export to the output path in the format you chose.

### Compiling

Refer to [BUILDING.md](https://github.com/BlockBuilder57/XB2AssetTool/blob/master/BUILDING.md).

### Using the xb2core library in your own projects

XB2AssetTool's source code layout is designed to allow other developers to use the core code driving it in your own projects.
Here's how!

First and foremost, if you're using Git for your source control, I'd reccomend reading up on what submodules are;
they make this process so much less of a pain for other people.

Include the XB2AssetTool source code in your project like so, so that you only get xb2core.
```cmake
set(XB2AT_XB2CORE_ONLY ON)
# Optional if you want to use the Easymode API:
#set(XB2AT_XB2CORE_EASYMODE ON)
add_subdirectory(path/to/3rdparty/XB2AssetTool)
```


Thanks to CMake, this one line is all you need in order for everything like include path and dependencies to be set up in your project:
```cmake
target_link_libraries(MyCoolProject xb2core)
```
