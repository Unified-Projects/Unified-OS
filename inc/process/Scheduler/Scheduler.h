#ifndef __UNIFIED_OS_PROCESS_SCHEDULER_H
#define __UNIFIED_OS_PROCESS_SCHEDULER_H

#include <common/stdint.h>

#include <process/process.h>
#include <common/vector.h>

#include <interrupts/interrupts.h>

#include <paging/paging.h>

namespace UnifiedOS{
    namespace Scheduling{
        //Scheduler
        class Scheduler : public Interrupts::InterruptHandler{
        private:
            //PID
            static uint64_t NextPID;
        public:
            //Setup
            Scheduler(Interrupts::InterruptManager* im, uint64_t entry);

            //To be migrated and re sorted to work with the new system
            // void Sleep(Processes::Process* process);
            // int Sleep(uint64_t pid);
            // void Wakeup(Processes::Process* process, int level = -1);
            // int Wakeup(uint64_t pid, int level = -1);

            // void Kill(Processes::Process* process);
            // int Kill(uint64_t pid);

        private:
            //Process
            //NOTE: In the add process add to the AllActive for PID searching
            Vector<Processes::Process*> Dead;
            Vector<Processes::Process*> Sleeping;
            Vector<Processes::Process*> AllActive;

        public:
            //Creation
            Processes::Process* CreateIdleProcess(const char* Name);
            Processes::Process* NewProcess(const char* Name, uint64_t entry, uint64_t data);

        public:
            //Add a already made process
            void AddProcessToQueue(Processes::Process* process);

        public:
            //Actuall Schedule
            void Schedule(uint64_t rsp);

            //Interrupts
            void Tick(uint64_t rsp);
            void HandleInterrupt(uint64_t rsp);
        };

        //Global
        extern Scheduler* __SCHEDULER__;

        //Setup
        void IntialiseScheduler(Interrupts::InterruptManager* im, uint64_t entry);
    }
}

//Task changing
extern "C" void ProcessSwitch(UnifiedOS::Processes::ProcessContext* context, uint64_t pml4);

#endif
