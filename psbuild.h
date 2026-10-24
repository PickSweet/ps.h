#ifndef PSBUILD_H
#define PSBUILD_H

#include "pscore.h"
#include "pscollection.h"

typedef struct psb_config_t {
    const char* root_dir;
} psb_config_t;

typedef int (*psb_fn_t)(void*);
typedef size_t ps_idx_t;
typedef struct ps_graph_node_t {
    PS_CLOSURE_T(int, void) closure;
    ps_idx_t parent;
    PS_LIST_T(ps_idx_t) children;
} ps_graph_node_t;


typedef PS_LIST_T(ps_graph_node_t) ps_graph_t;

ps_idx_t psb_append_child_cmd_(ps_graph_t *graph, ps_idx_t parent, psb_fn_t fn, void *ctx, size_t ctx_size);
void psb_free(ps_graph_t *graph);


#define psb_graph_append_child_cmd(graph, parent_idx, p_fn)\
    psb_append_child_cmd_(graph,parent_idx,(psb_fn_t)(p_fn),NULL,0)


#define psb_graph_append_cmd(graph, p_fn)\
    psb_graph_append_child_cmd(graph,0,p_fn)


#define psb_graph_append_child_cmd_with_ctx(graph, parent_idx, p_fn, T, ...)\
    psb_append_child_cmd_(graph, parent_idx, (psb_fn_t)(p_fn), (void *)&((T){__VA_ARGS__}),sizeof((T){__VA_ARGS__}))

#define psb_graph_append_cmd_with_ctx(graph,p_fn,T,...)\
    psb_graph_append_child_cmd_with_ctx(graph,0,p_fn,T,__VA_ARGS__)

typedef struct psb_frame_t{
    ps_idx_t node_idx;
    size_t child_idx; // Tracks which child we are currently visiting
} psb_frame_t;

typedef PS_LIST_T(psb_frame_t) psb_frame_stack; 

PS_DEF int psb_run_(ps_graph_t *graph);

#define psb_run(graph) \
    PS_BLK( \
        psb_run_((graph)); \
    )


#ifdef PSBUILD_IMPLEMENTATION

ps_idx_t psb_append_child_cmd_(ps_graph_t *graph, ps_idx_t parent, psb_fn_t fn, void *ctx,size_t ctx_size){
    void *ctx_cp = PS_REALLOC(NULL,ctx_size);
    PS_MEMCPY(ctx_cp,ctx,ctx_size);
    ps_graph_node_t node = {0};
    node.closure.fn = fn;
    node.closure.ctx = ctx_cp;
    node.parent = parent;
    ps_list_append(graph,node);
    ps_idx_t new_idx = graph->size-1;
    ps_list_append(&(graph->items[parent].children),new_idx);
    return  new_idx;
}

void psb_free(ps_graph_t *graph){
    ps_list_foreach(ps_graph_node_t,node,graph){
        ps_list_free(&node->children);
        PS_FREE(node->closure.ctx);
        node->closure.ctx = NULL;
        node->closure.fn = NULL;
        node->parent = 0;
    }
    ps_list_free(graph);
}

PS_DEF int psb_run_(ps_graph_t *graph) {
    // Create a local dynamic array to act as our explicit stack
    psb_frame_stack stack = {0};
    PS_LIST_T(char) states = {0};
    ps_list_init_with_size(&states,graph->size);

    // Push the root node (index 0) to start
    ps_list_set(&states, 0, 1);
    ps_list_append(&stack, ((psb_frame_t){ .node_idx = 0, .child_idx = 0 }));

    int result = 0;
    while (!ps_list_is_empty(&stack)) {
        // Look at the top frame without popping it yet
        psb_frame_t *frame = &ps_list_last(&stack);
        ps_graph_node_t *node = &graph->items[frame->node_idx];

        size_t num_children = node->children.size;

        // 1. If there are unvisited children, push the next child onto the stack
        if (frame->child_idx < num_children) {
            ps_idx_t next_child_idx = node->children.items[frame->child_idx];
            
            // Advance parent's child index for the next iteration
            frame->child_idx++;
            if(ps_list_get(&states, next_child_idx) == 1){
                PS_AFTER_GOTO(
                    result = -1;,
                    defer
                );
            }
            if (ps_list_get(&states, next_child_idx) == 0){
                ps_list_set(&states, next_child_idx, 1);
                ps_list_append(&stack, ((psb_frame_t){ .node_idx = next_child_idx, .child_idx = 0 }));
            }
        } 
        // 2. All children finished! Pop this frame and execute the node.
        else {
            ps_idx_t completed_idx = frame->node_idx;
            ps_list_pop(&stack); // Remove frame from stack
            ps_list_set(&states, completed_idx, 2);
            if (node->closure.fn != NULL){
                int res = node->closure.fn(node->closure.ctx);
                if (res != 0) {
                    PS_AFTER_GOTO(
                        result = res;,
                        defer
                    );
                }
            }
        }
    }
    defer:
        ps_list_free(&stack);
        ps_list_free(&states);
        return result;
}

#ifndef PSBUILD_NO_BOILER

#define PSLOG_IMPLEMENTATION
#include "pslog.h"

#ifndef PSB_TEMP_SIZE
#define PSB_TEMP_SIZE 1024*1024
#endif // PSB_TEMP_SIZE

PS_DEF ps_graph_t psb_graph = {0};
PS_DEF PS_LIST_T(char) psb_temp;
PS_DEF PS_LIST_T(size_t) psb_temp_marks;

#define psb_append_child_cmd(parent_idx, p_fn)\
    psb_graph_append_child_cmd(&psb_graph,parent_idx, p_fn)

#define psb_append_cmd(p_fn)\
    psb_graph_append_cmd(&psb_graph,p_fn)

#define psb_append_child_cmd_with_ctx(parent_idx,p_fn,T, ...)\
    psb_graph_append_child_cmd_with_ctx(&psb_graph,parent_idx, p_fn, T, __VA_ARGS__)

#define psb_append_cmd_with_ctx(p_fn,T, ...)\
    psb_graph_append_cmd_with_ctx(&psb_graph,p_fn, T, __VA_ARGS__)

int build(int argc, char** argv);
int main(int argc, char** argv) {
    ps_list_reserve_precise(&psb_temp, PSB_TEMP_SIZE);
    ps_list_append(&psb_graph,(ps_graph_node_t){0});
    int result = build(argc,argv);
    if(result) return 1;
    psb_run(&psb_graph);
    psb_free(&psb_graph);
    return 0;
}

void psb_temp_mark(){
    ps_list_append(&psb_temp_marks, psb_temp.size);
}

void psb_temp_unmark(){
    size_t mark = ps_list_pop(&psb_temp_marks);
    ps_list_resize(&psb_temp, mark);
}

int psb_shell_cmd_fn(const char **ctx) {
    psb_temp_mark();

    
    size_t start_offset = ps_list_last(&psb_temp_marks);

    const char **p = ctx;
    while (*p != NULL) {
        size_t len = strlen(*p);
        if (psb_temp.size > start_offset) {
            ps_list_append(&psb_temp, ' ');
        }
        ps_list_append_many(&psb_temp, *p, len);
        p++;
    }
    ps_list_append(&psb_temp, '\0');

    // Get a pointer to our freshly built command string in the arena
    char *cmd = &ps_list_get(&psb_temp,start_offset);

    ps_log(PS_LOG_INFO, "Running: %s\n", cmd);
    int ret = system(cmd);

    // Wipe away everything we allocated for this command instantly
    psb_temp_unmark();

    return ret;
}

#define psb_append_shell_cmd(arg1,...)\
    psb_append_cmd_with_ctx(psb_shell_cmd_fn, const char *[], arg1, ##__VA_ARGS__,NULL)

#define psb_append_child_shell_cmd(parent_idx, ...)\
    psb_append_child_cmd_with_ctx(parent_idx, psb_shell_cmd_fn, const char*[], ##__VA_ARGS__, NULL)

#endif // PSBUILD_NO_BOILER
#endif // PSBUILD_IMPLEMENTATION
#endif // PSBUILD_H
