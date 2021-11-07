#ifndef __UNIFIED_OS_IO_PORT_H
#define __UNIFIED_OS_IO_PORT_H

#include <common/stdint.h>

namespace UnifiedOS{
    namespace IO{
        class Port
        {
        protected:
            Port(uint16_t portnumber);
            ~Port();
            uint16_t portnumber;
        };


        class Port8Bit : public Port
        {
        public:
            Port8Bit(uint16_t portnumber);
            ~Port8Bit();

            virtual uint8_t Read();
            virtual void Write(uint8_t data);

        protected:
            static inline uint8_t Read8(uint16_t _port)
            {
                uint8_t result;
                asm volatile("inb %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }

            static inline void Write8(uint16_t _port, uint8_t _data)
            {
                asm volatile("outb %0, %1" : : "a" (_data), "Nd" (_port));
            }
        };



        class Port8BitSlow : public Port8Bit
        {
        public:
            Port8BitSlow(uint16_t portnumber);
            ~Port8BitSlow();

            virtual void Write(uint8_t data);
        protected:
            static inline void Write8Slow(uint16_t _port, uint8_t _data)
            {
                asm volatile("outb %0, %1" : : "a"(_data), "Nd"(_port));
                asm volatile ("outb %%al, $0x80" : : "a"(0));
            }
        };



        class Port16Bit : public Port
        {
        public:
            Port16Bit(uint16_t portnumber);
            ~Port16Bit();

            virtual uint16_t Read();
            virtual void Write(uint16_t data);

        protected:
            static inline uint16_t Read16(uint16_t _port)
            {
                uint16_t result;
                asm volatile("inw %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }

            static inline void Write16(uint16_t _port, uint16_t _data)
            {
                asm volatile("outw %0, %1" : : "a" (_data), "Nd" (_port));
            }
        };



        class Port32Bit : public Port
        {
        public:
            Port32Bit(uint16_t portnumber);
            ~Port32Bit();

            virtual uint32_t Read();
            virtual void Write(uint32_t data);

        protected:
            static inline uint32_t Read32(uint16_t _port)
            {
                uint32_t result;
                asm volatile("inl %1, %0" : "=a" (result) : "Nd" (_port));
                return result;
            }

            static inline void Write32(uint16_t _port, uint32_t _data)
            {
                asm volatile("outl %0, %1" : : "a"(_data), "Nd" (_port));
            }
        };
    }
}

#endif