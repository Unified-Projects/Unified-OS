#ifndef __UNIFIED_OS_PROCESS_SCHEDULER_H
#define __UNIFIED_OS_PROCESS_SCHEDULER_H

#include <common/stdint.h>

#include <process/process.h>
#include <common/vector.h>

#define DEFAUL_PROCESS_TICK_COUNT 10

namespace UnifiedOS{
    namespace Scheduling{
        struct ProcessLevel{
            Processes::Process* processes[256]; //Make dynamic
            Processes::Process* sleeping[256]; //Make Dynamic

            Vector<Processes::Process*> Awake;
            Vector<Processes::Process*> Asleep;

            int64_t NextPID = -1;
            int64_t CurrentPID = -1;

            uint64_t AvialablePID1 = 0x00;
            uint64_t AvialablePID2 = 0x00;
            uint64_t AvialablePID3 = 0x00;
            uint64_t AvialablePID4 = 0x00;

            int64_t GetNextPID();
            int64_t FreePID(uint64_t PID);
        };

        class Scheduler{
        protected:
            uint64_t TimerTick; //Ticking

        public:
            // level: 0 = lowest, kMaxLevel = highest
            static const int kMaxLevel = 3;

            Scheduler();

            Processes::Process& NewProcess(int level);

            void SwitchProcess(bool CurrentSleep = false);
            void SwitchProcessF();

            void Sleep(Processes::Process* process);
            int Sleep(uint64_t pid);
            void Wakeup(Processes::Process* process, int level = -1);
            int Wakeup(uint64_t pid, int level = -1);

            void Kill(Processes::Process* process);
            int Kill(uint64_t pid);

            Processes::Process& CurrentProcess();

        private:
            ProcessLevel ProcessLevels[4];
            Processes::Process* ActiveProcess;

            int CurrentLevel{kMaxLevel};
            bool LevelChanged{false};

            void ChangeLevelRunning(Processes::Process* process, int level);

        public:
            void Tick();
        };

        extern Scheduler* __SCHEDULER__;

        void InitializeProcess();
    }
}

extern "C" void ProcessSwitch(void* NextContext, void* CurrentContext);

#endif
