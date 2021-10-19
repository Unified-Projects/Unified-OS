#include "mouse.h"
#include "../screen/rendering/BasicRenderer.h"

//Will Eventually Swap this for image loading when FS is done
uint8_t MousePointer[] = {
    0b10000000, 0b00000000,
    0b11000000, 0b00000000,
    0b11100000, 0b00000000,
    0b11110000, 0b00000000,
    0b11111000, 0b00000000,
    0b11111100, 0b00000000,
    0b11111110, 0b00000000,
    0b11111111, 0b00000000,
    0b11111111, 0b10000000,
    0b11111000, 0b00000000,
    0b11011000, 0b00000000,
    0b10011100, 0b00000000,
    0b00001100, 0b00000000,
    0b00001100, 0b00000000,
    0b00000000, 0b00000000,
    0b00000000, 0b00000000,
};

//Wait for mouse IO ready
void mouse_wait(){
    uint64_t timeout = 100000;
    while(timeout--){
        if((inb(0x64) & 0b10) == 0){
            return;
        }
    }
}

//Wait for mouse Input IO Ready
void mouse_wait_input(){
    uint64_t timeout = 100000;
    while(timeout--){
        if(inb(0x64) & 0b1){
            return;
        }
    }
}

//Write Data to Mouse IO
void MouseWrite(uint8_t value){
    mouse_wait();
    outb(0x64, 0xD4);
    mouse_wait();
    outb(0x60, value);
}

//Read from Mouse IO
uint8_t MouseRead(){
    mouse_wait_input();
    return inb(0x60);
}

//Mouse Setup
uint8_t mouseCycle = 0;
uint8_t MousePacket[4];
bool MousePacketReady = false;
Point MousePosition;
Point MousePositionOld;

//Interpret Mouse Data
void HandlePS2Mouse(uint8_t data){

    ProcessMousePacket();

    //Skip first packet
    static bool skip = true;
    
    if(skip) {skip = false; return;}
    
    //Change specific Data Sections dependinc on packet type
    switch (mouseCycle)
    {
    case 0:
        if((data & 0b00001000) == 0) break;
        MousePacket[0] = data;
        mouseCycle++;
        break;
    case 1:
        MousePacket[1] = data;
        mouseCycle++;
        break;
    case 2:
        MousePacket[2] = data;
        MousePacketReady = true;
        mouseCycle = 0;
        break;
    }
}

//Process Data from interrupt
void ProcessMousePacket(){
    if (!MousePacketReady) return;

    //Checker for invalid positions
    bool xNegative, yNegative, xOverflow, yOverflow;

    if (MousePacket[0] & PS2XSign){
        xNegative = true;
    }else xNegative = false;

    if (MousePacket[0] & PS2YSign){
        yNegative = true;
    }else yNegative = false;

    if (MousePacket[0] & PS2XOverflow){
        xOverflow = true;
    }else xOverflow = false;

    if (MousePacket[0] & PS2YOverflow){
        yOverflow = true;
    }else yOverflow = false;
    
    //Sort out packet if negative x
    if (!xNegative){
        MousePosition.x += MousePacket[1];
        if (xOverflow){
            MousePosition.x += 255;
        }
    } else
    {
        MousePacket[1] = 256 - MousePacket[1];
        MousePosition.x -= MousePacket[1];
        if (xOverflow){
            MousePosition.x -= 255;
        }
    }

    //Sort out packet if negative y
    if (!yNegative){
        MousePosition.y -= MousePacket[2];
        if (yOverflow){
            MousePosition.y -= 255;
        }
    } else
    {
        MousePacket[2] = 256 - MousePacket[2];
        MousePosition.y += MousePacket[2];
        if (yOverflow){
            MousePosition.y += 255;
        }
    }

    //Button Interactions (Will be sent to system eventaully)
    if(MousePacket[0] & PS2Leftbutton){

    }
    if(MousePacket[0] & PS2Middlebutton){
        
    }
    if(MousePacket[0] & PS2Rightbutton){
        
    }

    //Mouse positioning and Renderer Setup (Will be changed to System Manager At Some Point)
    if (MousePosition.x < 0) MousePosition.x = 0;
    if (MousePosition.x > GlobalRenderer->framebuffer->Width-1) MousePosition.x = GlobalRenderer->framebuffer->Width-1;
    
    if (MousePosition.y < 0) MousePosition.y = 0;
    if (MousePosition.y > GlobalRenderer->framebuffer->Height-1) MousePosition.y = GlobalRenderer->framebuffer->Height-1;

    //Update Screen
    //GlobalRenderer->putChar(0xffffffff, 'a', MousePosition.x, MousePosition.y);
    GlobalRenderer->ClearMouseCursor(MousePointer, MousePositionOld);
    GlobalRenderer->DrawMouseCursor(MousePointer, MousePosition, 0xffffffff);

    //Await new packet
    MousePacketReady = false;
    MousePositionOld = MousePosition;
}

//Sort out mouse for interrupt handling
void PS2MouseIntialize(){
    outb(0x64, 0xA8); //enabling the auxiliary device - mouse

    mouse_wait();
    outb(0x64, 0x20); //tells the keyboard controller that we want to send a command to the mouse
    mouse_wait_input();
    uint8_t status = inb(0x60);
    status |= 0b10;
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status); // setting the correct bit is the "compaq" status byte

    MouseWrite(0xF6);
    MouseRead();

    MouseWrite(0xF4);
    MouseRead();
}