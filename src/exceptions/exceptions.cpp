#include <exceptions/exceptions.h>
#include <memory/heap.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Exceptions;
using namespace UnifiedOS::Interrupts;

ExceptionHandler::ExceptionHandler(uint8_t Exception, InterruptManager* manager)
    : InterruptHandler(Exception, manager) //Inti Handler
{
    interrupt = Exception; //For Panic
}
ExceptionHandler::~ExceptionHandler(){

}

void ExceptionHandler::HandleInterrupt(uint64_t rsp){
    Exception(interrupt); //Just Panic
    
    //Then Stop the computer
    while (true)
    {
        asm("hlt");
    }
}

ExceptionManager::ExceptionManager(InterruptManager* manager){
    //Intitalise Interrupts
    for(size_t i = 0; i <= 19; i++){
        Handlers[i] = new ExceptionHandler(i, manager);
    }
}