#include <pointers.h>

using namespace UnifiedOS;

using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Exceptions;
using namespace UnifiedOS::Interrupts::Syscalls;

using namespace UnifiedOS::Drivers;

using namespace UnifiedOS::Devices;

//Interrupts (Default)
InterruptManager* Pointers::Interrupts::Interrupts = nullptr;

//Syscalls
SyscallHandler* Pointers::Interrupts::Syscalls::Syscalls = nullptr;

//Exceptions
ExceptionManager* Pointers::Exceptions::Exceptions = nullptr;

//Drivers
DriverManager* Pointers::Drivers::DriverManager = nullptr;

//Devices
DeviceManager* Pointers::Devices::DeviceManager = nullptr;

IO::APIC::SpuriousInterrupHandler* Pointers::APIC::SpuriousInterupts = nullptr;