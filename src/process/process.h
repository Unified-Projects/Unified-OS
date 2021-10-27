#pragma once
#include <stdint.h>

#include "thread.h"
#include "../scheduling/pit/pit.h"

class Process{
private:
    uint64_t PID;

    Process* Parent;
    Process* Children;

    Thread* MainThread;
    Thread* Threads;

    Thread* ActiveThread;

public:
    void Block();
    void Unblock();

    void Sleep(uint64_t MicroSeconds);

private:
public:

};