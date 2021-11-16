# Unified-OS 64-Bit
A Custom Operating System

## What now:
Syscalls need to be worked on (adding propper functionallity).
I need to update my scheduling to utilise vectors, also removing priority levels and PID unlimited
Vectors need a pop_front (although can be ignored)
Vectors need a find (iterater) for processes
I need to add some Interprocess Communication using a syscall messaging system
Filesystem
Commenting
AHCI - DiskDevice to privatise readDiskBlock and make a new one that reads into the diskDivice buffers them memcps