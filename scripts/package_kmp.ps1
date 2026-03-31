$OutputDir = "release/kmp"
$NativeLibsDir = "$OutputDir/native-libs"

# Create directories
New-Item -ItemType Directory -Force -Path "$NativeLibsDir/android/arm64-v8a"
New-Item -ItemType Directory -Force -Path "$NativeLibsDir/android/x86_64"
New-Item -ItemType Directory -Force -Path "$NativeLibsDir/windows"
New-Item -ItemType Directory -Force -Path "$NativeLibsDir/macos"
New-Item -ItemType Directory -Force -Path "$NativeLibsDir/ios"
New-Item -ItemType Directory -Force -Path "$OutputDir/interop"

# 1. Build for Windows (Local)
Write-Host "Building for Windows..."
bazelisk build //ink:libinkify
Copy-Item "bazel-bin/ink/libinkify.dll" "$NativeLibsDir/windows/"
Copy-Item "ink/inkify_c_api.h" "$OutputDir/interop/"
Copy-Item "inkify.def" "$OutputDir/interop/"

# 2. Build for Android (Requires NDK)
# Note: This will fail if ANDROID_NDK_HOME is not set.
Write-Host "Building for Android (arm64)..."
try {
    bazelisk build //ink:libinkify --platforms=//:android-arm64
    Copy-Item "bazel-bin/ink/libinkify.so" "$NativeLibsDir/android/arm64-v8a/"
} catch {
    Write-Warning "Android arm64 build failed. Ensure NDK is configured."
}

Write-Host "Building for Android (x86_64)..."
try {
    bazelisk build //ink:libinkify --platforms=//:android-x86_64
    Copy-Item "bazel-bin/ink/libinkify.so" "$NativeLibsDir/android/x86_64/"
} catch {
    Write-Warning "Android x86_64 build failed. Ensure NDK is configured."
}

Write-Host "Packaging complete. Files are in $OutputDir"
Write-Host "Note: iOS and macOS builds must be performed on a Mac."
