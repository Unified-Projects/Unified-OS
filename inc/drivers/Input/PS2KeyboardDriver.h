#ifndef __UNIFIED_OS_DRIVERS_INPUT_PS2KEYBOARDDRIVER_H
#define __UNIFIED_OS_DRIVERS_INPUT_PS2KEYBOARDDRIVER_H

#include <common/stdint.h>
#include <drivers/Driver.h>
#include <interrupts/interrupts.h>

namespace UnifiedOS{
    namespace Drivers{
        class PS2KeyboardEventHandler{ //Used as a default handler for things like desktop to inherit from
        public:
            PS2KeyboardEventHandler();
            ~PS2KeyboardEventHandler();

            virtual void OnKeyDown(char c); //Events
            virtual void OnKeyUp(char c); //Events
        };

        class PS2KeyboardDriver : public Interrupts::InterruptHandler, public Drivers::Driver{
            //Ports
            IO::Port8Bit dataPort;
            IO::Port8Bit commandPort;
            
            //The event handler
            PS2KeyboardEventHandler* Handler;
        public:
            //Startup the driver and interrupt handler
            PS2KeyboardDriver(Interrupts::InterruptManager* manager, PS2KeyboardEventHandler* handler);
            ~PS2KeyboardDriver();

            //Handler
            virtual void HandleInterrupt(uint64_t rsp);

            //Actiavtor
            virtual void Activate();
        };
    }
}

#endif