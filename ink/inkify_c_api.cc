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

#include "ink/inkify_c_api.h"

#include <cstring>
#include <vector>

#include "ink/strokes/stroke_manager.h"
#include "ink/strokes/chunk_manager.h"
#include "ink/geometry/rect.h"

using namespace ink;

extern "C" {

InkifyStrokeManagerPtr inkify_stroke_manager_create() {
    return new StrokeManager();
}

void inkify_stroke_manager_destroy(InkifyStrokeManagerPtr manager) {
    delete static_cast<StrokeManager*>(manager);
}

void inkify_stroke_manager_add_empty_stroke(InkifyStrokeManagerPtr manager, const char* uuid, uint64_t timestamp) {
    CrdtStroke stroke;
    stroke.uuid = uuid;
    stroke.lamport_timestamp = timestamp;
    static_cast<StrokeManager*>(manager)->AddOrUpdateStroke(stroke);
}

bool inkify_stroke_manager_is_deleted(InkifyStrokeManagerPtr manager, const char* uuid) {
    const auto* s = static_cast<StrokeManager*>(manager)->GetStroke(uuid);
    return s ? s->is_deleted : false;
}

void inkify_stroke_manager_delete_stroke(InkifyStrokeManagerPtr manager, const char* uuid, uint64_t timestamp) {
    static_cast<StrokeManager*>(manager)->DeleteStroke(uuid, timestamp);
}

uint32_t inkify_stroke_manager_get_active_count(InkifyStrokeManagerPtr manager) {
    return static_cast<uint32_t>(static_cast<StrokeManager*>(manager)->GetActiveStrokes().size());
}

InkifyChunkManagerPtr inkify_chunk_manager_create(float chunk_size) {
    return new ChunkManager(chunk_size);
}

void inkify_chunk_manager_destroy(InkifyChunkManagerPtr manager) {
    delete static_cast<ChunkManager*>(manager);
}

void inkify_chunk_manager_register_stroke(InkifyChunkManagerPtr chunk_manager, InkifyStrokeManagerPtr stroke_manager, const char* uuid) {
    static_cast<ChunkManager*>(chunk_manager)->RegisterStroke(uuid, *static_cast<StrokeManager*>(stroke_manager));
}

InkifyQueryResult inkify_chunk_manager_query_viewport(InkifyChunkManagerPtr chunk_manager, InkifyStrokeManagerPtr stroke_manager, 
                                                    float x_min, float y_min, float x_max, float y_max) {
    Rect viewport = Rect::FromTwoPoints({x_min, y_min}, {x_max, y_max});
    auto strokes = static_cast<ChunkManager*>(chunk_manager)->QueryViewport(viewport, *static_cast<StrokeManager*>(stroke_manager));
    
    InkifyQueryResult result;
    result.count = static_cast<uint32_t>(strokes.size());
    if (result.count == 0) {
        result.uuids = nullptr;
        return result;
    }

    const char** uuid_array = new const char*[result.count];
    for (uint32_t i = 0; i < result.count; ++i) {
        // Use a cross-platform way to duplicate strings.
#ifdef _WIN32
        uuid_array[i] = _strdup(strokes[i]->uuid.c_str());
#else
        uuid_array[i] = strdup(strokes[i]->uuid.c_str());
#endif
    }
    result.uuids = uuid_array;
    return result;
}

void inkify_query_result_free(InkifyQueryResult result) {
    if (result.uuids) {
        for (uint32_t i = 0; i < result.count; ++i) {
            free((void*)result.uuids[i]);
        }
        delete[] result.uuids;
    }
}

}
