#pragma once
#include <stdint.h>

#include "../IO/IO.h"
#include "Spinlock.h"

#define THREAD_TIMESLICE_DEFAULT 10

enum {
	ThreadStateRunning, //Thread is running
	ThreadStateBlocked, //Thread is blocked, do not schedule
	ThreadStateZombie, //Waiting on thread to close a resource/exit a syscall
	ThreadStateDying, //Thread is actively being killed
};

class Process;
struct Thread;

class ThreadBlocker{
	friend struct Thread;
protected:
	lock_t lock = 0;
	Thread* thread = nullptr;

	bool shouldBlock = true; // If Unblock() is called before the thread is blocked or the lock is acquired then tell the thread not to block
	bool interrupted = false; // Returned by Block so the thread knows it has been interrupted
	bool removed = false; // Has the blocker been removed from queue(s)?
public:
	virtual ~ThreadBlocker() = default;

	virtual void Interrupt(); // A blocker may get interrupted because a thread is getting killed. 
	virtual void Unblock();

	inline bool ShouldBlock() { return shouldBlock; }
	inline bool WasInterrupted() { return interrupted; } 
};

class GenericThreadBlocker : public ThreadBlocker{
public:
	inline void Interrupt() {}	
};

using FutexThreadBlocker = GenericThreadBlocker;
 
struct Thread {
    void* stack = nullptr; //Pointer to the initial stack
	void* stackLimit = nullptr; //The limit of the stack
	void* kernelStack = nullptr; //Kernel Stack
	uint32_t timeSlice = THREAD_TIMESLICE_DEFAULT;
	uint32_t timeSliceDefault = THREAD_TIMESLICE_DEFAULT;
	RegisterContext registers;  //Registers
	RegisterContext lastSyscall; //Last system call
	void* fxState; //State of the extended registers

    Thread* Next = nullptr;
    Thread* Prev = nullptr;

    Process* parent; //Parent Process
    uint64_t TID = 1;
    
    uint8_t priority = 0; //Thread priority
	uint8_t state = ThreadStateRunning; //Thread state

    uint64_t fsBase = 0;

	bool blockTimedOut = false;
	ThreadBlocker* blocker = nullptr;

	uint64_t pendingSignals = 0; //Bitmap of pending signals
	uint64_t signalMask = 0; //Masked signals

    Thread(class Process* _parent, uint64_t _tid);

    void Signal(int signal);
	void HandlePendingSignal(RegisterContext* regs);

    bool Block(ThreadBlocker* blocker);
    bool Block(ThreadBlocker* blocker, long& usTimeout);

    void Unblock();

    void Sleep(uint64_t MicroSeconds);
};