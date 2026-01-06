@echo off
prompt $g
chcp 65001 >nul
setlocal enabledelayedexpansion

title %~nx0

REM ###############################################
REM 版本（可以根据需要调整对应的已发布版本号）
set ANDROID_PLATFORM=android-28
set NDK_VER=27.0.12077973

REM 配置android路径（修改为您本地的实际路径）
set SDK_ROOT=C:/Users/Administrator/AppData/Local/Android/Sdk
set NDK_ROOT=%SDK_ROOT%/ndk/%NDK_VER%
set JAVA_HOME=%~dp0tools/android/OpenJDK17

REM 指定sdkmanager版本，需要先安装
set SDKMANAGER=%SDK_ROOT%/cmdline-tools/latest/bin/sdkmanager.bat

REM ========== 检查并安装 platforms/NDK（如未安装自动装） ==========
echo 检查并安装
call "%SDKMANAGER%" "ndk;%NDK_VER%" "platforms;%ANDROID_PLATFORM%"

REM 设置Android构建参数
set "ABI_LIST=arm64-v8a armeabi-v7a x86 x86_64"  :: 需要构建的ABI列表

REM 编译项目设置
set "BATDIR=%~dp0"
set "CMAKE_DIR=../../../engine"
set "BUILD_ROOT=build/android"
set "BUILD_RETURN=../../.."
REM ###############################################

REM 检查基本路径有效性
echo 正在验证路径配置...
if not exist "%SDK_ROOT%" (
    echo [错误] SDK路径不存在：%SDK_ROOT%
    pause
    exit /b 1
)
if not exist "%NDK_ROOT%" (
    echo [错误] NDK路径不存在：%NDK_ROOT%
    pause
    exit /b 1
)
if not exist "%JAVA_HOME%" (
    echo [错误] JDK路径不存在：%JAVA_HOME%
    pause
    exit /b 1
)

REM 设置环境变量
echo 正在配置环境变量...
set PATH=%JAVA_HOME%/bin;%NDK_ROOT%;%SDK_ROOT%/tools;%BATDIR%/tools/windows/ninja-win;%PATH%

REM 检查CMake可用性
where cmake >nul 2>&1
if %ERRORLEVEL% neq 0 (
    echo [错误] 找不到CMake，请安装并添加到PATH环境变量
    pause
    exit /b 1
)

REM 多ABI构建循环
for %%a in (%ABI_LIST%) do (
    echo.
    echo ========================================
    echo 正在构建架构：%%a
    echo ========================================
    
    REM 创建架构专用构建目录
    set "BUILD_DIR=%BUILD_ROOT%/%%a"
    if not exist "!BUILD_DIR!" (
        echo 创建构建目录：!BUILD_DIR!
        mkdir "!BUILD_DIR!"
    )

    REM 生成构建系统
    echo 正在生成构建系统...
    cd "!BUILD_DIR!"
    cmake %CMAKE_DIR% ^
        -G "Ninja" ^
        -DCMAKE_TOOLCHAIN_FILE="%NDK_ROOT%/build/cmake/android.toolchain.cmake" ^
        -DANDROID_ABI=%%a ^
        -DANDROID_PLATFORM=%ANDROID_PLATFORM% ^
        -DCMAKE_BUILD_TYPE=Release

    if !ERRORLEVEL! neq 0 (
        echo [错误] CMake生成失败：%%a
        cd %BUILD_RETURN%
        pause
        exit /b 1
    )

    REM 开始编译
    echo 正在编译项目...
    cmake --build . --config Release --parallel 8

    if !ERRORLEVEL! neq 0 (
        echo [错误] 编译失败：%%a
        cd %BUILD_RETURN%
        pause
        exit /b 1
    )

    echo 成功构建架构：%%a
    cd %BUILD_RETURN%
)

echo.
echo ========================================
echo 所有ABI构建完成！
echo 生成位置：%BUILD_ROOT%
echo ========================================
pause
endlocal