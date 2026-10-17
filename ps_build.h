#ifndef PS_BUILD_H
#define PS_BUILD_H

#include "_ps.h"
#include "ps_array.h"

typedef struct ps_build_config_t {
    const char* root_dir;
} ps_build_config_t;

#define PS_VFN_RESULT_SUCCESS 0
#define PS_VFN_RESULT_SKIP 1

typedef int (*ps_build_fn_t)(void*);
typedef size_t ps_idx_t;
typedef PS_DARRAY_T(ps_graph_node_t) ps_graph_t;

typedef struct ps_graph_node_t {
    ps_build_fn_t fn;
    void *ctx;
    ps_idx_t parent;
    PS_DARRAY_T(ps_idx_t) children;
} ps_graph_node_t;

#define PS_CALL(fn, result, ...) \
    PS_BLK( \
        PS_ASSERT(fn != NULL && "Function pointer is NULL"); \
        (result) = fn(__VA_ARGS__); \
    )

#define ps_build_append_cmd(graph,fn)\
    PS_BLK( \
        ps_graph_node_t node = {0}; \
        node.fn = (PS_BUILD_FN)(fn); \
        ps_darray_append(graph, node); \
    )

#define ps_build_append_child_cmd(graph, parent_idx, fn)\
    PS_BLK( \
        ps_graph_node_t node = {0}; \
        node.fn = (PS_BUILD_FN)(fn);\
        node.parent = (parent_idx); \
        ps_darray_append(graph, node); \
        ps_darray_append(&graph[parent_idx].children, graph.size - 1); \
    )

#define ps_build_append_cmd_with_ctx(graph, fn, T, ...)\
    PS_BLK(\
        ps_graph_node_t node = {0};\
        node.fn = (PS_BUILD_FN)(fn);\
        T *ctx = PS_REALLOC(NULL, sizeof(T));\
        *ctx = (T){__VA_ARGS__};\
        node.ctx = (void *)(ctx);\
        ps_darray_append(graph, node);\
    )

#define ps_build_append_child_cmd_with_ctx(graph, parent_idx, fn, T, ...)\
    PS_BLK(\
        ps_graph_node_t node = {0};\
        node.fn = (PS_BUILD_FN)(fn);\
        T *ctx = PS_REALLOC(NULL, sizeof(T));\
        *ctx = (T){__VA_ARGS__};\
        node.ctx = (void *)(ctx);\
        node.parent = parent_idx;\
        ps_darray_append(graph,node);\
        ps_darray_append(&graph[parent_idx].children, graph.size - 1);\
    )


typedef struct ps_build_frame_t{
    ps_idx_t node_idx;
    size_t child_idx; // Tracks which child we are currently visiting
} ps_build_frame_t;

typedef PS_DARRAY_T(ps_build_frame_t) ps_build_frame_stack; 

PS_DEF int ps_build_run_iterative(ps_graph_t *graph);

#define ps_build_run(graph) \
    PS_BLK( \
        ps_build_run_iterative(&(graph)); \
    )


#ifdef PS_BUILD_IMPLEMENTATION

PS_DEF int ps_shell_fn(const char **args){
    PS_TODO("SHELL FN")
}

PS_DEF int ps_build_run_iterative(ps_graph_t *graph) {
    // Create a local dynamic array to act as our explicit stack
    ps_build_frame_stack stack = {0};

    // Push the root node (index 0) to start
    ps_darray_append(&stack, ((ps_build_frame_t){ .node_idx = 0, .child_idx = 0 }));

    while (!ps_darray_is_empty(&stack)) {
        // Look at the top frame without popping it yet
        ps_build_frame_t *frame = &ps_darray_last(&stack);
        ps_graph_node_t *node = &graph->items[frame->node_idx];

        size_t num_children = node->children.size;

        // 1. If there are unvisited children, push the next child onto the stack
        if (frame->child_idx < num_children) {
            ps_idx_t next_child_idx = node->children.items[frame->child_idx];
            
            // Advance parent's child index for the next iteration
            frame->child_idx++;

            // Push the child frame
            ps_darray_append(&stack, ((ps_build_frame_t){ .node_idx = next_child_idx, .child_idx = 0 }));
        } 
        // 2. All children finished! Pop this frame and execute the node.
        else {
            ps_darray_pop(&stack); // Remove frame from stack
            if (node->fn == NULL) continue;
            int res = node->fn(node->ctx);
            if (res != PS_VFN_RESULT_SUCCESS && res != PS_VFN_RESULT_SKIP) {
                ps_darray_free(&stack);
                return res; // Stop on failure
            }
        }
    }

    ps_darray_free(&stack);
    return PS_VFN_RESULT_SUCCESS;
}

#ifndef PS_NO_BUILD 
static DARRAY(ps_graph_node_t) ps_build_graph = {0};
int build(int argc, char** argv);
int main(int argc, char** argv) {
    ps_darray_reserve(ps_build_graph,1);
    if(build(argc, argv)) return 1;
    ps_build_run(ps_build_graph);
}

#endif // PS_NO_BUILD
#endif // PS_BUILD_IMPLEMENTATION
#endif // PS_BUILD_H
