## Inkify 项目计划书

### 一、 项目意图 (Intent)
本项目旨在基于 Google Ink 的高性能矢量线条生成能力，构建一个支持**无限画布 (Infinite Canvas)**、**多端协同 (Real-time Collaboration)** 且**架构跨平台**的现代化笔记应用。
* **核心价值**：解决传统笔记应用在海量数据下的缩放卡顿问题，通过 CRDT 算法实现无冲突的多人实时书写体验。
* **目标平台**：利用 C++ 核心库实现 Android、iOS、Web (Wasm) 及 Desktop 的全平台覆盖。

---

### 二、 核心数据结构 (Data Structures)

为了支持无界空间和冲突合并，我们在 Ink 原有 `Stroke` 基础上进行扩展：

#### 1. 空间分块结构 (Chunking System)
使用 `i64` 坐标系定义分块，解决浮点数在高精缩放下的溢出问题。
* **Chunk 索引**：
    * `int64_t x, y`：分块在全局网格中的索引。
    * `std::vector<UUID> stroke_ids`：该块内包含的线条 ID 列表。
* **空间索引**：集成 `ink::StaticRTree`，用于视口 (Viewport) 内线条的快速检索和裁剪。

#### 2. 协同线条结构 (CRDT Stroke)
线条不再是简单的点集，而是带有身份信息的原子对象。
* **唯一标识**：`std::string uuid`。
* **逻辑时钟**：`uint64_t lamport_timestamp`（用于解决操作的时序冲突）。
* **数据载体**：
    * `ink::Brush`：笔刷样式。
    * `ink::StrokeInputBatch`：原始输入轨迹数据。
    * `bool is_deleted`：Tombstone 标记，用于协同删除逻辑。

#### 3. 渲染网格缓存 (Render Cache)
* **PartitionedMesh**：存储由 `ink::Tessellator` 生成的分区网格。
* **LOD Level**：根据缩放比例存储不同精度的网格快照。

---

### 三、 跨平台方案 (Cross-Platform Strategy)

#### 1. 核心层 (Core Library - C++)
* **逻辑解耦**：将 `ink/strokes`（线条逻辑）、`ink/geometry`（几何计算） 和 `ink/color` 等核心模块编译为跨平台静态库（.a / .lib）。
* **移除平台依赖**：剥离原有的 `internal/jni` 绑定，改为通用的 C++ 接口。

#### 2. 渲染抽象层 (Rendering Abstraction)
* **Skia 统一渲染**：利用 `SkiaRenderer` 作为基准渲染器。
    * **Android/iOS/Desktop**：直接使用 Skia 原生后端。
    * **Web**：通过 CanvasKit (Skia Wasm) 实现一致的渲染效果。
* **网格提取**：在不支持 Skia 的环境，直接通过 `ink::Mesh` 的 `RawVertexData()` 提取顶点数据，对接平台原生的图形 API（如 Metal 或 WebGPU）。

#### 3. 存储与通信 (Storage & Sync)
* **协议定义**：扩展 `ink/storage/proto` 中的 Protobuf 定义，增加协同元数据字段。
* **数据交换**：使用 `ink::storage` 提供的序列化工具进行二进制压缩存储，降低网络同步延迟。

---

### 四、 关键技术路线图 (Milestones)

1.  **Phase 1: 架构剥离**
    * 重构 Bazel 构建配置，支持 NDK 以外的 toolchains。
    * 封装纯 C++ 的 `Stroke` 管理器。
2.  **Phase 2: 无界空间实现**
    * 开发 `ChunkManager`，实现基于 `StaticRTree` 的动态加载/卸载逻辑。
    * 实现视口到分块局部坐标的 `AffineTransform` 转换。
3.  **Phase 3: 协同协议集成**
    * 基于 Protobuf 实现增量同步指令。
    * 集成 LWW (Last Write Wins) 冲突合并策略。
4.  **Phase 4: 多端渲染适配**
    * 完成 WebAssembly 编译与 CanvasKit 对接。
    * 优化低功耗设备上的 `Mesh` 简化 (LOD) 逻辑。