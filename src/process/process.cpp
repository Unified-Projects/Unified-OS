#include <process/process.h>

#include <memory/heap.h>
#include <memory/memory.h>

#include <process/Scheduler/Scheduler.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Processes;
using namespace UnifiedOS::Memory;

const uint16_t kKernelCS = 1 << 3;
const uint16_t kKernelSS = 2 << 3;
const uint16_t kKernelDS = 0;

Process& Process::ContextCreation(uint64_t entry, int64_t data){
    const size_t StackSize = DefaultStackBytes;
    Stack = new uint8_t[StackSize];
    uint64_t StackEnd = reinterpret_cast<uint64_t>(Stack) + StackSize;

    memset(&Context, 0, sizeof(Context));

    asm volatile("mov %%cr3, %0" : "=r" (Context.cr3));
    Context.rflags = 0x202;
    Context.cs = kKernelCS;
    Context.ss = kKernelSS;
    Context.rsp = (StackEnd & ~0xflu) - 8;

    Context.rip = entry;
    Context.rdi = PID;
    Context.rsi = data;

    // Mask all exceptions of MXCSR.
    *reinterpret_cast<uint32_t*>(&Context.fxsave_area[24]) = 0x1f80;

    return *this;
}

ProcessContext& Process::GetContext(){
    return Context;
}
uint64_t Process::GetPID() const{
    return PID;
}
Process& Process::Sleep(){
    Scheduling::__SCHEDULER__->Sleep(this);
    return *this;
}
Process& Process::Wakeup(){
    Scheduling::__SCHEDULER__->Wakeup(this);
    return *this;
}

Process::Process(uint64_t pid){
    PID = pid;
}
Process::~Process(){

}