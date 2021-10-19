#pragma once

#include "../IO/IO.h"
#include "../Math/Math.h"

//Buttons and Positioning
#define PS2Leftbutton 0b00000001
#define PS2Middlebutton 0b00000010
#define PS2Rightbutton 0b00000100
#define PS2XSign 0b00010000
#define PS2YSign 0b00100000
#define PS2XOverflow 0b01000000
#define PS2YOverflow 0b10000000

//Design
extern uint8_t MousePointer[];

//Mouse
void PS2MouseIntialize();
void HandlePS2Mouse(uint8_t data);
void ProcessMousePacket();

//Cursor Position
extern Point MousePosition;