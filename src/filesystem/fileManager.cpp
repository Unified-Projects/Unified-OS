#include "fileManager.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"
#include "../Paging/PageFrameAllocator.h"
#include "../scheduling/pit/pit.h"

//Global FM
FileManager GlobalFileManager;

//Initaliser
void FileManager::Initialize(){
    
}

//Validates and entry
bool ValidatePath(const char* path, size_t Size){
    if(Size >= 8){
        if(path[0] < NextMountable && path[0] >= 0x41){
            if(path[1] == ':'){
                if(path[2] == '\\' || path[2] == '/'){
                    return true;
                }
            }
        }
    }
    return false;
}

//Check how many directories there are
uint64_t CountLayers(const char* path, size_t Size){
    uint64_t Layers = 0;

    //Loop over characters
    for(int i = 0; i < Size; i++){
        if(path[i] == '\\' || path[i] == '/'){
            Layers++;
        }
    }

    return Layers - 1;
}

//Returns the file from a entry
FILE::File FileManager::FindFile(const char* path){
    //Create Empty
    FILE::File file;

    //Size of path
    uint64_t Size = strlen(path);

    //Validate
    if(!ValidatePath(path, Size)) return file;

    //Location of Parititon
    Partition* partition;
    Drive* drive;
    bool FoundPartition;

    //Look for patition in drives
    for (int d = 0; d < DriveCount; d++){
        drive = Drives[d];

        //Loop over paritions
        for(int p = 0; p < drive->DriveGPT->EntryCount; p++){
            
            //Check if it is mounted
            if(drive->Partitions[p].Mounted){
                
                //If mounted check if is correct parititon
                if(drive->Partitions[p].MountPoint == path[0]){
                    
                    //If correct fix references and exit loop
                    FoundPartition = true;
                    partition = &drive->Partitions[p];
                    break;
                }
            }
        }

        if(FoundPartition)
            break;
    }

    //Count layers ("A:/foo/bar/...")
    uint64_t Layers = CountLayers(path, Size);

    //First Do Root
    DIR::Directory LayerDir(2, drive, partition);

    //Path Offset
    uint64_t PathOffset = 3;

    //Found Yet
    bool Found = false;
    FILE::File File;

    //Layering
    bool DirFound = false;

    //Loop over directory Entries
    for(int e = 0; e < LayerDir.SizeOfEntries; e++){
        //Create a temporary File
        FILE::File f(LayerDir.DirEntries[e]);

        //Offsetting of the path
        PathOffset = 3;

        //Check if Main FileName is the same
        if(strcmp(f.FileName, path + PathOffset, f.FileNameSize)){

            //Apply offset to check extention
            PathOffset+=f.FileNameSize + 1;

            //Compare extentions
            if(strcmp(f.Extention, path + PathOffset, f.ExtentionSize) || f.ExtentionSize == 0){
                //Check if directory (Another Layer Needed)
                if(!f.DirEntry.RDE.FileAttributes.Directory && Layers == 0){
                    //Create File
                    Found = true;
                    File = FILE::File(LayerDir.DirEntries[e], drive, partition);

                    //Return the file
                    return File;
                }
                //Otherwise if it is a directory and Layered
                else if(f.DirEntry.RDE.FileAttributes.Directory && Layers > 0){
                    //Locate Dir
                    DirFound = true;

                    //Find Cluster of Dir
                    uint32_t FATStartEntry = (((((uint32_t)f.DirEntry.RDE.High2BytesOfAddressOfFirstCluster) << 16) & 0xFFFF0000) |
                                                       f.DirEntry.RDE.Low2BytesOfAddressOfFirstCluster);

                    //Read dir
                    LayerDir = DIR::Directory(FATStartEntry, drive, partition);

                    break;
                }
            }
        }
    }

    //Look over any other layers
    for(int l = 0; l < Layers; l++){
        //Stop Any Repetitions of Directories
        if(DirFound){
            DirFound = false;
            
            uint64_t BackupPathOffset = PathOffset;

            //Loop over directory Entries
            for(int e = 0; e < LayerDir.SizeOfEntries; e++){
                if(DirFound) continue;

                //Create a temporary File
                FILE::File f(LayerDir.DirEntries[e]);

                //Offsetting of the path
                PathOffset = BackupPathOffset;

                //Check if Main FileName is the same
                if(strcmp(f.FileName, path + PathOffset, f.FileNameSize)){

                    //Apply offset to check extention
                    PathOffset+=f.FileNameSize + 1;

                    //Compare extentions
                    if(strcmp(f.Extention, path + PathOffset, f.ExtentionSize)){
                        //Check if directory (Another Layer Needed)
                        if(!f.DirEntry.RDE.FileAttributes.Directory && Layers - l == 1){

                            //Create File
                            Found = true;
                            File = FILE::File(LayerDir.DirEntries[e], drive, partition);

                            //Return the file
                            return File;
                        }
                        //Otherwise if it is a directory and Layered
                        else if(f.DirEntry.RDE.FileAttributes.Directory && Layers > l+1){
                            //Locate Dir
                            DirFound = true;

                            //Find Cluster of Dir
                            uint32_t FATStartEntry = (((((uint32_t)f.DirEntry.RDE.High2BytesOfAddressOfFirstCluster) << 16) & 0xFFFF0000) |
                                                            f.DirEntry.RDE.Low2BytesOfAddressOfFirstCluster);

                            //Read dir
                            LayerDir = DIR::Directory(FATStartEntry, drive, partition);
                        }
                    }
                }
            }
        }
    }

    //Return
    return file;
}