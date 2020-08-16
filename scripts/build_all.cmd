:: Build all configurations
@echo off


setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd" -no_logo -arch=amd64

pushd out\build\x64-release
	cmake --build . -t xb2at
:: install into install root
	cmake --build . -t install
popd

pushd out\build\x64-debug
	cmake --build . -t xb2at
:: install into install root
	cmake --build . -t install
popd

endlocal


setlocal

call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd" -no_logo -arch=x86

pushd out\build\x86-release
	cmake --build . -t xb2at
:: install into install root
	cmake --build . -t install
popd

pushd out\build\x86-debug
	cmake --build . -t xb2at
:: install into install root
	cmake --build . -t install
popd

endlocal
