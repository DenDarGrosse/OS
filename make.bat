FASM Bootloader\boot2.ASM compiled\boot2.bin

FASM Kernel\code\startup.ASM Kernel\code\startup.o

gcc -c -m32 -ffreestanding -o Kernel\code\Kernel.o Kernel\code\Kernel.cpp
gcc -c -m32 -ffreestanding -o Kernel\code\interrupts.o Kernel\code\interrupts.cpp
gcc -c -m32 -ffreestanding -o Kernel\std\stdlibh.o Kernel\std\stdlibh.cpp

ld -mi386pe -T script.ld -o compiled\kernel.bin Kernel\code\startup.o Kernel\code\interrupts.o Kernel\code\Kernel.o Kernel\std\stdlibh.o

objcopy compiled\kernel.bin -O binary

PAUSE