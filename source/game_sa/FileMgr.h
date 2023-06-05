/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once

#include <cstdio>

// For backward compatibility
typedef FILE* FILESTREAM;
constexpr size_t DIRNAMELENGTH = 128;

static constexpr auto TopLineEmptyFile{ "THIS FILE IS NOT VALID YET" };

class CFileMgr {
public:
    static char (&ms_dirName)[128];
    static char (&ms_rootDirName)[128];

    static void Initialise();
    static int32 ChangeDir(const char* path);
    static int32 SetDir(const char* path);
    static int32 SetDirMyDocuments();
    static size_t LoadFile(const char* path, uint8* buf, size_t size, const char* mode);
    static FILESTREAM OpenFile(const char* path, const char* mode);
    static FILESTREAM OpenFileForWriting(const char* path);
    static FILESTREAM OpenFileForAppending(const char* path);
    static size_t Read(FILESTREAM file, void* buf, size_t size);
    static size_t Write(FILESTREAM file, const void* buf, size_t size);
    static bool Seek(FILESTREAM file, long offset, int32 origin);
    static bool ReadLine(FILESTREAM file, char* str, int32 num);
    static int32 CloseFile(FILESTREAM file);
    static int32 GetTotalSize(FILESTREAM file);
    static int32 Tell(FILESTREAM file);
    static bool GetErrorReadWrite(FILESTREAM file);

    //! Increment the file's seek pointer until after the next new line (`\n`) (make sure file is open in non-binary mode!)
    static void SeekNextLine(FILESTREAM file);

private:
    friend void InjectHooksMain();
    static void InjectHooks();
};

char* InitUserDirectories();
