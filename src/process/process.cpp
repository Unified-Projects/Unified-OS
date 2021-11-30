#include <process/process.h>

#include <memory/heap.h>
#include <paging/PageFrameAllocator.h>
#include <memory/memory.h>

#include <process/Scheduler/Scheduler.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Processes;
using namespace UnifiedOS::Memory;

const uint16_t kKernelCS = 0x08;
const uint16_t kKernelSS = 0x10;

Process* Process::ContextCreation(uint64_t entry, int64_t data){
    //Request a large stack
    const size_t StackSize = DefaultStackBytes * 32;
    Stack = (uint8_t*)Paging::__PAGING__PFA_GLOBAL.RequestPages(32);

    //Set it to zero
    memset(Stack, 0, DefaultStackBytes * 32);

    //We offset the stack
    uint64_t StackEnd = reinterpret_cast<uint64_t>(Stack) + StackSize;

    //Setup Registers
    memset(&Context, 0, sizeof(Context));

    //Configure flags and sectors
    Context.rflags = 0x202;
    Context.cs = kKernelCS;
    Context.ss = kKernelSS;

    //Stack pointer
    Context.rsp = StackEnd;
    Context.rbp = StackEnd;

    //Load the actuall process
    Context.rip = entry;
    Context.rdi = PID;
    Context.rsi = data;

    //Timing
    ActiveTicks = 0;
    TimeSlice = DefaultTimeSlice;

    //FXState
    fx_State->mxcsr = 0x1f80; // Default MXCSR (SSE Control Word) State
    fx_State->mxcsrMask = 0xffbf;
    fx_State->fcw = 0x33f; // Default FPU Control Word State

    return this;
}

Process* Process::IdleContextCreation(uint64_t entry, int64_t data){
    //Request a small stack as the task is a idle task
    const size_t StackSize = DefaultStackBytes;
    Stack = (uint8_t*)Paging::__PAGING__PFA_GLOBAL.RequestPage();

    //Set it to zero
    memset(Stack, 0, DefaultStackBytes);

    //We offset the stack
    uint64_t StackEnd = reinterpret_cast<uint64_t>(Stack) + StackSize;

    //Setup Registers
    memset(&Context, 0, sizeof(Context));

    //Configure flags and sectors
    Context.rflags = 0x202;
    Context.cs = kKernelCS;
    Context.ss = kKernelSS;

    //Stack pointer
    Context.rsp = StackEnd;
    Context.rbp = StackEnd;

    //Load the actuall process
    Context.rip = entry;
    Context.rdi = PID;
    Context.rsi = data;

    //Timing
    ActiveTicks = 0;
    TimeSlice = DefaultTimeSlice;

    //FXState
    fx_State->mxcsr = 0x1f80; // Default MXCSR (SSE Control Word) State
    fx_State->mxcsrMask = 0xffbf;
    fx_State->fcw = 0x33f; // Default FPU Control Word State

    return this;
}

uint64_t Process::GetPID() const{
    return PID;
}

Process::Process(uint64_t pid){
    PID = pid;
    State = PROCESS_READY;
}
Process::~Process(){

}