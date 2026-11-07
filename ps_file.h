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

#ifndef _H_PS_FILE_H_
#define _H_PS_FILE_H_

#ifdef PS_FILE_IMP
#define PS_CORE_IMP
#define PS_COLLECTIONS_IMP
#define PS_LOG_IMP
#endif // PS_FILE_IMP

#include "ps_core.h"
#include "ps_collections.h"
#include "ps_log.h"

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define PS_HANDLE HANDLE
#define PS_WINDOWS_EPOCH_DIFF 116444736000000000ULL
#endif

#ifdef _WIN32
#define PS_PATH_SEP '\\'
#define PS_ALT_PATH_SEP '/'
#else
#define PS_PATH_SEP '/'
#define PS_ALT_PATH_SEP '\\'
#endif

typedef struct {
    char *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_path_t;

typedef struct ps_path_list_t {
    ps_path_t *items;
    ps_half_t capacity;
    ps_half_t size;
} ps_path_list_t;

PS_DEF int ps_glob_match(const char *pattern, const char *str);

PS_DEF void ps_path_normalize_sep(char *path);

PS_DEF int ps_dir_exists(const char *path);
PS_DEF int ps_dir_ensure(const char *path);
PS_DEF int ps_dir_create(const char *path);
PS_DEF int ps_dir_delete(const char *path);
PS_DEF ps_path_list_t ps_dir_find_files(const char *dir, const char *blob, int recursive);

PS_DEF int ps_file_exists(const char *path);
PS_DEF int ps_file_delete(const char *path);
PS_DEF int ps_file_rename(const char *src_path, const char *dest_path);
PS_DEF time_t ps_file_get_mod_time(const char *path);

#define _ps_path_parts(path_p) (_ps_list_split(&(path_p)->items, (char[]){PS_PATH_SEP}))
#define _ps_path_basename(path_p) (_ps_list_items(&(path_p)->items) + _ps_list_find_last(&(path_p)->items, (char[]){PS_PATH_SEP}) + 1)

#ifdef PS_FILE_IMP

PS_DEF int ps_glob_match(const char *pattern, const char *str){
    const char *p = pattern;
    const char *s = str;
    const char *star = NULL;
    const char *ss = NULL;

    while (*s) {
        if (*p == *s) {
            p++;
            s++;
        } else if (*p == '?') {
            p++;
            s++;
        } else if (*p == '*') {
            star = p++;
            ss = s;
        } else if (star) {
            p = star + 1;
            s = ++ss;
        } else {
            return 0;
        }
    }

    while (*p == '*') {
        p++;
    }

    return *p == '\0';
}

PS_DEF void ps_path_normalize_sep(char *path){
    while (*path){
        if (*path == PS_ALT_PATH_SEP){
            *path = PS_PATH_SEP;
        }
        path++;
    }
}

PS_DEF int ps_dir_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

PS_DEF int ps_dir_create(const char *path){
#ifdef _WIN32
    if (CreateDirectoryA(path,NULL)){
        ps_log(PS_LOG_INFO, "Created directory %s\n", path);
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to create directory %s, %lu\n", path, GetLastError());
    return 0;
#else
    PS_TODO("Unimplemented");
#endif
}

PS_DEF int ps_dir_ensure(const char *path){
#ifdef _WIN32
    if (CreateDirectoryA(path,NULL)){
        ps_log(PS_LOG_INFO, "Created directory: %s\n",path);
        return 1;
    }
    DWORD error = GetLastError();
    if (error == ERROR_ALREADY_EXISTS){
        ps_log(PS_LOG_INFO, "Directory %s already exits\n", path);
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to create directory %s, %lu\n", path, error);
    return 0;
#else
    PS_TODO("Unimplemented");
#endif
}


PS_DEF int ps_dir_delete(const char *path){
#ifdef _WIN32
    if (RemoveDirectoryA(path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to delete directory %s\n", path);
    return 0;
#else
    PS_TODO("Unimplemented");
    return 0;
#endif
}

PS_DEF ps_path_list_t ps_dir_find_files(const char *dir, const char *blob, int recursive){
#ifdef _WIN32
    ps_path_list_t result = {0};
    if (!dir || !blob) goto return_l;
    const ps_size_t dir_len = strlen(dir);
    if (dir_len == 0 || (dir[dir_len - 1] != PS_PATH_SEP && dir[dir_len - 1] != PS_ALT_PATH_SEP) || !ps_dir_exists(dir)) goto return_l;
    ps_path_t **r_ptr = (ps_path_t **)&result;
    const char ***dir_stack = ((const char***)&((ps_list_t){0}));
    char **search_blob = ((char **)&((ps_list_t){0}));
    char *dir_copy = _ps_realloc(NULL, dir_len + 1);
    _ps_memcpy((char *)dir_copy, dir, dir_len + 1);
    ps_path_normalize_sep(dir_copy);
    _ps_list_append_one(dir_stack, dir_copy);
    WIN32_FIND_DATA find_data;
    do {
        const char *current_dir = _ps_list_pop(dir_stack);
        if (!current_dir) break;
        const ps_size_t current_dir_len = strlen(current_dir);
        _ps_list_size(search_blob) = 0;
        _ps_list_append_many(search_blob, current_dir, current_dir_len + 2);
        _ps_list_set(search_blob, current_dir_len, '*');
        _ps_list_set(search_blob, current_dir_len + 1, '\0');
        // Process the current directory here
        PS_HANDLE iter = FindFirstFile(*search_blob, &find_data);
        if (iter == INVALID_HANDLE_VALUE){
            ps_log(PS_LOG_ERROR, "Failed to open directory %s. (%lu)\n", *search_blob, GetLastError());
            _ps_after_goto(
                _ps_free((void *)current_dir);
                _ps_list_foreach(dir_stack, item){
                    _ps_free((void *)*item);
                }
                ,cleanup_l
            );
        }
        do {
            if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;
            if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY){
                if (recursive){
                    char *sub_dir = _ps_realloc(NULL, current_dir_len + strlen(find_data.cFileName) + 2);
                    _ps_memcpy(sub_dir, current_dir, current_dir_len);
                    _ps_memcpy(sub_dir + current_dir_len, find_data.cFileName, strlen(find_data.cFileName));
                    sub_dir[current_dir_len + strlen(find_data.cFileName)] = PS_PATH_SEP;
                    sub_dir[current_dir_len + strlen(find_data.cFileName) + 1] = '\0';
                    _ps_list_append_one(dir_stack, sub_dir);
                }
            }else{
                if (ps_glob_match(blob, find_data.cFileName)){
                    ps_size_t str_len = strlen(find_data.cFileName);
                    ps_path_t file_path = {0};
                    _ps_list_append_many(&file_path.items, current_dir, current_dir_len);
                    _ps_list_append_many(&file_path.items, find_data.cFileName, str_len + 1);
                    _ps_list_append_one(r_ptr, file_path);
                }
            }
        } while(FindNextFile(iter, &find_data) != 0);
        _ps_free((void *)current_dir);
        FindClose(iter);
    } while(_ps_list_size(dir_stack) > 0);
    cleanup_l:
    _ps_list_free(search_blob);
    _ps_list_free(dir_stack);
    return_l:
        return result;
#else
    PS_TODO("Unimplemented");
#endif
}

PS_DEF int ps_file_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

PS_DEF int ps_file_delete(const char *path){
#ifdef _WIN32
    if (DeleteFileA(path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to delete file %s\n", path);
    return 0;
#else
    PS_TODO("Unimplemented");
    return 0;
#endif
}

PS_DEF int ps_file_rename(const char *src_path, const char *dest_path){
#ifdef _WIN32
    if (MoveFileA(src_path, dest_path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to rename file %s to %s (%lu)\n", src_path, dest_path, GetLastError());
    return 0;
#else
    PS_TODO("Unimplemented");
    return 0;
#endif
}

PS_DEF time_t ps_file_get_mod_time(const char *path){
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)){
        ps_log(PS_LOG_ERROR, "Unable to get file mod time for %s, (%lu)\n", path, GetLastError());
        return 0;
    }

    ULARGE_INTEGER ull;
    ull.LowPart = file_info.ftLastWriteTime.dwLowDateTime;
    ull.HighPart = file_info.ftLastWriteTime.dwHighDateTime;

    if (ull.QuadPart < PS_WINDOWS_EPOCH_DIFF){
        ps_log(PS_LOG_ERROR, "ERROR!!! %llu \n", ull.QuadPart);
        return 0;
    }
    unsigned long long unix_time = (ull.QuadPart - PS_WINDOWS_EPOCH_DIFF) / 10000000ULL;
    return (time_t)unix_time;
#else
    PS_TODO("Unimplemented");
    return 0;
#endif
}

#endif // PS_FILE_IMP

#endif // _H_PS_FILE_H_