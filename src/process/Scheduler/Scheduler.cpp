#include <process/Scheduler/Scheduler.h>
#include <memory/heap.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Scheduling;
using namespace UnifiedOS::Processes;

//Notes:
//  Implement a vectoring system for all the processes
//  Create a vector for each process layer to be able to add processes and remove seamlessly
//  Create a vector for all tasks to allow Kill, Sleep and Wakup to be able to rely on tasks using a PID over all layers
// 
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#include <common/stdio.h>
#include <common/cstring.h>

void IdleProcess() {
    while (true) __asm__("hlt");
}

int64_t ProcessLevel::GetNextPID(){
    //Check the first PID section
    for(int i = 0; i < 64; i++){
        //Check if not locked
        if(!((AvialablePID1 >> i) & 1U)){
            AvialablePID1 ^= (-1 ^ AvialablePID1) & (1UL << i);
            return i;
        }
    }

    for(int i = 0; i < 64; i++){
        //Check if not locked
        if(!((AvialablePID2 >> i) & 1U)){
            AvialablePID2 ^= (-1 ^ AvialablePID2) & (1UL << i);
            return i + 64;
        }
    }

    for(int i = 0; i < 64; i++){
        //Check if not locked
        if(!((AvialablePID3 >> i) & 1U)){
            AvialablePID3 ^= (-1 ^ AvialablePID3) & (1UL << i);
            return i + 128;
        }
    }

    for(int i = 0; i < 64; i++){
        //Check if not locked
        if(!((AvialablePID4 >> i) & 1U)){
            AvialablePID4 ^= (-1 ^ AvialablePID4) & (1UL << i);
            return i + 192;
        }
    }

    return -1;
}

int64_t ProcessLevel::FreePID(uint64_t PID){
    if(PID < 64){
        AvialablePID1 ^= (1 << (PID % 64));
        return 1;
    }
    else if(PID < 128){
        AvialablePID2 ^= (1 << (PID % 64));
        return 1;
    }
    else if(PID < 192){
        AvialablePID3 ^= (1 << (PID % 64));
        return 1;
    }
    else if(PID < 256){
        AvialablePID4 ^= (1 << (PID % 64));
        return 1;
    }
    return -1;
}

Scheduler::Scheduler(){
    for(int i = 0; i <= 3; i++){
        for(int u = 0; u < 256; u++){
            ProcessLevels[i].processes[u] = nullptr;
            ProcessLevels[i].sleeping[u] = nullptr;
        }
    }

    //Idle
    //We create idle first for PID 0
    Process& idle = NewProcess(0).ContextCreation((uint64_t)IdleProcess, 0).SetLevel(0).SetRunning(true);
    ProcessLevels[0].CurrentPID = idle.GetPID();

    //Move current to the stack
    Process& MainProcess = NewProcess(CurrentLevel).SetLevel(CurrentLevel).SetRunning(true);
    ProcessLevels[CurrentLevel].CurrentPID = MainProcess.GetPID();

    //Set current
    ActiveProcess = &MainProcess;
}

Process& Scheduler::NewProcess(int level){
    if(level >= 0 && level <= kMaxLevel){
        uint64_t PID = ProcessLevels[level].GetNextPID();

        if(PID == -1){
            return *((Process*)nullptr);
        }

        Process* Proc = new Process(PID);

        ProcessLevels[level].processes[PID] = Proc;

        return *Proc;
    }

    return *((Process*)nullptr);
}

void Scheduler::SwitchProcess(bool CurrentSleep = false){
    if(CurrentSleep){
        ProcessLevels[CurrentLevel].processes[ActiveProcess->GetPID()] = nullptr;
        ProcessLevels[CurrentLevel].sleeping[ActiveProcess->GetPID()] = ActiveProcess;

        //Find next PID
        uint64_t NextPID = ActiveProcess->GetPID() + 1;
        uint64_t Count = 0;
        for(; Count < 256; Count++, NextPID++){
            NextPID %= 256;

            if(ProcessLevels[CurrentLevel].processes[NextPID] != nullptr){
                ProcessLevels[CurrentLevel].NextPID = NextPID;

                break;
            }
        }

        if(NextPID == ActiveProcess->GetPID()){
            LevelChanged = true;
            ProcessLevels[CurrentLevel].NextPID = -1;
        }
    }

    if(LevelChanged){
        LevelChanged = false;
        for(int level = kMaxLevel; level >= 0; --level){
            if(ProcessLevels[level].NextPID != -1){
                CurrentLevel = level;
                break;
            }
        }
    }

    Process* PrevProcess = ActiveProcess; 
    ActiveProcess = ProcessLevels[CurrentLevel].processes[ProcessLevels[CurrentLevel].NextPID];

    ProcessLevels[CurrentLevel].CurrentPID = ProcessLevels[CurrentLevel].NextPID;    

    ProcessSwitch((void*)(&ActiveProcess->GetContext()), (void*)(&PrevProcess->GetContext()));
}

void Scheduler::SwitchProcessF(){
    //Find next PID
    uint64_t NextPID = ActiveProcess->GetPID() + 1;
    uint64_t Count = 0;
    for(; Count < 256; Count++, NextPID++){
        NextPID %= 256;

        if(ProcessLevels[CurrentLevel].processes[NextPID] != nullptr){
            ProcessLevels[CurrentLevel].NextPID = NextPID;
            break;
        }
    }

    if(NextPID == ActiveProcess->GetPID()){
        LevelChanged = true;
        ProcessLevels[CurrentLevel].NextPID = -1;
    }

    bool Found = false;

    if(LevelChanged){
        LevelChanged = false;
        for(int level = kMaxLevel; level >= 0; --level){
            if(ProcessLevels[level].NextPID != -1){
                CurrentLevel = level;
                Found = true;
                break;
            }
        }
    }

    if(Found || !LevelChanged){
        Process* PrevProcess = ActiveProcess; 
        ActiveProcess = ProcessLevels[CurrentLevel].processes[ProcessLevels[CurrentLevel].NextPID];

        ProcessLevels[CurrentLevel].CurrentPID = ProcessLevels[CurrentLevel].NextPID;    

        ProcessSwitch((void*)(&ActiveProcess->GetContext()), (void*)(&PrevProcess->GetContext()));
    }
}

void Scheduler::Sleep(Process* process){
    if(process == nullptr) return;

    if (!process->GetRunning()) {
        return;
    }

    process->SetRunning(false);

    if (process == ProcessLevels[CurrentLevel].processes[ProcessLevels[CurrentLevel].CurrentPID]) {
        SwitchProcess(true);
        return;
    }
}
int Scheduler::Sleep(uint64_t pid){
    if(pid > 255){
        return -1;
    }
    Sleep(ProcessLevels[CurrentLevel].processes[pid]);
    return 1;
}
void Scheduler::Wakeup(Process* process, int level = -1){
    if(process == nullptr) return;

    if (process->GetRunning()) {
        ChangeLevelRunning(process, level);
        return;
    }

    if (level < 0) {
        level = process->GetLevel();
    }

    process->SetLevel(level);
    process->SetRunning(true);

    //POTENTIAL ISSUE WITH NO PID AVAILABLE!!!!!!!
    ProcessLevels[level].processes[process->PID] = process;

    if (level > CurrentLevel) {
        LevelChanged = true;
    }
    return;
}
int Scheduler::Wakeup(uint64_t pid, int level = -1){
    if(pid > 255){
        return -1;
    }
    Wakeup(ProcessLevels[CurrentLevel].processes[ProcessLevels[CurrentLevel].CurrentPID], level);
    return 1;
}

void Scheduler::Kill(Processes::Process* process){
    if(process == nullptr) return;

    for(int i = 0; i < kMaxLevel; i++){
        if(ProcessLevels[i].processes[process->GetPID()] == process){
            ProcessLevels[i].processes[process->GetPID()] = nullptr;
            ProcessLevels[i].FreePID(process->GetPID());
            break;
        }
        else if (ProcessLevels[i].sleeping[process->GetPID()] == process){
            ProcessLevels[i].sleeping[process->GetPID()] = nullptr;
            ProcessLevels[i].FreePID(process->GetPID());
            break;
        }
    }

    // delete process;
}
int Scheduler::Kill(uint64_t pid){
    if(pid > 255){
        return -1;
    }
    Kill(ProcessLevels[CurrentLevel].processes[pid]);
}

Process& Scheduler::CurrentProcess(){
    return *ActiveProcess;
}

void Scheduler::ChangeLevelRunning(Process* process, int level){
    //Ingore untill Vectors
}

void Scheduler::Tick(){
    TimerTick++;
    if(TimerTick % DEFAUL_PROCESS_TICK_COUNT == 0){
        SwitchProcessF();
        TimerTick = 0;
    }
}

Scheduler* Scheduling::__SCHEDULER__;

void Scheduling::InitializeProcess(){
    __SCHEDULER__ = new Scheduler;
}