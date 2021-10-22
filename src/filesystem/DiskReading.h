#pragma once
#include "./DirectoryEntries.h"

//RDE Getter
ROOT_DIRECTORY_ENTRY GetRDE(void* buffer, uint64_t EntryOffset);

//LFN Getter
LFN_DRIECTORY_ENTRY GetLFNDE(void* buffer, uint64_t EntryOffset);

//Attribute Checker
bool isLFN(void* buffer, uint64_t EntryOffset);

//Entry for a file or a folder getter
DIRECTORY_ENTRY RootGetDirectoryEntry(void* buffer, uint64_t EntryOffset);

//Separate Folder Entries
DOT_ENTRY GetFolerInEntry();
ROOT_DIRECTORY_ENTRY GetFileInDirectory();