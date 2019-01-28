
/* ///////////////////////////////////////////////////////////////////////////

	[Kernel]

	TODO:
		create interrupt handler

/////////////////////////////////////////////////////////////////////////// */


#define _x32_

#include "\OS\Kernel\std\stdlibh.h"
#include "Interrupts.h"

char* memory_map;
int disk_id;
int cnt_of_heads;
int sector_per_track;

void kernel_main(char* memory_mapi, int disk_idi, int cnt_of_headsi, int sector_per_tracki) {

	memory_map = memory_mapi;
	disk_id = disk_idi;
	cnt_of_heads = cnt_of_headsi;
	sector_per_track = sector_per_tracki;

	init_interrupts();
	sti();

	char* screen_buffer = (char*)0xB8000;
	char string[]{ 'K', 0x0F , 'e', 0x0F, 'r', 0x0F, 'n', 0x0F, 'e', 0x0F, 'l', 0x0F, ' ', 0x0F, '3', 0x0F, '2', 0x0F, '-', 0x0F, 'b', 0x0F, 'i', 0x0F, 't', 0x0F, ' ', 0x0F, 'C', 0x0F, '+', 0x0F, '+', 0x0F };

	memcpy(screen_buffer + 2*80 * 24, string, sizeof(string));
}