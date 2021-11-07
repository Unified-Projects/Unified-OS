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
      dataPort(0x60), //Set up ports
      commandPort(0x64)
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
    while (commandPort.Read() & 0x01)
    {
        dataPort.Read();
    }

    commandPort.Write(0xAE); //PIC start sending interrupts
    commandPort.Write(0x20); //Get state

    uint8_t status = (dataPort.Read() | 1) & ~0x10;
    commandPort.Write(0x60); //Set State
    dataPort.Write(status);

    dataPort.Write(0xF4); //Activate
}