#include <IO/port.h>

UnifiedOS::IO::Port::Port(uint16_t portnumber)
{
    this->portnumber = portnumber;
}

UnifiedOS::IO::Port::~Port()
{
}

UnifiedOS::IO::Port8Bit::Port8Bit(uint16_t portnumber)
    : Port(portnumber)
{
}

UnifiedOS::IO::Port8Bit::~Port8Bit()
{
}

void UnifiedOS::IO::Port8Bit::Write(uint8_t data)
{
    Write8(portnumber, data);
}

uint8_t UnifiedOS::IO::Port8Bit::Read()
{
    return Read8(portnumber);
}





UnifiedOS::IO::Port8BitSlow::Port8BitSlow(uint16_t portnumber)
    : Port8Bit(portnumber)
{
}

UnifiedOS::IO::Port8BitSlow::~Port8BitSlow()
{
}

void UnifiedOS::IO::Port8BitSlow::Write(uint8_t data)
{
    Write8Slow(portnumber, data);
}





UnifiedOS::IO::Port16Bit::Port16Bit(uint16_t portnumber)
    : Port(portnumber)
{
}

UnifiedOS::IO::Port16Bit::~Port16Bit()
{
}

void UnifiedOS::IO::Port16Bit::Write(uint16_t data)
{
    Write16(portnumber, data);
}

uint16_t UnifiedOS::IO::Port16Bit::Read()
{
    return Read16(portnumber);
}





UnifiedOS::IO::Port32Bit::Port32Bit(uint16_t portnumber)
    : Port(portnumber)
{
}

UnifiedOS::IO::Port32Bit::~Port32Bit()
{
}

void UnifiedOS::IO::Port32Bit::Write(uint32_t data)
{
    Write32(portnumber, data);
}

uint32_t UnifiedOS::IO::Port32Bit::Read()
{
    return Read32(portnumber);
}
