@echo off
setlocal EnableDelayedExpansion

title %~nx0

set "BATDIR=%~dp0"
set "CMAKE_DIR=../.."
set "BUILD_DIR=build/wasm"

REM 设置环境变量
echo 正在配置环境变量...
set PATH=%BATDIR%tools/windows/ninja-win;%PATH%

REM 1. 进入 emsdk 目录并激活环境（带错误检查）
cd /d "tools/wasm/emsdk" || (
    echo Failed to enter emsdk directory
    pause
    exit /b 1
)
call emsdk activate latest || (
    echo Failed to activate emsdk
    pause
    exit /b 1
)
call emsdk_env.bat || (
    echo Failed to set emsdk environment
    pause
    exit /b 1
)

REM 2. 返回项目根目录并创建构建目录
cd /d "../../../app/test/" || (
    echo Failed to return to project root
    pause
    exit /b 1
)

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"
cd /d "%BUILD_DIR%"

REM 3. checking environment
echo [INFO] Current PATH: %PATH%
echo [INFO] python info: 
python --version

echo [INFO] Checking emcc: 
call emcc --version || (
    echo emcc check failed
    pause
    exit /b 1
)
echo [INFO] emsdk list:
call emsdk list

REM 4. emcmake cmake
call emcmake cmake "%CMAKE_DIR%" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -G "Ninja" || (
    echo CMake configuration failed
    pause
    exit /b 1
)

REM 5. cmake --build
call cmake --build . -- -j 4 || (
    echo Build failed
    pause
    exit /b 1
)

REM 6. 暂停查看结果
pause