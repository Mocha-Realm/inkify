#include "ink/inkify_c_api.h"
#include <stdio.h>
#include <assert.h>

int main() {
    printf("Testing Inkify C API...\n");

    InkifyStrokeManagerPtr sm = inkify_stroke_manager_create();
    assert(sm != NULL);

    inkify_stroke_manager_add_empty_stroke(sm, "stroke-1", 100);
    assert(inkify_stroke_manager_get_active_count(sm) == 1);

    inkify_stroke_manager_delete_stroke(sm, "stroke-1", 200);
    assert(inkify_stroke_manager_is_deleted(sm, "stroke-1") == true);
    assert(inkify_stroke_manager_get_active_count(sm) == 0);

    InkifyChunkManagerPtr cm = inkify_chunk_manager_create(1024.0f);
    assert(cm != NULL);

    // Test query (should be empty)
    InkifyQueryResult res = inkify_chunk_manager_query_viewport(cm, sm, 0, 0, 100, 100);
    assert(res.count == 0);
    inkify_query_result_free(res);

    inkify_chunk_manager_destroy(cm);
    inkify_stroke_manager_destroy(sm);

    printf("C API Test Passed!\n");
    return 0;
}
