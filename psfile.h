#ifndef PSFILE_H
#define PSFILE_H

#include "pscore.h"
#include "pslog.h"

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
#endif

int ps_file_exists(const char *path);
int ps_dir_exists(const char *path);
int ps_ensure_dir(const char *path);
int ps_create_dir(const char *path);
int ps_delete_file(const char *path);
int ps_delete_dir(const char *path);


#ifdef PSFILE_IMP
int ps_file_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && !(attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_dir_exists(const char *path){
#ifdef _WIN32
    DWORD attrs = GetFileAttributesA(path);
    return (attrs != INVALID_FILE_ATTRIBUTES && (attrs & FILE_ATTRIBUTE_DIRECTORY));
#else
    PS_TODO("Unimplemented");
#endif
}

int ps_create_dir(const char *path){
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

int ps_ensure_dir(const char *path){
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

int ps_delete_file(const char *path){
#ifdef _WIN32
    ps_log(PS_LOG_INFO, "Attempting to remove file %s\n", path);
    return DeleteFileA(path) != 0;
#else
    PS_TODO("Unimplemented");
#endif
}


int ps_delete_dir(const char *path){
#ifdef _WIN32
    ps_log(PS_LOG_INFO, "Attempting to remove directory %s\n", path);
    return RemoveDirectoryA(path) != 0;
#else
    PS_TODO("Unimplemented");
#endif
}

#endif // PSFILE_IMP

#endif // PSFILE_H