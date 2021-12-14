# **Main To Do**

## Processing:

### **Threads**
 - [x] Threads (Multiple threads)
 - [x] Separate Stack
 - [x] States
 - [ ] Blocker
 - [ ] User Space Stack

### **All**
 - [ ] Task killing (Remove Stack Etc)
 - [x] Task Yeilding (Premature Switches)
 - [x] Task Forking (Cloning)
 - [ ] Blocking - Sleeping [OSDev](https://wiki.osdev.org/Blocking_Process)
 - [ ] Context creation from a elf (Executable)
 - [x] Switch to lists
 - [ ] Comtains its own PageMap (Needs to be properly setup and fixed in task switching)
 - [ ] Switch to refPtrs for better queing and memory mapping

## Interprocess Communication

### **Sephmore** [OSDev](https://wiki.osdev.org/Semaphore)
 - [ ] Wait (Blocker)
 - [ ] Signal (Unblocker)

### **Shared Memory** [OSDev](https://wiki.osdev.org/Shared_Memory)
 - [ ] Per Process Memory Map (Page Table)
 
### **Interfaces**
 - [ ] A setup for inter process communcation
 - [ ] Somewhere to send messages

### **Message Passing** [OSDev](https://wiki.osdev.org/Message_Passing) and [OSDevTut](https://wiki.osdev.org/Message_Passing_Tutorial)
 - [ ] Mailboxes (Queue)

### **Signals** [OSDev](https://wiki.osdev.org/Signals)
 - [ ] A signal is sent to a process
 - [ ] raise() function to handle the signal
 - [ ] signal is sent throught signal()
 - [ ] raise() cause error when no handler is found killing process

### **Services**
 - [ ] A named interface that can be interacted with

## Interrupts

### **Halt**
 - [x] Dissable Interrupts
 - [x] Hlt the cpu
 - [x] Used for kernel Panics
 - [x] Send over IPI to other CPUs
 - [x] Assert embedded into Vectors

### **Timer**
 - [x] Callbacks
 - [ ] Optimised to use the Thread Blocker Method
 - [x] Microseconds
 - [x] Tick down

## FileSystem

### **Blocker**
 - [ ] Lock Read/Write when currently being read from (Blocking the thread) (**Disk**)
 - [ ] Assert for invalid inputs

# Once all done I will move refresh this list

## What is needed
I need to (pretty much) delete all my scheduling code and process stuff. Replace it with threads build in signals and handlers for IPC