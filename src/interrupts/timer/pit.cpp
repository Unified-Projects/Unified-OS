#include <interrupts/timer/pit.h>

using namespace UnifiedOS;
using namespace UnifiedOS::Interrupts;
using namespace UnifiedOS::Interrupts::Timer;

#define PIT_SCALE 1193180

//Constructors
PIT::PIT(InterruptManager* manager)
    : InterruptHandler(0x20, manager),
      PITDataPort0(0x40),
      PITCommandPort(0x43)
{

}

//Main Sleep
void PIT::SleepS(double seconds){
    //Save start time
    double startTime = TimeSinceBoot;

    //Do nothing untill Times is start time + seconds
    while(TimeSinceBoot < startTime + seconds){
        asm("hlt"); //Performace
    }
}
void PIT::SleepM(uint64_t milliseconds){
    SleepS((double)milliseconds / 1000); //Converts to seconds
}

//Configuring PIT
void PIT::SetDivistor(uint16_t divisor){
    //Minimal divisor value
    if(divisor < 100){
        divisor = 100;
    }

    //Save divisor
    Divisor = divisor;

    //Pass divisor into port
    PITCommandPort.Write(0x36);
    PITDataPort0.Write((uint8_t)(divisor & 0x00ff));
    PITDataPort0.Write((uint8_t)((divisor & 0xff00) >> 8));
}
uint64_t PIT::GetFreqency(){
    return BaseFrequency / Divisor; //Modify with divisor
}
void PIT::SetFrequency(uint64_t frequency){
    SetDivistor(BaseFrequency / frequency);
}

//Interrutp Handler (Ticking)
uint64_t PIT::HandleInterrupt(uint64_t rsp){
    //Will increase Time since boot the tick of a modified frequency
    TimeSinceBoot += 1 / (double)GetFreqency();

    return rsp;
}

//Global Timer
PIT* UnifiedOS::Interrupts::Timer::__TIMER__PIT__ = nullptr;