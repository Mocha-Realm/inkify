# Inkify KMP 接入指南

本项目底层由 C++ 实现，通过 C-Bridge 桥接层支持全平台集成。

## 1. 编译各平台二进制文件
在项目根目录运行以下脚本（需配置好相应平台的编译链）：

- **Windows/Android**: 运行 `./scripts/package_kmp.ps1` (Windows) 或 Bash 等价脚本。
- **iOS/macOS**: 需在 Mac 环境下运行 `bazelisk build //ink:libinkify --apple_platform_type=ios`。

编译后的产物将归类在 `release/kmp/native-libs` 目录下。

## 2. Kotlin Multiplatform (KMP) 项目配置

### A. 添加 Interop 定义
将 `release/kmp/interop/inkify_c_api.h` 和 `inkify.def` 放入 KMP 项目的 `src/nativeInterop/cinterop` 目录。

### B. 修改 build.gradle.kts
```kotlin
kotlin {
    // 1. 配置各平台 Target
    iosArm64()
    macosArm64()
    mingwX64() // Windows
    androidTarget()

    // 2. 配置 Native CInterop
    val nativeTargets = listOf(iosArm64(), macosArm64(), mingwX64())
    nativeTargets.forEach { target ->
        target.compilations.getByName("main") {
            val inkify by cinterops.creating("inkify") {
                definitionFile.set(project.file("src/nativeInterop/cinterop/inkify.def"))
            }
        }
    }

    // 3. 链接二进制库
    sourceSets {
        val nativeMain by creating {
            // ...
        }
    }
}
```

### C. 库文件存放位置
- **Android**: 将 `.so` 放入 `src/main/jniLibs/{abi}/`。
- **iOS**: 将编译出的静态库 `.a` 链接到 Framework。
- **Windows**: 确保 `libinkify.dll` 在运行路径或系统的 PATH 中。

## 3. API 调用示例 (Common)

在 `commonMain` 中定义统一接口：
```kotlin
expect class InkifyEngine(chunkSize: Float) {
    fun addStroke(uuid: String, timestamp: Long)
    fun queryStrokes(xMin: Float, yMin: Float, xMax: Float, yMax: Float): List<String>
    fun dispose()
}
```

在 `nativeMain` 中实现 (Native 目标自动共享逻辑)：
```kotlin
import com.google.inkify.interop.*
import kotlinx.cinterop.*

actual class InkifyEngine actual constructor(chunkSize: Float) {
    private val sm = inkify_stroke_manager_create()
    private val cm = inkify_chunk_manager_create(chunkSize)

    actual fun addStroke(uuid: String, timestamp: Long) {
        inkify_stroke_manager_add_empty_stroke(sm, uuid, timestamp.toULong())
    }

    actual fun queryStrokes(xMin: Float, yMin: Float, xMax: Float, yMax: Float): List<String> {
        val res = inkify_chunk_manager_query_viewport(cm, sm, xMin, yMin, xMax, yMax)
        val list = mutableListOf<String>()
        res.useContents {
            for (i in 0 until count) {
                list.add(uuids!![i.toInt()]?.toKString() ?: "")
            }
        }
        inkify_query_result_free(res)
        return list
    }

    actual fun dispose() {
        inkify_chunk_manager_destroy(cm)
        inkify_stroke_manager_destroy(sm)
    }
}
```
