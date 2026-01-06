#!/bin/bash

# 配置
CMAKE_DIR="../.."
BUILD_DIR="app/test/build/mac"
BUILD_TYPE="Debug"

# 创建构建目录
mkdir -p $BUILD_DIR || exit 1
cd $BUILD_DIR || exit 1

# 生成构建系统
cmake $CMAKE_DIR -DCMAKE_BUILD_TYPE=$BUILD_TYPE || {
    echo "Build failed cmake!"
    exit 1
}

# 编译项目（自动检测CPU核心数）
make -j$(sysctl -n hw.logicalcpu) || {
    echo "Build failed make!"
    exit 1
}

echo "Build completed：$(pwd)"