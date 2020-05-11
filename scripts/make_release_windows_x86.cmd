@echo off

setlocal

if not exist "%CD%\bin32\" mkdir bin32
set PATH=C:\Qt\5.14.2\msvc2017\bin\;%PATH%

pushd bin32
	copy ..\out\build\x86-release\src\ui\xb2at.exe .
	copy ..\out\build\x86-release\vendor\zlib\zlib.dll .
	windeployqt --release .\xb2at.exe
:: delete some files that aren't needed
:: (slims down the build tree quite a bit)
	del opengl32sw.dll
	del D3Dcompiler_47.dll
	del libEGL.dll
	del libGLESv2.dll
popd


endlocal