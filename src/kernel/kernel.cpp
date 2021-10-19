#include "kernelUtil.h"
#include "../filesystem/fileManager.h"
#include "../system/systemManager.h"

extern "C" void _start(BootInfo* bootInfo){
	//Subsytem
	KernelInfo kernelInfo = InitializeKernel(bootInfo);

	//Main Processes
	GSystemManager->Initliaze(bootInfo->framebuffer);

	//Do nothing (stop sudden resets)
    while(true){
		asm ("hlt");
	};
}