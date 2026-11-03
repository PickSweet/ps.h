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

#ifndef _H_PS_BUILD_H_
#define _H_PS_BUILD_H_

#include "ps_core.h"

#ifdef PS_BUILD_IMP
#define PS_COLLECTIONS_IMP
#define PS_LOG_IMP
#define PS_FILE_IMP
#endif
#include "ps_collections.h"
#include "ps_log.h"
#include "ps_file.h"

#ifndef CC
#define CC "gcc"
#endif

typedef int (*psb_fn_t)(void*);
typedef size_t ps_idx_t;
typedef struct ps_graph_node_t {
    _ps_closure_t(int, void) closure;
    ps_list_t children;
    ps_idx_t parent;
} ps_graph_node_t;


typedef ps_graph_node_t** ps_graph_t;


typedef struct ps_builder_t {
    char ***args;
    ps_graph_t graph;
} ps_builder_t;

ps_idx_t __i_ps_build_append_child_cmd(ps_graph_t graph, ps_idx_t parent, psb_fn_t fn, void *ctx, size_t ctx_size);
void psb_free(ps_graph_t graph);


#define _ps_build_append_child_cmd(builder, parent_idx, p_fn)\
    __i_ps_build_append_child_cmd((builder)->graph,parent_idx,(psb_fn_t)(p_fn),NULL,0)


#define _ps_build_graph_append_cmd(builder, p_fn)\
    _ps_build_append_child_cmd(builder,0,p_fn)


#define _ps_build_append_child_cmd_with_ctx(builder, parent_idx, p_fn, T, ...)\
    __i_ps_build_append_child_cmd((builder)->graph, parent_idx, (psb_fn_t)(p_fn), (void *)&((T){__VA_ARGS__}),sizeof((T){__VA_ARGS__}))

#define _ps_build_append_cmd_with_ctx(builder,p_fn,T,...)\
    _ps_build_append_child_cmd_with_ctx(builder,0,p_fn,T,__VA_ARGS__)

typedef struct ps_build_frame_t{
    ps_idx_t node_idx;
    size_t child_idx; // Tracks which child we are currently visiting
} ps_build_frame_t;

PS_DEF int ps_build_run(ps_graph_t graph);

int ps_build_shell_cmd_fn(const char **ctx);

#define _ps_build_append_shell_cmd(builder,...)\
    _ps_build_append_cmd_with_ctx(builder,ps_build_shell_cmd_fn, const char*[], __VA_ARGS__, NULL);\


#define _ps_build_append_child_shell_cmd(builder,parent_idx, ...)\
    _ps_build_append_child_cmd_with_ctx(builder,parent_idx, psb_shell_cmd_fn, const char*[], __VA_ARGS__, NULL);\

#ifdef PS_BUILD_IMP
#define PS_SRC_FILE() __FILE__
#define _ps_build_rebuild_yourself(args) __i_ps_build_rebuild_yourself(args, __FILE__);

ps_idx_t __i_ps_build_append_child_cmd(ps_graph_t graph, ps_idx_t parent, psb_fn_t fn, void *ctx,size_t ctx_size){
    void *ctx_cp = _ps_realloc(NULL,ctx_size);
    _ps_memcpy(ctx_cp,ctx,ctx_size);
    ps_graph_node_t node = {0};
    node.closure.fn = fn;
    node.closure.ctx = ctx_cp;
    node.parent = parent;
    node.children = (ps_list_t){0};
    _ps_list_append_one(graph,node);
    ps_idx_t new_idx = _ps_list_size(graph) - 1;
    _ps_list_append_one((ps_idx_t**)(&(_ps_list_get(graph,parent)).children.items),new_idx);
    return  new_idx;
}

void psb_free(ps_graph_t graph){
    _ps_list_foreach(graph, node){
        _ps_list_free(&node->children.items);
        _ps_free(node->closure.ctx);
        node->closure.ctx = NULL;
        node->closure.fn = NULL;
        node->parent = 0;
    }
    _ps_list_free(graph);
}

PS_DEF int ps_build_run(ps_graph_t graph) {
    // Create a local dynamic array to act as our explicit stack
    ps_build_frame_t **stack = (ps_build_frame_t**)(&(ps_list_t){0});
    char **states = (char**)(&(ps_list_t){0});
    _ps_list_resize(states, _ps_list_size(graph));
    _ps_memset(_ps_list_items(states),0,_ps_list_size(graph));

    // Push the root node (index 0) to start
    _ps_list_set(states, 0, 1);
    _ps_list_append(stack, ((ps_build_frame_t){ .node_idx = 0, .child_idx = 0 }));

    int result = 0;
    while (!_ps_list_is_empty(stack)) {
        // Look at the top frame without popping it yet
        ps_build_frame_t *frame = &_ps_list_last(stack);
        ps_graph_node_t *node = &_ps_list_get(graph,frame->node_idx);

        size_t num_children = _ps_list_size(&node->children.items);

        // 1. If there are unvisited children, push the next child onto the stack
        if (frame->child_idx < num_children) {
            ps_idx_t next_child_idx = _ps_list_get((ps_idx_t**)(&node->children.items), frame->child_idx);
            
            // Advance parent's child index for the next iteration
            frame->child_idx++;
            if(_ps_list_get(states, next_child_idx) == 1){
                _ps_after_goto(
                    result = -1;,
                    defer_l
                );
            }
            if (_ps_list_get(states, next_child_idx) == 0){
                _ps_list_set(states, next_child_idx, 1);
                _ps_list_append_one(stack, ((ps_build_frame_t){ .node_idx = next_child_idx, .child_idx = 0 }));
            }
        } 
        // 2. All children finished! Pop this frame and execute the node.
        else {
            ps_idx_t completed_idx = frame->node_idx;
            _ps_list_pop(stack); // Remove frame from stack
            _ps_list_set(states, completed_idx, 2);
            
            if (node->closure.fn != NULL){
                int res = node->closure.fn(node->closure.ctx);
                if (res != 0) {
                    _ps_after_goto(
                        result = res;,
                        defer_l
                    );
                }
            }
        }
    }
    defer_l:
        _ps_list_free(stack);
        _ps_list_free(states);
        return result;
}

int ps_build_shell_cmd_fn(const char **ctx) {
    char **scratch = (char**)(&(ps_list_t){0});

    const char **p = ctx;
    while (*p != NULL) {
        size_t len = strlen(*p);
        if (_ps_list_size(scratch) > 0) {
            _ps_list_append_one(scratch, ' ');
        }
        _ps_list_append_many(scratch, *p, len);
        p++;
    }
    _ps_list_append_one(scratch, '\0');
    char *cmd = *scratch;
    ps_log(PS_LOG_INFO, "Executing: %s\n", cmd);
    int ret = system(cmd);
    _ps_list_free(scratch);
    return ret;
}

int __i_ps_build_needs_rebuild(char  ***args, const char *src_file){
    time_t src_time = ps_file_get_mod_time(src_file);
    time_t exe_time = ps_file_get_mod_time(_ps_slice_get(args,0));
    return src_time > exe_time;
}

void __i_ps_build_rebuild_yourself(char ***args, const char *src_file){
    if (__i_ps_build_needs_rebuild(args, src_file)){
        ps_log(PS_LOG_INFO, "Source file : %s updated, rebuilding %s\n", src_file, _ps_slice_get(args,0));
        size_t exe_path_len = strlen(_ps_slice_get(args,0));
        char **cmd = (char**)(&(ps_list_t){0});
        char ** old_exe_path = (char**)(&(ps_list_t){0});
        ps_str_append_fmt(cmd, "%s %s -o %s", CC, src_file, _ps_slice_get(args,0));
        ps_str_append_fmt(old_exe_path, "%s.old", _ps_slice_get(args,0));
        if (ps_file_exists(*old_exe_path)){
            ps_file_delete(*old_exe_path);
        }
        ps_file_rename(_ps_slice_get(args,0), *old_exe_path);
        int err = system(*cmd);
        if (err){
            ps_file_rename(*old_exe_path, _ps_slice_get(args,0));
            goto return_l;
        };
#ifdef _WIN32
        STARTUPINFOA si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        if (!CreateProcessA(NULL,_ps_slice_get(args,0),NULL,NULL,0,0,NULL,NULL,&si,&pi)){
            ps_log(PS_LOG_INFO, "Unable to restart %s", _ps_slice_get(args,0));
        }
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
#else
        PS_TODO("Unimplemented");
#endif
    return_l:
        _ps_list_free(cmd);
        _ps_list_free(old_exe_path);
        exit(0);
    }
}

#ifndef PSBUILD_NO_BOILER

int build(ps_builder_t *builder);
int main(int argc, char** argv) {
    ps_builder_t builder = {
        (char ***)(&_ps_get_slice(argv,argc)),
        (ps_graph_t)(&(ps_list_t){0})
    };
    _ps_list_append_one(builder.graph,(ps_graph_node_t){0});
    int result = build(&builder);
    if(result) goto defer_l;
    ps_build_run(builder.graph);
    defer_l:
        return result;
}

#endif // PSBUILD_NO_BOILER
#endif // PSBUILD_IMP
#endif // PSBUILD_H
