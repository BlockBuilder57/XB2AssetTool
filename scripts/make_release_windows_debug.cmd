@echo off

setlocal

if not exist "%CD%\bind64\" mkdir bind64
set PATH=C:\Qt\5.14.2\msvc2017_64\bin\;%PATH%

pushd bind64
	copy ..\out\build\x64-debug\src\ui\xb2at.exe .
	copy ..\out\build\x64-debug\vendor\zlib\zlibd.dll .
	windeployqt .\xb2at.exe
:: delete some files that aren't needed
:: (slims down the build tree quite a bit)
	del opengl32sw.dll
	del D3Dcompiler_47.dll
	del libEGLd.dll
	del libGLESv2d.dll
popd


endlocal