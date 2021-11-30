#ifndef __UNIFIED_OS_DRIVERS_INPUT_PS2MOUSEDRIVER_H
#define __UNIFIED_OS_DRIVERS_INPUT_PS2MOUSEDRIVER_H

#include <common/stdint.h>

#include <common/stdint.h>
#include <drivers/Driver.h>
#include <interrupts/interrupts.h>

namespace UnifiedOS{
    namespace Drivers{
        class PS2MouseEventHandler{ //Default event handler to be inherited from
        public:
            PS2MouseEventHandler();
            ~PS2MouseEventHandler();

            virtual void OnMouseDown(uint8_t button); //Events
            virtual void OnMouseUp(uint8_t button); //Events

            virtual void OnMouseMove(int xOff, int yOff); //Events
        };

        class PS2MouseDriver : public Interrupts::InterruptHandler, public Driver{
            //Ports for interraction
            IO::Port8Bit dataPort;
            IO::Port8Bit commandPort;

            //Buffers
            uint8_t buffer[3];
            uint8_t offset;
            uint8_t buttons;

            //Handler
            PS2MouseEventHandler* Handler;

            //Input Delays
            void Wait();
            void Wait_Input();

        public:
            //Constructors
            PS2MouseDriver(Interrupts::InterruptManager* manager, PS2MouseEventHandler* handler);
            ~PS2MouseDriver();

            //Interrupt
            virtual void HandleInterrupt(uint64_t rsp);

            //Activation
            virtual void Activate();
        };
    }
}

#endif