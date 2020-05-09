@echo off

setlocal

if not exist "%CD%\bin64\" mkdir bin64
set PATH=C:\Qt\5.14.2\msvc2017_64\bin\;%PATH%

pushd bin64
	copy ..\out\build\x64-release\src\ui\xb2at.exe .
	copy ..\out\build\x64-release\vendor\zlib\zlib.dll .
	windeployqt --release .\xb2at.exe
popd


endlocal