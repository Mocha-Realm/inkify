#!/bin/bash

OUTPUT_DIR="release/kmp"
NATIVE_LIBS_DIR="$OUTPUT_DIR/native-libs"

GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo -e "${GREEN}Preparing directories...${NC}"
mkdir -p "$NATIVE_LIBS_DIR/android/arm64-v8a"
mkdir -p "$NATIVE_LIBS_DIR/android/x86_64"
mkdir -p "$NATIVE_LIBS_DIR/windows"
mkdir -p "$NATIVE_LIBS_DIR/macos"
mkdir -p "$NATIVE_LIBS_DIR/ios"
mkdir -p "$OUTPUT_DIR/interop"

echo -e "${GREEN}Building for Windows (if configured)...${NC}"
if bazel build //ink:inkify --platforms=@rules_java//toolchains:windows_x64 2>/dev/null; then
    cp -f "bazel-bin/ink/inkify.dll" "$NATIVE_LIBS_DIR/windows/"
else
    echo -e "${YELLOW}Skipping Windows build (no suitable toolchain found).${NC}"
fi

cp -f "ink/inkify_c_api.h" "$OUTPUT_DIR/interop/"
cp -f "inkify.def" "$OUTPUT_DIR/interop/"

# 2. Build for Android (arm64)
echo -e "${GREEN}Building for Android (arm64)...${NC}"
if bazel build //ink:inkify --platforms=//:android-arm64; then
    cp -f "bazel-bin/ink/libinkify.so" "$NATIVE_LIBS_DIR/android/arm64-v8a/"
    echo -e "${GREEN}Android arm64 build success!${NC}"
else
    echo -e "${RED}Android arm64 build failed. Check NDK configuration.${NC}"
fi

# 3. Build for Android (x86_64)
echo -e "${GREEN}Building for Android (x86_64)...${NC}"
if bazel build //ink:inkify --platforms=//:android-x86_64; then
    cp -f "bazel-bin/ink/libinkify.so" "$NATIVE_LIBS_DIR/android/x86_64/"
    echo -e "${GREEN}Android x86_64 build success!${NC}"
else
    echo -e "${RED}Android x86_64 build failed. Check NDK configuration.${NC}"
fi

echo -e "${GREEN}Packaging complete. Files are in $OUTPUT_DIR${NC}"
echo -e "${YELLOW}Note: iOS and macOS builds must be performed on a Mac.${NC}"