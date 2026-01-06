@echo off
chcp 65001 >nul
setlocal enabledelayedexpansion

title %~nx0

set "BUILD_TYPE=Debug"
set "CMAKE_DIR=../../../engine"
set "BUILD_DIR=build/win/win64_mingw"
set "MINGW_PATH=%~dp0tools/windows/mingw64"
set "MINGW_BIN=%MINGW_PATH%/bin"
set "PATH=%MINGW_BIN%;%PATH%"

where mingw32-make.exe >nul 2>nul
if errorlevel 1 (
    echo [ERROR] mingw32-make.exe not found! Please check MINGW_PATH setting.
    pause
    exit /b 1
)

if not exist "%BUILD_DIR%" (
    mkdir "%BUILD_DIR%"
)
cd /d "%BUILD_DIR%"

cmake "%CMAKE_DIR%" ^
    -G "MinGW Makefiles" ^
    -DCMAKE_BUILD_TYPE=%BUILD_TYPE% ^
    -DCMAKE_C_COMPILER=%MINGW_BIN%/gcc.exe ^
    -DCMAKE_CXX_COMPILER=%MINGW_BIN%/g++.exe

if errorlevel 1 (
    echo [ERROR] CMake configure failed.
    cd ..
    pause
    exit /b 1
)

mingw32-make -j

if errorlevel 1 (
    echo [ERROR] Build failed.
    cd ..
    pause
    exit /b 1
)

echo Build completed successfully
pause