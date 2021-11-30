#ifndef __UNIFIED_OS_GLOBAL_POINTERS_H
#define __UNIFIED_OS_GLOBAL_POINTERS_H

#include <IO/APIC/apic.h>

#include <interrupts/interrupts.h>
#include <exceptions/exceptions.h>
#include <interrupts/syscalls.h>

#include <drivers/Driver.h>

#include <IO/DeviceManager/DeviceManager.h>

namespace UnifiedOS{
    namespace Pointers{
        namespace Interrupts{
            
            //Interrupts (Default)
            extern UnifiedOS::Interrupts::InterruptManager* Interrupts;
        }

        namespace Exceptions{
            //Exception Handler
            extern UnifiedOS::Exceptions::ExceptionManager* Exceptions;
        }

        namespace Interrupts::Syscalls{
            //Syscall Handler
            extern UnifiedOS::Interrupts::Syscalls::SyscallHandler* Syscalls;
        }

        namespace Drivers{
            //Driver Manager
            extern UnifiedOS::Drivers::DriverManager* DriverManager;
        }

        namespace Devices{
            //Device Manager
            extern UnifiedOS::Devices::DeviceManager* DeviceManager;
        }

        namespace APIC{
            //Spurious Interrupt Handler
            extern UnifiedOS::IO::APIC::SpuriousInterrupHandler* SpuriousInterupts;
        }
    }
}

#endif