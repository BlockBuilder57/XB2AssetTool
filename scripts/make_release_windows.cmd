@echo off

setlocal

if not exist "%CD%\bin64\" mkdir bin64
set PATH=C:\Qt\5.14.2\msvc2017_64\bin\;%PATH%

pushd bin64
	copy ..\out\build\x64-release\src\ui\xb2at.exe .
	windeployqt --release .\xb2at.exe
:: delete some files that aren't needed
:: (slims down the build tree quite a bit)
	del opengl32sw.dll
	del D3Dcompiler_47.dll
	del libEGL.dll
	del libGLESv2.dll
	
	:: remove unnesscary translations shaving some space
	for /f %%f in ('dir /b translations\*.qm') do if not %%f==qt_en.qm del /s /q %%f
	
popd


endlocal