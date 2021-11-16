#include <IO/DeviceManager/ACPI/acpi.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Devices;
using namespace UnifiedOS::Devices::ACPI;

void* ACPI::FindTable(SDTHeader* sdtHeader, char* signature){
    //Calculate entries count
    // int entries = (sdtHeader->Length - sizeof(ACPI::SDTHeader)) / sizeof(uint64_t);
    int entries = 0;
    if (sdtHeader->Revision == 2) {
        entries = (sdtHeader->Length - sizeof(SDTHeader)) / sizeof(uint64_t); // ACPI 2.0
    } else {
        entries = (sdtHeader->Length - sizeof(SDTHeader)) / sizeof(uint32_t); // ACPI 1.0
    }
    
    //Loop over entries
    for (int t = 0; t < entries; t++){
        //Get header at position
        ACPI::SDTHeader* newSDTHeader = (ACPI::SDTHeader*)*(uint64_t*)((uint64_t)sdtHeader + sizeof(ACPI::SDTHeader) + (t * 8));
        
        //Found
        bool found = true;

        //Check loop over signatures check if its identical
        for (int i = 0; i < 4; i++){
            if (newSDTHeader->Signature[i] != signature[i]){
                found = false;
                break;
            }
            // if (t == 3) return newSDTHeader;
        } 

        if(found)
            return newSDTHeader;
    }

    // No SDT found
    return NULL;
}