// #ifndef __UNIFIED_OS_IO_PCI_H
// #define __UNIFIED_OS_IO_PCI_H

// #include <common/stdint.h>
// #include <driver/driver.h>
// #include <IO/port.h>
// #include <interrupts/interrupts.h>
// #include <memory/manager.h>

// namespace UnifiedOS{
//     namespace IO{
//         namespace PCI{

//             enum BaseAddressRegisterType{
//                 MemoryMapping = 0,
//                 InputOutput = 1,
//             };

//             class BaseAddressRegister{
//             public:
//                 bool prefetchable;
//                 uint8_t* address;
//                 uint32_t size;
//                 BaseAddressRegisterType type;
//             };
            
//             class PeripheralComponentInterconnectDeviceDescriptor{
//             public:
//                 uint32_t portBase;
//                 uint32_t interrupt;

//                 uint16_t bus;
//                 uint16_t device;
//                 uint16_t function;

//                 uint16_t vendor_id;
//                 uint16_t device_id;
                
//                 uint8_t class_id;
//                 uint8_t subclass_id;
//                 uint8_t interface_id;

//                 uint8_t revision;

//                 PeripheralComponentInterconnectDeviceDescriptor();
//                 ~PeripheralComponentInterconnectDeviceDescriptor();
//             };

//             class PeripheralComponentInterconnectController{
//                 Port32Bit dataPort;
//                 Port32Bit commandPort;
//             public:
//                 PeripheralComponentInterconnectController();
//                 ~PeripheralComponentInterconnectController();

//                 uint32_t Read(uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset);
//                 void Write(uint16_t bus, uint16_t device, uint16_t function, uint32_t registerOffset, uint32_t data);
//                 bool DeviceHasFunctions(uint16_t bus, uint16_t device);

//                 void SelectDrivers(Driver::DriverManager* driverManager, Interrupt::InterruptManager* interrupts);
//                 Driver::Driver* GetDriver(PeripheralComponentInterconnectDeviceDescriptor device, Interrupt::InterruptManager* interrupts);
                
//                 PeripheralComponentInterconnectDeviceDescriptor GetDeviceDescriptor(uint16_t bus, uint16_t device, uint16_t function);
//                 BaseAddressRegister GetBaseAddressRegister(uint16_t bus, uint16_t device, uint16_t function, uint16_t bar);
//             };
//         }
//     }
// }

// #endif