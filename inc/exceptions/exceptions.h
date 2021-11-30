#ifndef __UNIFIED_OS_EXCEPTIONS_EXCEPTIONS_H
#define __UNIFIED_OS_EXCEPTIONS_EXCEPTIONS_H

#include <common/stdint.h>
#include <exceptions/panic.h>
#include <interrupts/interrupts.h>

namespace UnifiedOS{
    namespace Exceptions{
        //Hanlder (Intterupt for exceptions)
        class ExceptionHandler : public Interrupts::InterruptHandler{
            uint8_t interrupt;
        public:
            ExceptionHandler(uint8_t Exception, Interrupts::InterruptManager* manager);
            ~ExceptionHandler();

            virtual void HandleInterrupt(uint64_t rsp);
        };

        //For intitlisation and can implement custom actions towards exceptions
        class ExceptionManager{
        protected:
            ExceptionHandler* Handlers[19]; //Handlers

        public:
            ExceptionManager(Interrupts::InterruptManager* manager); //Constructors
        };
    }
}

#endif