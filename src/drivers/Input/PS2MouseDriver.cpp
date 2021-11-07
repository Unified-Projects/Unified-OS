#include <drivers/Input/PS2MouseDriver.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Drivers;

PS2MouseEventHandler::PS2MouseEventHandler(){

}
PS2MouseEventHandler::~PS2MouseEventHandler(){

}

void PS2MouseEventHandler::OnMouseDown(uint8_t button){

}
void PS2MouseEventHandler::OnMouseUp(uint8_t button){

}
void PS2MouseEventHandler::OnMouseMove(int x, int y){

}

PS2MouseDriver::PS2MouseDriver(Interrupts::InterruptManager* manager, PS2MouseEventHandler* handler)
    : InterruptHandler(0x2C, manager),
      dataPort(0x60),
      commandPort(0x64)
{
    Handler = handler;
}
PS2MouseDriver::~PS2MouseDriver(){

}

void PS2MouseDriver::Wait(){
    uint64_t timeout = 100000;
    while(timeout--){
        if((commandPort.Read() & 0b10) == 0){
            return;
        }
    }
}

void PS2MouseDriver::Wait_Input(){
    uint64_t timeout = 100000;
    while(timeout--){
        if(commandPort.Read() & 0b1){
            return;
        }
    }
}

void PS2MouseDriver::Activate(){
    offset = 0; //Default Buffers
    buttons = 0;

    commandPort.Write(0xA8); //PIC start sending interrupts
    Wait();

    commandPort.Write(0x20); //Get state
    Wait_Input();

    uint8_t status = dataPort.Read() | 2;
    Wait();
    
    commandPort.Write(0x60); //Set State
    Wait();

    dataPort.Write(status);

    Wait();
    commandPort.Write(0xD4);
    Wait();
    dataPort.Write(0xF6); //Activate

    Wait_Input();
    dataPort.Read();

    Wait();
    commandPort.Write(0xD4);
    Wait();
    dataPort.Write(0xF4); //Activate

    Wait_Input();
    dataPort.Read();
}

bool Skip = true;

uint64_t PS2MouseDriver::HandleInterrupt(uint64_t rsp){
    if(Skip) {Skip = false; return rsp;} //Skip first packet

    //Read status
    uint8_t status = commandPort.Read();
    if (!(status & 0x20))
        return rsp; //If not ready return

    //Read Buffer at current offset
    buffer[offset] = dataPort.Read(); //We dont want to skip this if there is no handler
    
    if(Handler == 0) //If handler does not exist leave
        return rsp;

    offset = (offset + 1) % 3; //Increase offset while keeping it less than three

    if(offset == 0) //If offset 0 (Means full buffer overwrite)
    {
        if(buffer[1] != 0 || buffer[2] != 0) //Buffer positions for cursor movement
        {
            Handler->OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2])); //We send to handler but inverse y because it is negative version
        }

        for(uint8_t i = 0; i < 3; i++) //We loop over buttons
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i))) //Check if it is different
            {
                if(buttons & (0x1<<i)) //Shows if it is up or down
                    Handler->OnMouseUp(i+1); //Send to handler
                else
                    Handler->OnMouseDown(i+1); //Send to handler
            }
        }
        buttons = buffer[0]; //Load the buttons from buffer for next comparison
    }
    
    return rsp;
}