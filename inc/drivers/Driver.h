#ifndef __UNIFIED_OS_DRIVERS_DRIVER_H
#define __UNIFIED_OS_DRIVERS_DRIVER_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace Drivers{
        class Driver //Default driver class
        {
        public:
            Driver();
            ~Driver();

            const char* DriverName = "Default Driver";

            void* MainObject = nullptr;
            
            virtual void Activate(); //Some require some code on start
            virtual int Reset();
            virtual void Deactivate();
        };

        class DriverManager
        {
        private:
            Driver* drivers[255]; //Maximum Driver Count
            int numDrivers; //Count of current drivers
            
        public:
            DriverManager();
            ~DriverManager();
            void AddDriver(Driver* driver); //Adds a driver to the stack

            Driver* FindDriver(const char* Name);
            
            void ActivateAll(); //Activates all the drivers
        };
    }
}

#endif