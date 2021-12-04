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
        SMP::CPUs[i]->Queue = new Vector<Process*>; //Needed for some reason

        SMP::CPUs[i]->Queue->clear();
        releaseLock(&SMP::CPUs[i]->QueueLock);
    }

    //Create a process to continue the kernel process
    NewProcess("KernelStage2", entry, 0);

    //Current process to nullptr
    cpu->currentProcess = nullptr;
    
    //Start scheduling
    SchedulerReady = true;
    asm("sti; int $0xfd"); //IPI_SCHEDULE
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

    //If it has a active process
    if(cpu->currentProcess){
        //Increase it's ticks
        cpu->currentProcess->ActiveTicks++;
        
        //See how long it has left
        if(cpu->currentProcess->TimeSlice > 0){
            //If still needs to run then run untill time is 0
            cpu->currentProcess->TimeSlice--;
            return;
        }
    }

    //Ensure lock
    while (__builtin_expect(acquireTestLock(&cpu->QueueLock), 0)) {
        return;
    }

    //If process is dying
    if(__builtin_expect(cpu->currentProcess->State == PROCESS_DYING, 0)){
        //Remove the process from queue
        cpu->Queue->remove(cpu->currentProcess);
        
        //Load the idle process
        cpu->currentProcess = cpu->idleProcess;
    }

    //Check the size of the queue and if empte load idle process
    if(__builtin_expect(cpu->Queue->get_length() <= 0 || /*Or Try &&*/ !cpu->currentProcess, 0)){
        //If already on idle the ignore
        if(cpu->currentProcess == cpu->idleProcess){
            return;
        }

        //Load the idle process
        cpu->currentProcess = cpu->idleProcess;
    }
    else{ //Otherwise load from queue

        if(cpu->currentProcess){
            //Save current context (Registers)
            cpu->currentProcess->Context = *(ProcessContext*)rsp;

            //Save the fx_state (Issue here right now)
            // asm volatile("fxsave64 (%0)" :: "r"((uint64_t)&cpu->currentProcess->fx_State) : "memory");
        }

        //First come, first served basis
            if(cpu->currentProcess == cpu->Queue->get_at(0)){
                //Remove from the front
                cpu->Queue->erase(0);

                //Add to the back
                cpu->Queue->add_back(cpu->currentProcess);
            }

            //Load the front of the queue
            cpu->currentProcess = cpu->Queue->at(0);
        //
    }

    //Set it's tick time
    cpu->currentProcess->TimeSlice = cpu->currentProcess->DefaultTimeSlice;

    //Ensure rflag is set before swapping
    // cpu->currentProcess->Context.rflags |= 0x200;

    //Release the memory lock
    releaseLock(&cpu->QueueLock);

    //Load the FX_State
    // asm volatile("fxrstor64 (%0)" ::"r"((uint64_t)&cpu->currentProcess->fx_State) : "memory");

    //TSS
    GlobalDescriptorTable::SetKernelStack(&cpu->tss, (uint64_t)(cpu->currentProcess->Stack));

    //Swap the process
    ProcessSwitch(&(cpu->currentProcess->Context), (uint64_t)Paging::__PAGING__PTM_GLOBAL.PML4);

    // //Load the Page Table
    // asm("mov %0, %%cr3" : : "r" (Paging::__PAGING__PTM_GLOBAL.PML4));

    // ProcessSwitch((void*)(&cpu->currentProcess->GetContext()), (void*)(&Previous->GetContext()));
}

Processes::Process* Scheduler::NewProcess(const char* Name, uint64_t entry, uint64_t data){
    Process* p = new Process(NextPID++);

    Memory::memcpy(&p->Name, Name, 16);

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

    Memory::memcpy(&p->Name, Name, 16);

    p->IdleContextCreation((uint64_t)IdleProcess, 0);

    return p;
}

Scheduling::Scheduler* Scheduling::__SCHEDULER__ = nullptr;

void Scheduling::IntialiseScheduler(Interrupts::InterruptManager* im, uint64_t entry){
    __SCHEDULER__ = new Scheduler(im, entry);
}
