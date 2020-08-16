:: Script to make release roots for windows.

@echo off

:: Build all configurations
call scripts\build_all
if %ERRORLEVEL% neq 0 exit /b %ERRORLEVEL%

:: no additional logic, build_all & cmake installation do the rest