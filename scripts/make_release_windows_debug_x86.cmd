@echo off

setlocal

if not exist "%CD%\bind32\" mkdir bind32
set PATH=C:\Qt\5.14.2\msvc2017\bin\;%PATH%

pushd bind64
	copy ..\out\build\x86-debug\src\ui\xb2at.exe .
	copy ..\out\build\x86-debug\vendor\zlib\zlibd.dll .
	windeployqt .\xb2at.exe
popd


endlocal