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

#ifndef PSFILE_H
#define PSFILE_H

#include "pscore.h"
#include "pslog.h"

#include <stdio.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#define PS_HANDLE HANDLE
#define PS_WINDOWS_EPOCH_DIFF 116444736000000000ULL
#endif

#define PS_INCLUDE_DIRECTORIES 0x1
#define PS_INCLUDE_FILES 0x2

int ps_dir_exists(const char *path);
int ps_dir_ensure(const char *path);
int ps_dir_create(const char *path);
int ps_dir_delete(const char *path);
ps_list_t(char*) ps_dir_find_files(const char *path, int ps_include);


int ps_file_exists(const char *path);
int ps_file_delete(const char *path);
int ps_file_rename(const char *src_path, const char *dest_path);
time_t ps_file_get_mod_time(const char *path);


#ifdef PSFILE_IMP

int ps_dir_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_dir_create(const char *path){
#ifdef _WIN32
    if (CreateDirectoryA(path,NULL)){
        ps_log(PS_LOG_INFO, "Created directory %s\n", path);
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to create directory %s, %d\n", path, GetLastError());
    return 0;
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_dir_ensure(const char *path){
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
    ps_log(PS_LOG_ERROR, "Unable to create directory %s, %d\n", path, error);
    return 0;
#else
    PS_TODO("Unimplemented");
#endif
}


int ps_dir_delete(const char *path){
#ifdef _WIN32
    if (RemoveDirectoryA(path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to delete directory %s\n", path);
#else
    PS_TODO("Unimplemented");
#endif
}

ps_list_t(char*) ps_dir_find_files(const char *path, int ps_include){
#ifdef _WIN32
    WIN32_FIND_DATA find_data;
    ps_list_t(char*) result = ps_list_new(char *);
    PS_HANDLE iter = FindFirstFile(path, &find_data);
    if (iter == INVALID_HANDLE_VALUE){
        ps_log(PS_LOG_ERROR, "Failed to open directory %s. (%d)\n", path, GetLastError());
        ps_list_free(result);
        return NULL;
    }
    ps_log(PS_LOG_INFO, "Searching directory %s\n", path);
    do {
        if (strcmp(find_data.cFileName, ".") == 0 || strcmp(find_data.cFileName, "..") == 0) continue;
        if (((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)){
            if (ps_include && PS_INCLUDE_FILES){
                size_t str_len = strlen(find_data.cFileName);
                char *str = PS_REALLOC(NULL, str_len + 1);
                PS_MEMCPY(str,find_data.cFileName,str_len);
                str[str_len] = '\0';
                ps_list_append(result, str);
            }
        }else if((ps_include & PS_INCLUDE_FILES) == PS_INCLUDE_FILES){
            size_t str_len = strlen(find_data.cFileName);
            char *str = PS_REALLOC(NULL, str_len + 1);
            PS_MEMCPY(str,find_data.cFileName,str_len);
            str[str_len] = '\0';
            ps_list_append(result, str);
        }
    }while(FindNextFile(iter, &find_data) != 0);
    FindClose(iter);
    return result;
#else
    PS_TODO("Unimplemented");
#endif    
}


int ps_file_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_file_delete(const char *path){
#ifdef _WIN32
    if (DeleteFileA(path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to delete file %s\n", path);
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_file_rename(const char *src_path, const char *dest_path){
#ifdef _WIN32
    if (MoveFileA(src_path, dest_path)){
        return 1;
    }
    ps_log(PS_LOG_ERROR, "Unable to rename file %s to %s (%d)\n", src_path, dest_path, GetLastError());
#else
    PS_TODO("Unimplemented");
#endif
}

time_t ps_file_get_mod_time(const char *path){
#ifdef _WIN32
    WIN32_FILE_ATTRIBUTE_DATA file_info;
    if (!GetFileAttributesExA(path, GetFileExInfoStandard, &file_info)){
        ps_log(PS_LOG_ERROR, "Unable to get file mod time for %s, (%d)\n", path, GetLastError());
        return (time_t)-1;
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
#endif
}

#endif // PSFILE_IMP

#endif // PSFILE_H