:: Build all configurations
@echo off

setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd"

:: only make the relbuild directory tree if needed
if not exist "relbuild\" mkdir relbuild
if not exist "relbuild\x64-release\" mkdir relbuild\x64-release
if not exist "relbuild\x64-debug\" mkdir relbuild\x64-debug
if not exist "relbuild\x86-release\" mkdir relbuild\x86-release
if not exist "relbuild\x86-debug\" mkdir relbuild\x86-debug

pushd relbuild\x64-release
	cmake ..\.. -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017_64
	cmake --build . -t xb2at -- /p:Configuration="Release"
popd

pushd relbuild\x64-debug
	cmake ..\.. -A x64 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017_64
	cmake --build . -t xb2at -- /p:Configuration="Debug"
popd

pushd relbuild\x86-release
	cmake ..\.. -A Win32 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017
	cmake --build . -t xb2at -- /p:Configuration="Release"
popd

pushd relbuild\x86-debug
	cmake ..\.. -A Win32 -DCMAKE_BUILD_TYPE=Debug -DCMAKE_PREFIX_PATH=C:\Qt\5.14.2\msvc2017
	cmake --build . -t xb2at -- /p:Configuration="Debug"
popd

endlocal