#pragma once

/* ///////////////////////////////////////////////////////////////////////////

	[Header of std]

	TODO:
		


/////////////////////////////////////////////////////////////////////////// */


#ifndef STDLIB_H
#define STDLIB_H

#define NULL 0

typedef signed char 	int8;
typedef unsigned char	uint8;

typedef signed short	int16;
typedef unsigned short	uint16;

typedef signed long		int32;
typedef unsigned long	uint32;

typedef signed long long int64;
typedef unsigned long long uint64;

#ifdef _x64_
typedef uint64 size_t;
typedef uint64 ptrdiff_t
#else
typedef uint32 size_t;
typedef uint32 ptrdiff_t;
#endif

void memset(char* destination, uint8 value, size_t count);

void memset_word(char* destination, uint16 value, size_t count);

void memcpy(char* destination, char* original, size_t count);

int32 memcmp(char* memory1, char* memory2, size_t count);

char* memchr(char *destination, uint8 value, size_t count);

uint8 inb(uint8 port);
uint16 inw(uint8 port);
uint32 inl(uint8 port);

void outb(uint8 port, uint8 value);
void outw(uint8 port, uint16 value);
void outl(uint8 port, uint32 value);

void io_wait();

//зарпетить
void cli();
//разрешить
void sti();
#endif // !STDLIB_H
