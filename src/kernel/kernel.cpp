#include "kernelUtil.h"
#include "../filesystem/fileManager.h"
#include "../system/systemManager.h"
#include "../filesystem/ByteInteraction.h"
#include "../CPU/cpuid.h"
#include "../interrupts/panic.h"

extern "C" void _start(BootInfo* bootInfo){
	//Subsytem
	KernelInfo kernelInfo = InitializeKernel(bootInfo);

	//Main Processes
	GSystemManager->Initliaze(bootInfo->framebuffer);

	//Locate Image File
	/*FILE::File f = GlobalFileManager.FindFile("B:/ErrorScreen.tga");

	TGA tga(&f);
	
	TGA_Image image = tga.GetImage();

	uint32_t* Buffer = image.Buffer;

	uint32_t col = 0x00;
	
	memcpy(bootInfo->framebuffer->BaseAddress, image.Buffer, image.header.width * image.header.height * 4);*/

	Executable e("B:/test.elf");
	e.DebugHeader();

	Function proc = e.GetEntry();

	int Output = proc();
	//GlobalRenderer->PrintAtCursor("Output: ");
	
	//int Output = 0;
	//GlobalRenderer->PrintAtCursor(to_string((uint64_t)Output));

	//Do nothing (stop sudden resets)
	//Implement Safe Shutowns
    while(true){
		asm ("hlt");
	};
}