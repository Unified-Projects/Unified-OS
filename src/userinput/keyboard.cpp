#include "keyboard.h"

bool isLeftShiftPressed;
bool isRightShiftPressed;

//Translate Scancode and special scancodes
void HandleKeyboard(uint8_t scancode){
    switch (scancode){
        case LeftShift:
            isLeftShiftPressed = true;
            return;
        case LeftShift + 0x80:
            isLeftShiftPressed = false;
            return;
        case RightShift:
            isRightShiftPressed = true;
            return;
        case RightShift + 0x80:
            isRightShiftPressed = false;
            return;
        case Enter:
            GlobalRenderer->Next();
            return;
        case Spacebar:
            GlobalRenderer->putChar(' ');
            return;
        case BackSpace:
           GlobalRenderer->ClearChar();
           return;
    }

    char ascii = QWERTYKeyboard::Translate(scancode, isLeftShiftPressed | isRightShiftPressed);
    if(ascii != 0){
        GlobalRenderer->putChar(ascii);
    }
}