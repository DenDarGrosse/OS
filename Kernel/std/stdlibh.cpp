#include "stdlibh.h"

void memset(char* destination, uint8 value, size_t count) {
	asm("movl %0, %%eax \n movl %1, %%edi \n movl %2, %%ecx \n rep stosl"
		::"a"((uint32)value | ((uint32)value << 8) | ((uint32)value << 16) | ((uint32)value << 24)), "b"(destination), "c"(count >> 2));
	asm("movb %b0, %%al \n movl %1, %%ecx \n rep stosb"::"a"(value), "b"(count & 3));
}

void memset_word(char* destination, uint16 value, size_t count) {
	asm("movl %0, %%eax \n movl %1, %%edi \n movl %2, %%ecx \n rep stosl"
		::"a"((uint32)value | ((uint32)value << 16)), "b"(destination), "c"(count >> 1));
}

void memcpy(char* destination, char* original, size_t count) {
	asm("movl %0, %%edi \n movl %1, %%esi \n movl %2, %%ecx \n rep movsl"::"a"(destination), "b"(original), "c"(count >> 2));
	asm("movl %0, %%ecx \n rep movsb"::"a"(count & 3));
}

int32 memcmp(char* memory1, char* memory2, size_t count) {
	char above, below;
	asm("movl %0, %%esi \n movl %1, %%edi \n movl %2, %%ecx \n repe cmpsb"::"a"(memory1), "b"(memory2), "c"(count));
	asm("seta %0 \n setb %1":"=a"(above), "=b"(below));
	return above - below;
}

char* memchr(char *destination, uint8 value, size_t count) {
	char *result;
	asm("movb %b0, %%al \n movl %1, %%edi \n movl %2, %%ecx \n repe cmpsb"::"a"(value), "b"(destination), "c"(count));
	asm("movl %%edi, %0":"=a"(result));
	if (result < destination + count) {
		return result;
	}
	else {
		return NULL;
	}
}

uint8 inb(uint8 port)
{
	uint8 out_value;

	asm("inb %w1, %b0":"=a"(out_value) : "d"(port));

	return out_value;
}

uint16 inw(uint8 port)
{
	uint16 out_value;

	asm("inw %w1, %w0":"=a"(out_value) : "d"(port));

	return out_value;
}
uint32 inl(uint8 port)
{
	uint32 out_value;

	asm("inl %w1, %0":"=a"(out_value) : "d"(port));

	return out_value;
}

void outb(uint8 port, uint8 value)
{
	asm("outb %b0, %w1"::"a"(value), "d"(port));
}

void outw(uint8 port, uint16 value)
{
	asm("outw %w0, %w1"::"a"(value), "d"(port));
}

void outl(uint8 port, uint32 value)
{
	asm("outl %0, %w1"::"a"(value), "d"(port));
}

void io_wait()
{
	asm("nop \n nop");
}

void cli() {
	asm("cli");
}

void sti() {
	asm("sti");
}