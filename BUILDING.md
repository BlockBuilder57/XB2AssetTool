## Building XB2AssetTool

First, 
- Clone this repository with the `--recursive`  argument to `git clone`.  This will fetch all the required dependencies for you.

#### Linux
The following packages need to be installed first before building:

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
# ... profit!
```

#### Windows
You need Visual Studio 2019 with the C++ and CMake tools installed. 

We reccomend also installing the Clang compiler from within the VS installer, 
and it's going to be used for any further binary releases going on (the CMakeSettings.json will also prefer Clang by default).

MSVC *can* be used with modifiation to the CMakeSettings, however there's a high chance that the binaries may be slightly slower than Clang.

First:

-  Install Qt 5 (the current releases use 5.15.0). For Clang, you can just use the MSVC 2019 libraries if you don't feel like compiling from source.

If you chose to install Qt to C:\Qt and used 5.15.0, then that's all the setup you need!
If not, you'll have to edit CMakeSettings.json to change the Qt path to where you installed Qt.

Open the folder in Visual Studio (Visual Studio will autodetect a CMake project and use the CMakeSettings.json in the repository), pick the build type you want, or generate projects and build with CMake like:
```batch
mkdir build
cd build
cmake .. <-A x64> -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=<path_to_Qt5>
cmake --build .
```

### Creating a release package for Windows

If you want to make a self-contained release for Windows (or you want to run in the debugger),

simply switch to the configuration you want in VS, and do `Build->Install xb2at`. The release package will be made in the out/install/[Config] directory
and the Qt DLLs/runtime data will magically be put there too! 

scripts/package_windows can be used to make 7z packages ready to give to users.

