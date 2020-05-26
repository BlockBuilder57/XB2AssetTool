:: Script to package xb2at for windows.

@echo off

:: Build all configurations
call scripts\build_all
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: Make all of the release roots
:: (Release roots are self contained)
call scripts\make_release_windows
call scripts\make_release_windows_debug
call scripts\make_release_windows_x86
call scripts\make_release_windows_debug_x86

:: Package binaries.
::	For debug configurations, we also additionally publish a SymsIncluded archive
::	that contains the pdb.

pushd bin64
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x64.7z * ..\README.md
popd

pushd bind64
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x64_Debug.7z * ..\README.md
	copy ..\out\build\x64-debug\src\ui\debug\xb2at.pdb .
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x64_Debug_SymsIncluded.7z * ..\README.md
	del xb2at.pdb
popd

pushd bin32
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x86.7z * ..\README.md
popd

pushd bind32
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x86_Debug.7z * ..\README.md
	copy ..\out\build\x86-debug\src\ui\debug\xb2at.pdb .
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x86_Debug_SymsIncluded.7z * ..\README.md
	del xb2at.pdb
popd