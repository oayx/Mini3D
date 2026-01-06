#!/bin/bash

# 配置
CMAKE_DIR="../.."
BUILD_DIR="app/fighter/build/ios"
BUILD_TYPE="Debug"

TOOLCHAIN="../../../../tools/iOS/ios-cmake/ios.toolchain.cmake"  # iOS 工具链路径
DEPLOYMENT_TARGET="14.0"                      # 最低支持 iOS 版本
PLATFORM="OS64"                               # OS64=真机(ARM64), SIMULATOR=模拟器(x86_64/ARM64)，OS64COMBINED: 真机+模拟器通用库

# 创建构建目录
mkdir -p $BUILD_DIR || exit 1
cd $BUILD_DIR || exit 1

# 检查工具链文件是否存在
if [ ! -f "$TOOLCHAIN" ]; then
    echo "错误：iOS 工具链文件未找到，请确认路径或下载："
    echo "git clone https://github.com/leetal/ios-cmake.git"
    exit 1
fi

# 生成构建系统
cmake $CMAKE_DIR \
	-DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN" \
	-DPLATFORM="$PLATFORM" \
	-DDEPLOYMENT_TARGET="$DEPLOYMENT_TARGET" \
	-DCMAKE_BUILD_TYPE="$BUILD_TYPE" /v:d || {
    echo "Build failed cmake!"
    exit 1
}

# 编译项目（自动检测CPU核心数）
make -j$(sysctl -n hw.logicalcpu) || {
    echo "Build failed make!"
    exit 1
}

echo "Build completed：$(pwd)"