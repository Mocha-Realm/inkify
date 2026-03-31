// Copyright 2024-2025 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef INK_INKIFY_C_API_H_
#define INK_INKIFY_C_API_H_

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Opaque handles for C++ objects
typedef void* InkifyStrokeManagerPtr;
typedef void* InkifyChunkManagerPtr;

// --- StrokeManager Lifecycle ---
InkifyStrokeManagerPtr inkify_stroke_manager_create();
void inkify_stroke_manager_destroy(InkifyStrokeManagerPtr manager);

// --- Stroke Operations ---
// Adds or updates a stroke. For simplicity in C, we just pass the metadata.
// In a real scenario, we'd also pass the serialized stroke data (proto).
void inkify_stroke_manager_add_empty_stroke(InkifyStrokeManagerPtr manager, const char* uuid, uint64_t timestamp);
bool inkify_stroke_manager_is_deleted(InkifyStrokeManagerPtr manager, const char* uuid);
void inkify_stroke_manager_delete_stroke(InkifyStrokeManagerPtr manager, const char* uuid, uint64_t timestamp);
uint32_t inkify_stroke_manager_get_active_count(InkifyStrokeManagerPtr manager);

// --- ChunkManager Lifecycle ---
InkifyChunkManagerPtr inkify_chunk_manager_create(float chunk_size);
void inkify_chunk_manager_destroy(InkifyChunkManagerPtr manager);

// --- Chunk Operations ---
void inkify_chunk_manager_register_stroke(InkifyChunkManagerPtr chunk_manager, InkifyStrokeManagerPtr stroke_manager, const char* uuid);

// Query results structure
typedef struct {
    const char** uuids;
    uint32_t count;
} InkifyQueryResult;

InkifyQueryResult inkify_chunk_manager_query_viewport(InkifyChunkManagerPtr chunk_manager, InkifyStrokeManagerPtr stroke_manager, 
                                                    float x_min, float y_min, float x_max, float y_max);
void inkify_query_result_free(InkifyQueryResult result);

#ifdef __cplusplus
}
#endif

#endif  // INK_INKIFY_C_API_H_
