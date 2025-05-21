@echo off
echo Starting build process...
call build.bat
if %ERRORLEVEL% EQU 0 (
    echo Build completed successfully
    echo Starting build\astro-sim.exe...
    start build\astro-sim.exe
) else (
    echo Build failed with error code %ERRORLEVEL%
    pause
    exit /b %ERRORLEVEL%
) 