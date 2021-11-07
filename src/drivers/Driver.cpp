#include <drivers/Driver.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Drivers;

Driver::Driver()
{
}

Driver::~Driver()
{
}
        
void Driver::Activate()
{
}

int Driver::Reset()
{
    return 0;
}

void Driver::Deactivate()
{
}

DriverManager::DriverManager()
{
    numDrivers = 0;
}
DriverManager::~DriverManager()
{
    
}

//Loads a driver onto the stack
void DriverManager::AddDriver(Driver* driver)
{
    drivers[numDrivers] = driver;
    numDrivers++; 
}

//Runs Driver.Activate() on all drivers in use
void DriverManager::ActivateAll()
{
    for(int i = 0; i < numDrivers; i++){
        if(drivers[i] != 0)
            drivers[i]->Activate();
    }
}