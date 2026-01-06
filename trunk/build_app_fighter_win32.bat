@echo off
setlocal enabledelayedexpansion

title %~nx0

set "BUILD_TYPE=Debug"			REM Debug/Release
set "BUILD_PLATFORM=Win32"		REM x64/Win32
set "CMAKE_DIR=../.."
set "BUILD_DIR=app/fighter/build/win32_vs2022"
set "SDK_VERSION=10.0.19041.0"
set "VS_GENERATOR=Visual Studio 17 2022"
set "VS_TOOLSET=version=17.9"

set "VS2022_PATH=C:/Program Files/Microsoft Visual Studio/2022/Community"
call "%VS2022_PATH%/Common7/Tools/VsDevCmd.bat" -arch=amd64

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

cmake "%CMAKE_DIR%" ^
    -G "%VS_GENERATOR%" ^
    -A %BUILD_PLATFORM%

if errorlevel 1 (
    echo [ERROR] CMake
    cd ..
    pause
    exit /b 1
)

cmake --build . --config %BUILD_TYPE% -- /m

if errorlevel 1 (
    echo [ERROR] Build
    cd ..
    pause
    exit /b 1
)

echo Build completed (%BUILD_PLATFORM% %BUILD_TYPE%).
pause