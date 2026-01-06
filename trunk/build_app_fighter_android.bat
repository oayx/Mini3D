@echo off
prompt $g
chcp 65001 >nul
setlocal enabledelayedexpansion

title %~nx0

REM ========== 配置 JAVA_HOME ==========
set "JAVA_HOME=%~dp0tools/android/OpenJDK17"
set PATH=%JAVA_HOME%/bin;%PATH%

REM ========== 配置 Android SDK 路径 ==========
set SDK_ROOT=C:/Users/Administrator/AppData/Local/Android/Sdk
REM 指定sdkmanager版本，需要先安装
set SDKMANAGER=%SDK_ROOT%/cmdline-tools/latest/bin/sdkmanager.bat
REM NDK/CMake 版本（可以根据需要调整对应的已发布版本号）
set NDK_VER=27.0.12077973
set CMAKE_VER=3.22.1

REM ========== 检查并安装 NDK 和 CMake（如未安装自动装） ==========
echo 检查并安装 NDK 和 CMake ...
call "%SDKMANAGER%" "ndk;%NDK_VER%" "cmake;%CMAKE_VER%"

REM ========== 设置 local.properties ==========
set "PROJECT_DIR=%~dp0app/fighter/project.android"
set "NDK_VER=27.0.12077973"
set "CMAKE_VER=3.22.1"
(
    echo sdk.dir=%SDK_ROOT%
    echo ndk.dir=%SDK_ROOT%/ndk/%NDK_VER%
    echo cmake.dir=%SDK_ROOT%/cmake/%CMAKE_VER%
) > "%PROJECT_DIR%/local.properties"

REM ===== gradlew 或 gradlew.bat 工具路径 =====
set GRADLEW=%PROJECT_DIR%/gradlew.bat

REM 切换到工程目录
cd /d "%PROJECT_DIR%"

REM 清理旧的构建（可选）
echo 正在清理...
call "%GRADLEW%" clean

REM 开始构建 Debug APK
echo 开始构建 Debug APK...
call "%GRADLEW%" assembleDebug

echo APK已完成

pause