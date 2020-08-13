:: Script to make release roots for windows.

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