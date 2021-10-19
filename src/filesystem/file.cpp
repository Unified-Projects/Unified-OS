#include "file.h"
#include "ByteInteraction.h"
#include "../memory/heap.h"
#include "../Paging/PageFrameAllocator.h"

namespace FILE{
    //Reads the data from a directory entry
    File readFileData(DIRECTORY_ENTRY dirEntry, AHCI::Port* FileDrive){
        //Setup before reading
        File returnVal = File{};
        
        //Set Entry
        returnVal.DirEntry = dirEntry;

        //LFNVersion
        if(dirEntry.LFNEntries > 0){
            //Values for locationing
            bool extFound = false;
            uint64_t extIndex = 0;
            uint64_t fnIndex = 0;
            uint64_t checkLength = 0;
            char backupExt[16];

            //Loop Over entries Backwards
            for(int e = 15 - dirEntry.LFNEntries + 1; e <= 15; e++){
                //Get Entry
                LFN_DRIECTORY_ENTRY l = (dirEntry.LFNE[e]);
                
                //Loop Over Characters
                for(int ci = 0; ci < 13; ci++){
                    if(l.FileName[ci] != 0x20){
                        //Check for "." as an extention starter
                        if(l.FileName[ci] == 0x2E){
                            extFound = true;
                            continue;
                        }

                        //Check if extention or not
                        if(extFound){
                            //Ensure it is extention If not undo and put onto file name
                            if(l.FileName[ci] == 0x2E){
                                //Loop over characters
                                while (checkLength > 0)
                                {   
                                    //Modify values swapping
                                    returnVal.FileName[fnIndex++] = l.FileName[ci];
                                    returnVal.Extension[checkLength--] = 0x00;
                                }
                            }
                            else{
                                //Otherwise add to extention
                                returnVal.Extension[extIndex++] = l.FileName[ci];
                                backupExt[checkLength++] = l.FileName[ci];
                            }
                        }
                        else{
                            //Add to filename
                            returnVal.FileName[fnIndex++] = l.FileName[ci];
                        }
                    }
                }
            }

            //Set sizes
            returnVal.FileNameSize = fnIndex - 1;
            returnVal.ExtentionSize = extIndex - 1;
        }
        //NonLFN
        else{
            //Get the filename(first 8) Ignore spaces
            for(int ci = 0; ci < 8; ci++){
                if(dirEntry.RDE.FileName[ci] != 0x20){
                    returnVal.FileName[returnVal.FileNameSize++] = dirEntry.RDE.FileName[ci];
                }
            }

            //Get ext (Last 3)
            for(int ci = 8; ci < 11; ci++){
                if(dirEntry.RDE.FileName[ci] != 0x20){
                    returnVal.Extension[returnVal.ExtentionSize++] = dirEntry.RDE.FileName[ci];
                }
            }
        }  

        //File size swap
        returnVal.FileSize = dirEntry.RDE.FileSize;

        //Port
        returnVal.port = FileDrive;

        //Return
        return returnVal;
    }

    //Free used memory
    void File::FreeData(){
        //Check if empty already
        if(bufferPageCount == 0) return;

        //Free data and address
        GlobalAllocator.FreePages(data, bufferPageCount);

        //Clear counter
        bufferPageCount = 0;
    }   
    
    //
    //
    // READING DATA NEEDS TO BE WORKED ON
    // TRY TO INCLUDE OTHER CLUSTERS
    // RELY ON THE FAT TABLE OR
    // SOME FILES WONT READ ALL THE DATA !!!!
    //
    //

    void File::ReadData(){
        //If Full Stop
        if(bufferPageCount > 0) return;

        //Work out sizes
        bufferPageCount = FileSize / 4096 + 1;
        sectorCount = FileSize / 512 + 1;

        //Get memory space
        data = GlobalAllocator.RequestPages(bufferPageCount);

        //Check if there is any page before reading into it
        if(data != NULL){
            //Read data
            port->Read(DirEntry.FILE_FIRST_SEGMENT, sectorCount, data);
        }
    }
}