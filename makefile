CC=gcc
CFLAGS= -ffreestanding -fshort-wchar -mno-red-zone -nostdlib -fno-use-cxa-atexit -fno-builtin -fno-rtti -fno-exceptions -fno-leading-underscore -Wno-write-strings -Iinc -fpermissive

ASM=nasm
ASMFLAGS= -f elf64

LD=ld
LDFLAGS= -static -Bsymbolic -nostdlib

SRCDIR := src
OBJDIR := obj
BUILDDIR := bin

rwildcard=$(foreach d,$(wildcard $(1:=/*)),$(call rwildcard,$d,$2) $(filter $(subst *,%,$2),$d))

SRC = $(call rwildcard,$(SRCDIR),*.cpp)
ASMSRC = $(call rwildcard,$(SRCDIR),*.asm)
OBJS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SRC))
OBJS += $(patsubst $(SRCDIR)/%.asm, $(OBJDIR)/%_asm.o, $(ASMSRC))
DIRS = $(wildcard $(SRCDIR)/*)

LINKER=linker-x86_64.ld
KERNEL=kernel.elf

kernel: $(OBJS) link

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $^ -o $@

$(OBJDIR)/%_asm.o: $(SRCDIR)/%.asm
	@ echo !==== COMPILING $^
	@ mkdir -p $(@D)
	$(ASM) $(ASMFLAGS) $^ -o $@

link: $(LINKER)
	@ echo !==== Linking
	$(LD) $(LDFLAGS) -T $< -o $(BUILDDIR)/x86_64/$(KERNEL) $(OBJS)

all: clean kernel

clean:
	rm -rf ./obj

qemu:
	E:\launch2.bat
#qemu-system-x86_64 -machine q35 -m 1G -hda ./kernel.iso -device VGA	
	qemu-system-x86_64 -machine q35 -hda //./PhysicalDrive3 -m 2G -cpu qemu64 -drive if=pflash,format=raw,unit=0,file="OVMFbin/OVMF_CODE-pure-efi.fd",readonly=on -drive if=pflash,format=raw,unit=1,file="OVMFbin/OVMF_VARS-pure-efi.fd" -net none

vbox:
	"D:\Program Files\Oracle\VirtualBox\VBoxManage.exe" startvm 'Unified OS'