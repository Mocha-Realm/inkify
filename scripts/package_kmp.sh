#!/bin/bash

# 设置输出目录
OUTPUT_DIR="release/kmp"
NATIVE_LIBS_DIR="$OUTPUT_DIR/native-libs"

# 定义颜色（让输出好看点）
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Preparing directories...${NC}"
# 创建所有必要的目录 (-p 会自动处理父目录且如果目录已存在不会报错)
mkdir -p "$NATIVE_LIBS_DIR/android/arm64-v8a"
mkdir -p "$NATIVE_LIBS_DIR/android/x86_64"
mkdir -p "$NATIVE_LIBS_DIR/windows"
mkdir -p "$NATIVE_LIBS_DIR/macos"
mkdir -p "$NATIVE_LIBS_DIR/ios"
mkdir -p "$OUTPUT_DIR/interop"

# 1. Build for Windows (如果是跨平台构建)
# 注意：在 Linux 下编译 Windows 库通常需要 mingw 或跨平台 LLVM
# 如果你在 WSL 下只想编译 Android 库，可以跳过这一步
echo -e "${GREEN}Building for Windows (if configured)...${NC}"
if bazel build //ink:libinkify --platforms=@rules_java//toolchains:windows_x64 2>/dev/null; then
    cp -f "bazel-bin/ink/libinkify.dll" "$NATIVE_LIBS_DIR/windows/"
else
    echo -e "${YELLOW}Skipping Windows build (no suitable toolchain found).${NC}"
fi

# 拷贝通用头文件和定义文件
cp -f "ink/inkify_c_api.h" "$OUTPUT_DIR/interop/"
cp -f "inkify.def" "$OUTPUT_DIR/interop/"

# 2. Build for Android (arm64)
echo -e "${GREEN}Building for Android (arm64)...${NC}"
if bazel build //ink:libinkify --platforms=//:android-arm64; then
    cp -f "bazel-bin/ink/libinkify.so" "$NATIVE_LIBS_DIR/android/arm64-v8a/"
    echo -e "${GREEN}Android arm64 build success!${NC}"
else
    echo -e "${RED}Android arm64 build failed. Check NDK configuration.${NC}"
fi

# 3. Build for Android (x86_64)
echo -e "${GREEN}Building for Android (x86_64)...${NC}"
if bazel build //ink:libinkify --platforms=//:android-x86_64; then
    cp -f "bazel-bin/ink/libinkify.so" "$NATIVE_LIBS_DIR/android/x86_64/"
    echo -e "${GREEN}Android x86_64 build success!${NC}"
else
    echo -e "${RED}Android x86_64 build failed. Check NDK configuration.${NC}"
fi

echo -e "${GREEN}Packaging complete. Files are in $OUTPUT_DIR${NC}"
echo -e "${YELLOW}Note: iOS and macOS builds must be performed on a Mac.${NC}"