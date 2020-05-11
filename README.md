
## XB2AssetTool
A *very* work-in-progress rewrite of [XBC2ModelDecomp](https://github.com/BlockBuilder57/XBC2ModelDecomp) in C++.

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
The following packages need to be installed:

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
You need Visual Studio 2019 with the C++ and CMake tools installed. MinGW-w64 *should* work, but this configuration is untested and unsupported.

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

If you want to make a self-contained release (or you want to run the project in the debugger), execute one of the batch files in the scripts directory to make a release that can be zipped up or ran directly (Note: these scripts assume you build in directories like `out/build/x64-Debug` like how Visual Studio is set up).
