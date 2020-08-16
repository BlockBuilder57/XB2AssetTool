:: Script to create packaged builds of XB2AssetTool for Windows.

@echo off

:: Make all of the release roots
:: (Release roots are self contained versions of the application that can run independently)
call scripts\release_all

:: Package binaries.
::	For debug configurations, we also additionally publish a SymsIncluded archive
::	that contains the pdb (for debugging purposes).

pushd out\install\x64-release
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x64.7z * ..\..\..\README.md
popd

pushd out\install\x64-debug
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x64_Debug.7z * ..\..\..\README.md
	copy ..\out\build\x64-debug\src\ui\xb2at.pdb .
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x64_Debug_SymsIncluded.7z * ..\..\..\README.md
	del xb2at.pdb
popd

pushd out\install\x86-release
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x86.7z * ..\..\..\README.md
popd

pushd out\install\x86-debug
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x86_Debug.7z * ..\..\..\README.md
	copy ..\..\build\x86-debug\src\ui\xb2at.pdb .
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\..\..\XB2AssetTool_x86_Debug_SymsIncluded.7z * ..\..\..\README.md
	del xb2at.pdb
popd