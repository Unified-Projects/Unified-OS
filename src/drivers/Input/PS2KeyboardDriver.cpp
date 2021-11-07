#include <drivers/Input/PS2KeyboardDriver.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Drivers;

PS2KeyboardEventHandler::PS2KeyboardEventHandler(){

}
PS2KeyboardEventHandler::~PS2KeyboardEventHandler(){

}

void PS2KeyboardEventHandler::OnKeyDown(char c){

}
void PS2KeyboardEventHandler::OnKeyUp(char c){

}

PS2KeyboardDriver::PS2KeyboardDriver(Interrupts::InterruptManager* manager, PS2KeyboardEventHandler* handler)
    : InterruptHandler(0x21, manager), //Setup interrupt
      dataPort(0x60) //Set up port
{
    Handler = handler; //Store handler pointer
}
PS2KeyboardDriver::~PS2KeyboardDriver(){
    
}

uint64_t PS2KeyboardDriver::HandleInterrupt(uint64_t rsp){
    uint8_t key = dataPort.Read(); //Read the scancode

    if(Handler == 0){ //If there is no handler end the interrupt
        return rsp;
    }

    //Send the key to the handler
    Handler->OnKeyDown(key);

    return rsp;
}
void PS2KeyboardDriver::Activate(){
    
}