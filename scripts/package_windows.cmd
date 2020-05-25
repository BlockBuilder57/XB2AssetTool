@echo off

scripts\make_release_windows
scripts\make_release_windows_debug
scripts\make_release_windows_x86
scripts\make_release_windows_debug_x86

pushd bin64
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x64.7z *
popd

pushd bind64
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x64_Debug.7z *
popd

pushd bin32
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x86.7z *
popd

pushd bind32
	"C:\Program Files\7-Zip\7z" a -mx9 -myx7 -mf=on -mtc=on -mta=on -mmt8 -bb3 ..\XB2AssetTool_x86_Debug.7z *
popd