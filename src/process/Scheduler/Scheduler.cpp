#include <process/Scheduler/Scheduler.h>
#include <memory/heap.h>
#include <IO/APIC/apic.h>
#include <IO/CPU/CPU.h>
#include <smp/smp.h>
#include <common/cstring.h>
#include <IO/spinlock.h>
#include <gdt/tss.h>

#include <paging/PageTableManager.h>

#include <common/stdio.h>
#include <common/cstring.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Scheduling;
using namespace UnifiedOS::Processes;

bool SchedulerReady = false;

//Scheduler Vars
    uint64_t Scheduler::NextPID = 0;
//

void IdleProcess() {
    while (true)
    {
        //IDLE
    }
}

Scheduler::Scheduler(Interrupts::InterruptManager* im, uint64_t entry)
    : Interrupts::InterruptHandler(IPI_SCHEDULE, im) //Interrupt
{
    //Current CPU
    UnifiedOS::CPU::CPU* cpu = CPU::GetCPULocal();

    //Create Idle processes
    for(unsigned i = 0; i < SMP::ActiveCPUs; i++){
        //Create a process name (With ID)
        char* ProcName = "IdleCPU00";
        ProcName[8] = to_string((int64_t)i)[0];
        ProcName[9] = to_string((int64_t)i)[1];

        //Create a idle Process with the name
        SMP::CPUs[i]->idleProcess = CreateIdleProcess(ProcName);
    }

    //Clear the queue
    for(unsigned i = 0; i < SMP::ActiveCPUs; i++){
        // acquireLock(&SMP::CPUs[i]->QueueLock);
        //Clear the queue with force
        // SMP::CPUs[i]->Queue = new Vector<Process*>; //Needed for some reason

        SMP::CPUs[i]->Queue->clear();
        releaseLock(&SMP::CPUs[i]->QueueLock);
    }

    //Create a process to continue the kernel process
    NewProcess("KernelStage2", entry, 0);

    //Current process to nullptr
    cpu->currentProcess = nullptr;
    
    //Start scheduling
    SchedulerReady = true;
    asm("int $0xfd"); //IPI_SCHEDULE
}

void Scheduler::Tick(uint64_t rsp){
    //Only run once intialised (Because it's run on each PIT tick, this may be called before intialisation)
    if(!SchedulerReady)
        return;

    //Send IPI to other processes
    IO::APIC::SendIPI(0, ICR_DSH_OTHER, ICR_MESSAGE_TYPE_FIXED, IPI_SCHEDULE);

    //THIS LINE CAUSES THE PAGE FAULT ERROR !
    Schedule(rsp);
}

//This is run by both tick and interrupts
//The Scheduling process
void Scheduler::Schedule(uint64_t rsp){
    //Work out the cpu
    UnifiedOS::CPU::CPU* cpu = CPU::GetCPULocal();

    //Check if a current process is running
    if(cpu->currentProcess){
        //Increase it's Tick counter
        cpu->currentProcess->ActiveTicks++;

        //If it still needs to run, let it continue
        if(cpu->currentProcess->TimeSlice > 0){
            cpu->currentProcess->TimeSlice--;
            return;
        }
    }

    //Ensure lock without stalling
    while (__builtin_expect(acquireTestLock(&cpu->QueueLock), 0)) {
        return;
    }
    
    //Idle the cpu if nothing is available
    if(!cpu->currentProcess || cpu->Queue->size() <= 0){
        cpu->currentProcess = cpu->idleProcess;
    }
    else{
        //Kill process if dying
        if(cpu->currentProcess->State == PROCESS_DYING){
            cpu->Queue->remove(cpu->currentProcess);
            cpu->currentProcess = cpu->idleProcess;
        }
        else{ //Swap process to next process
            if(cpu->currentProcess != cpu->idleProcess){
                cpu->currentProcess->TimeSlice = cpu->currentProcess->DefaultTimeSlice;

                //Save the FXState
                asm volatile("fxsave64 (%0)" :: "r"((uint64_t)cpu->currentProcess->fx_State) : "memory");

                //Save the stack
                cpu->currentProcess->Context = *((ProcessContext*)rsp);
                
                //Add process to the back
                cpu->Queue->erase(0);
                cpu->Queue->add_back(cpu->currentProcess);
            }

            //Swap process
            cpu->currentProcess = cpu->Queue->get_at(0);
            cpu->currentProcess->TimeSlice = cpu->currentProcess->DefaultTimeSlice;
        }
    }

    //Lock Remover
    releaseLock(&cpu->QueueLock);

    //FX state restoring
    asm volatile("fxrstor64 (%0)" ::"r"((uint64_t)cpu->currentProcess->fx_State) : "memory");

    //Used in user space
    asm volatile("wrmsr" ::"a"(cpu->currentProcess->fsBase & 0xFFFFFFFF) /*Value low*/,
                 "d"((cpu->currentProcess->fsBase >> 32) & 0xFFFFFFFF) /*Value high*/, "c"(0xC0000100) /*Set FS Base*/);

    //TSS
    GlobalDescriptorTable::SetKernelStack(&cpu->tss, (uint64_t)(cpu->currentProcess->Stack));

    //Swap the process (Paging causes errors when being swapped)
    ProcessSwitch(&(cpu->currentProcess->Context), (uint64_t)(Paging::__PAGING__PTM_GLOBAL.PML4));
}

Processes::Process* Scheduler::NewProcess(const char* Name, uint64_t entry, uint64_t data){
    Process* p = new Process(NextPID++);

    Memory::memcpy(&p->Name, Name, strlen(Name));

    p->ContextCreation((uint64_t)entry, data);

    UnifiedOS::CPU::CPU* cpu = SMP::CPUs[0];
    for (unsigned i = 1; i < SMP::ActiveCPUs; i++) {
        if (SMP::CPUs[i]->Queue->get_length() < cpu->Queue->get_length()) {
            cpu = SMP::CPUs[i];
        }

        if (!cpu->Queue->get_length()) { //If len is 0
            break;
        }
    }
    
    acquireLock(&cpu->QueueLock);
    asm("cli");
    cpu->Queue->add_back(p);
    releaseLock(&cpu->QueueLock);
    asm("sti");

    return p;
}

void Scheduler::HandleInterrupt(uint64_t rsp){
    //Call the scheduler
    Schedule(rsp);
}

Process* Scheduler::CreateIdleProcess(const char* Name){
    Process* p = new Process(NextPID++);

    Memory::memcpy(&p->Name, Name, strlen(Name));

    p->IdleContextCreation((uint64_t)IdleProcess, 0);

    return p;
}

Scheduling::Scheduler* Scheduling::__SCHEDULER__ = nullptr;

void Scheduling::IntialiseScheduler(Interrupts::InterruptManager* im, uint64_t entry){
    __SCHEDULER__ = new Scheduler(im, entry);
}
