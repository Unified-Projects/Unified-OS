#include "pit.h"
#include "../../IO/IO.h"

//Timer Class
namespace PIT{
    //System Online Time
    double TimeSinceBoot = 0;

    uint16_t Divisor = 65535;

    //Sleep In Seconds
    void SleepS(double seconds){
        double startTime = TimeSinceBoot;
        while(TimeSinceBoot < startTime + seconds){
            asm("hlt");
        }
    }

    //Sleep in milliseconds
    void SleepM(uint64_t milliseconds){
        SleepS((double)milliseconds / 1000);
    }

    //Send Divisor to the PIT Chip (Frequency)
    void SetDivistor(uint16_t divisor){
        if(Divisor < 100){
            divisor = 100;
        }
        Divisor = divisor;
        outb(0x40, (uint8_t)(divisor & 0x00ff));
        io_wait();
        outb(0x40, (uint8_t)((divisor & 0xff00) >> 8));
    }

    //Returns the frequency (Clocks Per Second)
    uint64_t GetFreqency(){
        return BaseFrequency / Divisor;
    }

    //Set Frequency (Clocks per second)
    void SetFrequency(uint64_t frequency){
        SetDivistor(BaseFrequency / frequency);
    }

    //Run On Every PIT interrupt
    void Tick(){
        TimeSinceBoot += 1 / (double)GetFreqency();
    };
}