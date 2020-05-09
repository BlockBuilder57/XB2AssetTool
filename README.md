## XB2AssetTool
A *very* work-in-progress rewrite of [XBC2ModelDecomp](https://github.com/BlockBuilder57/XBC2ModelDecomp) in C++.

### Features
* Dump models (including bones and flexes) to glTF
* Dump maps (including props) to glTF
* Save specified LOD values for both props and maps
* Dump all textures from files (including mesh textures/main chunk textures)
* Dumps raw files and animations for research

### Running
Simply run the executable, and pick an input file. An output folder will be created in the path you choose your file(s) in, but you can override this by picking a output folder manually. Each file will have its own folder in the output folder. Then, configure your output settings at the bottom and hit Extract. The file should export to the output path in the format you chose.

### Compiling
You need at least Visual Studio 2019 with the C++ and CMake tools installed.

First,
- Clone this repository with the `--recursive`  argument to `git clone`.  This will fetch all the dependencies for you.

-  Install Qt 5 (the current releases use 5.14.2).
	- If you install Qt to C:\Qt, then that's all you need to do. 
		- If not, you'll have to edit CMakeSettings.json to change the Qt path to where you installed Qt.

After that, simply open the folder in Visual Studio (or generate projects & build with CMake for Linux or people who prefer plain CMake), pick the build type you want, and build away!
