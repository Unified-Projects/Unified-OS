#pragma once
#include "../screen/rendering/BasicRenderer.h"
#include "../userinput/mouse.h"

#define PIC1_COMMAND 0x20
#define PIC1_DATA 0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA 0xA1
#define PIC_EOI 0x20

#define ICW1_INIT 0x10
#define ICW1_ICW4 0x01
#define ICW4_8086 0x01

struct interrupt_frame;
__attribute__((interrupt)) void Div0(interrupt_frame* frame);
__attribute__((interrupt)) void BoundRangeExceeded(interrupt_frame* frame);
__attribute__((interrupt)) void InvalidOpcode(interrupt_frame* frame);
__attribute__((interrupt)) void DevNotAvailable(interrupt_frame* frame);
__attribute__((interrupt)) void InvalidTSS(interrupt_frame* frame);
__attribute__((interrupt)) void SegmentNotPresent(interrupt_frame* frame);
__attribute__((interrupt)) void StackSegmentFault(interrupt_frame* frame);
__attribute__((interrupt)) void x87FloatingPoint(interrupt_frame* frame);
__attribute__((interrupt)) void AllignmentCheck(interrupt_frame* frame);
__attribute__((interrupt)) void MachineCheck(interrupt_frame* frame);
__attribute__((interrupt)) void SIMDFloatingPoint(interrupt_frame* frame);
__attribute__((interrupt)) void VirtulizationError(interrupt_frame* frame);
__attribute__((interrupt)) void SecurityException(interrupt_frame* frame);
__attribute__((interrupt)) void TrippleFault(interrupt_frame* frame);

__attribute__((interrupt)) void PageFault_Handler(interrupt_frame* frame);
__attribute__((interrupt)) void DoubleFault_Handler(interrupt_frame* frame);
__attribute__((interrupt)) void GPFault_Handler(interrupt_frame* frame);
__attribute__((interrupt)) void KeyboardInterrupt_Handler(interrupt_frame* frame);
__attribute__((interrupt)) void MouseInterrupt_Handler(interrupt_frame* frame);
__attribute__((interrupt)) void PitInterrupt_Handler(interrupt_frame* frame);

void RemapPIC();
void PIC_EndMaster();
void PIC_EndSlave();