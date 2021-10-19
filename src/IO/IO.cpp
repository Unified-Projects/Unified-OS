#include "IO.h"

//Main system communication
void outb(uint16_t port, uint8_t value){
    asm volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
}

//Main system communication
uint8_t inb(uint16_t port){
    uint8_t returnVal;
    asm volatile ("inb %1, %0"
    : "=a"(returnVal)
    : "Nd"(port));
    return returnVal;
}

//Wait for the io to be ready for interaction
void io_wait(){
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}