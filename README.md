
## XB2AssetTool
A rewrite of [XBC2ModelDecomp](https://github.com/BlockBuilder57/XBC2ModelDecomp) in C++, aiming for performance, 
cross platform compatibility, and code reuse.

### Features
* Dump models (including bones and flexes) to glTF
* Dump maps (including props) to glTF
* Save specified LOD values for both props and maps
* Dump all textures from files (including mesh textures/main chunk textures)
* Dumps uncompressed raw files and animations for research

### Running
Simply run the executable, and pick an input file. An output folder will be created in the path you choose your file(s) in, but you can override this by picking a output folder manually. Each file will have its own folder in the output folder. Then, configure your output settings at the bottom and hit Extract. The file should export to the output path in the format you chose.

### Compiling

First, 
- Clone this repository with the `--recursive`  argument to `git clone`.  This will fetch all the required dependencies for you.

#### Linux
The following packages need to be installed first:

- Debian/Ubuntu
	- `build-essential` 
	- `cmake`
	- `qtbase5-dev`
- Arch
	- `base-devel`
	- `cmake`
	- `qt5-base`

Build steps are as simple as:
```bash
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make xb2at
# ... profit?
```

#### Windows
You need Visual Studio 2019 with the C++ and CMake tools installed. 

MinGW-w64 *should* work (i dunno), but this configuration is currently untested and unsupported.

-  Install Qt 5 (the current releases use 5.14.2).
	- If you install Qt to C:\Qt, then that's all you need to do. 
		- If not, you'll have to edit CMakeSettings.json to change the Qt path to where you installed Qt.

Open the folder in Visual Studio (Visual Studio will autodetect a CMake project and use the CMakeSettings.json in the repository), pick the build type you want, or generate projects and build with CMake like:
```batch
mkdir build
cd build
cmake .. <-A x64> -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path_to_Qt5>
cmake --build .
```

### Creating a release package for Windows

If you want to make a self-contained release for Windows (or you want to run the project in the debugger),
execute one of the batch files in the scripts directory to make a release that can be zipped up or ran directly.


### Using the xb2core library in your own projects

XB2AssetTool's source code layout is designed to allow other developers to use the core code driving it in your own projects.
Here's how!


First and foremost if you're using Git for your source control, I'd reccomend reading up on what submodules are;
they make this process so much less of a pain for other people.

Include the XB2AssetTool source code in your project like
```cmake
set(XB2AT_XB2CORE_ONLY ON)
add_subdirectory(path/to/3rdparty/XB2AssetTool)
```


Thanks to CMake, this one line is all you need in order for everything like include path and dependencies to be set up in your project:
```cmake
target_link_libraries(MyCoolProject xb2core)
```
