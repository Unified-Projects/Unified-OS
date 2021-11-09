#include <process/process.h>

#include <memory/heap.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Processes;
using namespace UnifiedOS::Memory;

Process::Process(uint64_t entrypoint){
    stack = (uint8_t*)malloc(4096);

    cpu = (CPUState*)(stack + 4096 - sizeof(CPUState));

    cpu->r15 = 0;
    cpu->r14 = 0;
    cpu->r13 = 0;
    cpu->r12 = 0;
    cpu->r11 = 0;
    cpu->r10 = 0;
    cpu->r9 = 0;
    cpu->r8 = 0;
    
    cpu->rbp = 0;
    cpu->rdi = 0;
    cpu->rsi = 0;
    cpu->rdx = 0;
    cpu->rcx = 0;
    cpu->rbx = 0;
    cpu->rax = 0;

    //Entry
    cpu->rip = entrypoint;

    cpu->cs = 0x08; //Usermode Needed //GDT Code sector
    cpu->rflags = 0x202;
    // cpu->rsp = (uint64_t)stack;
    // cpu->ss = 0; //Usermode Needed //SS Not implemented
}
Process::~Process(){

}

ProcessManager::ProcessManager(){
    processCount = 0;
    currentProcess = -1;
}
ProcessManager::~ProcessManager(){

}

bool ProcessManager::AddProcess(Process* process){
    if(processCount >= 256)
        return false;

    processes[processCount++] = process;
    return true;
}

CPUState* ProcessManager::Schedule(CPUState* cpu){
    if(processCount <= 0)
        return cpu;
    
    if(currentProcess >= 0)
        processes[currentProcess]->cpu = cpu;
    
    if(++currentProcess >= processCount)
        currentProcess %= processCount;
        
    return processes[currentProcess]->cpu;
}