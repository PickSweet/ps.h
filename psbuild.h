/*
MIT License

Copyright (c) 2026 David J. Meagher

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#ifndef PSBUILD_H
#define PSBUILD_H

#include "pscore.h"
#include "pscollection.h"
#ifdef PSBUILD_IMP
#define PSLOG_IMP
#define PSFILE_IMP
#endif
#include "pslog.h"
#include "psfile.h"

#ifndef CC
#define CC "gcc"
#endif

typedef int (*psb_fn_t)(void*);
typedef size_t ps_idx_t;
typedef struct ps_graph_node_t {
    PS_CLOSURE_T(int, void) closure;
    ps_list_t(ps_idx_t) children;
    ps_idx_t parent;
} ps_graph_node_t;


typedef ps_list_t(ps_graph_node_t) ps_graph_t;


typedef struct ps_builder_t {
    ps_list_t(char*) args;
    ps_graph_t graph;
} ps_builder_t;

ps_idx_t psb_append_child_cmd_(ps_graph_t graph, ps_idx_t parent, psb_fn_t fn, void *ctx, size_t ctx_size);
void psb_free(ps_graph_t graph);


#define psb_append_child_cmd(builder, parent_idx, p_fn)\
    psb_append_child_cmd_((builder)->graph,parent_idx,(psb_fn_t)(p_fn),NULL,0)


#define psb_graph_append_cmd(builder, p_fn)\
    psb_append_child_cmd(builder,0,p_fn)


#define psb_append_child_cmd_with_ctx(builder, parent_idx, p_fn, T, ...)\
    psb_append_child_cmd_((builder)->graph, parent_idx, (psb_fn_t)(p_fn), (void *)&((T){__VA_ARGS__}),sizeof((T){__VA_ARGS__}))

#define psb_append_cmd_with_ctx(builder,p_fn,T,...)\
    psb_append_child_cmd_with_ctx(builder,0,p_fn,T,__VA_ARGS__)

typedef struct psb_frame_t{
    ps_idx_t node_idx;
    size_t child_idx; // Tracks which child we are currently visiting
} psb_frame_t;

typedef ps_list_t(psb_frame_t) psb_frame_stack; 

PS_DEF int psb_run(ps_graph_t graph);

int psb_shell_cmd_fn(const char **ctx);

#define psb_append_shell_cmd(builder,...)\
    psb_append_cmd_with_ctx(builder,psb_shell_cmd_fn, const char*[], __VA_ARGS__, NULL);\


#define psb_append_child_shell_cmd(builder,parent_idx, ...)\
    psb_append_child_cmd_with_ctx(builder,parent_idx, psb_shell_cmd_fn, const char*[], __VA_ARGS__, NULL);\


#ifdef PSBUILD_IMP
#define PS_SRC_FILE() __FILE__
#define psb_rebuild_yourself(args) psb_rebuild_yourself_(args, __FILE__);

ps_idx_t psb_append_child_cmd_(ps_graph_t graph, ps_idx_t parent, psb_fn_t fn, void *ctx,size_t ctx_size){
    void *ctx_cp = PS_REALLOC(NULL,ctx_size);
    PS_MEMCPY(ctx_cp,ctx,ctx_size);
    ps_graph_node_t node = {0};
    node.closure.fn = fn;
    node.closure.ctx = ctx_cp;
    node.parent = parent;
    node.children = ps_list_new(ps_idx_t);
    ps_list_append(graph,node);
    ps_idx_t new_idx = ps_list_size(graph) - 1;
    ps_list_append((ps_list_get(graph,parent)).children,new_idx);
    return  new_idx;
}

void psb_free(ps_graph_t graph){
    ps_list_foreach(graph, node){
        ps_list_free(node->children);
        PS_FREE(node->closure.ctx);
        node->closure.ctx = NULL;
        node->closure.fn = NULL;
        node->parent = 0;
    }
    ps_list_free(graph);
}

PS_DEF int psb_run(ps_graph_t graph) {
    // Create a local dynamic array to act as our explicit stack
    psb_frame_stack stack = ps_list_init(psb_frame_t);
    ps_list_t(char) states = ps_list_init(char);
    ps_list_resize(states, ps_list_size(graph));
    PS_MEMSET(ps_list_ptr(states),0,ps_list_size(graph));

    // Push the root node (index 0) to start
    ps_list_set(states, 0, 1);
    ps_list_append(stack, ((psb_frame_t){ .node_idx = 0, .child_idx = 0 }));

    int result = 0;
    while (!ps_list_is_empty(stack)) {
        // Look at the top frame without popping it yet
        psb_frame_t *frame = &ps_list_last(stack);
        ps_graph_node_t *node = &ps_list_get(graph,frame->node_idx);

        size_t num_children = ps_list_size(node->children);

        // 1. If there are unvisited children, push the next child onto the stack
        if (frame->child_idx < num_children) {
            ps_idx_t next_child_idx = ps_list_get(node->children, frame->child_idx);
            
            // Advance parent's child index for the next iteration
            frame->child_idx++;
            if(ps_list_get(states, next_child_idx) == 1){
                PS_AFTER_GOTO(
                    result = -1;,
                    defer
                );
            }
            if (ps_list_get(states, next_child_idx) == 0){
                ps_list_set(states, next_child_idx, 1);
                ps_list_append_one(stack, ((psb_frame_t){ .node_idx = next_child_idx, .child_idx = 0 }));
            }
        } 
        // 2. All children finished! Pop this frame and execute the node.
        else {
            ps_idx_t completed_idx = frame->node_idx;
            ps_list_pop(stack); // Remove frame from stack
            ps_list_set(states, completed_idx, 2);
            
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
        ps_list_info_free(stack);
        ps_list_info_free(states);
        return result;
}

int psb_shell_cmd_fn(const char **ctx) {
    ps_list_t(char) scratch = ps_list_init(char);

    const char **p = ctx;
    while (*p != NULL) {
        size_t len = strlen(*p);
        if (ps_list_size(scratch) > 0) {
            ps_list_append(scratch, ' ');
        }
        ps_list_append_many(scratch, *p, len);
        p++;
    }
    ps_list_append(scratch, '\0');
    char *cmd = ps_list_ptr(scratch);
    ps_log(PS_LOG_INFO, "Executing: %s\n", cmd);
    int ret = system(cmd);
    ps_list_info_free(scratch);
    return ret;
}

int psb_needs_rebuild(ps_list_t(char *) args, const char *src_file){
    time_t src_time = ps_file_get_mod_time(src_file);
    time_t exe_time = ps_file_get_mod_time(ps_list_get(args,0));
    return src_time > exe_time;
}

void psb_rebuild_yourself_(ps_list_t(char*) args, const char *src_file){
    if (psb_needs_rebuild(args, src_file)){
        ps_log(PS_LOG_INFO, "Source file : %s updated, rebuilding %s\n", src_file, ps_list_get(args,0));
        size_t exe_path_len = strlen(ps_list_get(args,0));
        ps_list_t(char) cmd = ps_list_init(char);
        ps_list_t(char) old_exe_path = ps_list_init(char);
        ps_list_append_fmt(cmd, "%s %s -o %s", CC, src_file, ps_list_get(args,0));;
        ps_list_append_fmt(old_exe_path, "%s.old", ps_list_get(args,0));
        if (ps_file_exists(ps_list_ptr(old_exe_path))){
            ps_file_delete(ps_list_ptr(old_exe_path));
        }
        ps_file_rename(ps_list_get(args,0), ps_list_ptr(old_exe_path));
        system(*cmd);
#ifdef _WIN32
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcessA(NULL,ps_list_get(args,0),NULL,NULL,0,0,NULL,NULL,&si,&pi)){
            ps_log(PS_LOG_INFO, "Unable to restart %s", ps_list_get(args,0));
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#else
        PS_TODO("Unimplemented");
#endif
        ps_list_info_free(cmd);
        ps_list_info_free(old_exe_path);
        exit(0);
    }
}

#ifndef PSBUILD_NO_BOILER

int build(ps_builder_t *builder);
int main(int argc, char** argv) {
    ps_builder_t builder = {
        ps_list_init(char*),
        ps_list_init(ps_graph_node_t),
    };
    ps_list_append_many(builder.args,argv,argc);
    ps_list_append(builder.graph,(ps_graph_node_t){NULL,0,ps_list_new(ps_idx_t)});
    int result = build(&builder);
    if(result) goto defer;
    psb_run(builder.graph);
    defer:
        //psb_free(builder.graph);
        return result;
}

#endif // PSBUILD_NO_BOILER
#endif // PSBUILD_IMP
#endif // PSBUILD_H
