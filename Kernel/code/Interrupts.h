#pragma once

/* ///////////////////////////////////////////////////////////////////////////

	[interrupts events]

	TODO:
		fix PIC init func
		write interrupts for irq0 and irq1

		write init_apic func


/////////////////////////////////////////////////////////////////////////// */

#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "\OS\Kernel\std\stdlibh.h"

void init_interrupts();
void init_apic();

#endif // !INTERUPTS_H
