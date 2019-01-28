#include "Interrupts.h"

#define PIC_M 0x0020			/* base address of Mater PIC */
#define PIC_S 0x00A0			/* base address of Slave PIC */
#define PIC_M_COMMAND PIC_M
#define PIC_S_COMMAND PIC_S
#define PIC_M_DATA PIC_M + 1
#define PIC_S_DATA PIC_S + 1

#define ICW1_ICW4	0x01		/* ICW4 (not) needed */
#define ICW1_SINGLE	0x02		/* Single (cascade) mode */
#define ICW1_INTERVAL4	0x04	/* Call address interval 4 (8) */
#define ICW1_LEVEL	0x08		/* Level triggered (edge) mode */
#define ICW1_INIT	0x10		/* Initialization - required! */

#define ICW4_8086	0x01		/* 8086/88 (MCS-80/85) mode */
#define ICW4_AUTO	0x02		/* Auto (normal) EOI */
#define ICW4_BUF_SLAVE	0x08	/* Buffered mode/slave */
#define ICW4_BUF_MASTER	0x0C	/* Buffered mode/master */
#define ICW4_SFNM	0x10		/* Special fully nested (not) */

typedef struct {
	uint16 offset_lower;
	uint16 selector;
	uint8 zero;
	uint8 type_attr;
	uint16 offset_higher;
} __attribute__((packed)) IDT_entry;

IDT_entry IDT[288];

void irq0() {
	outb(0x20, 0x20); //EOI
}

void irq1() {
	memset((char*)0xB8000 + 25 * 85 - 1, 'Q', 1);
	outb(0x20, 0x20); //EOI
}

void irq2() {
	outb(0x20, 0x20); //EOI
}

void irq3() {
	outb(0x20, 0x20); //EOI
}

void irq4() {
	outb(0x20, 0x20); //EOI
}

void irq5() {
	outb(0x20, 0x20); //EOI
}

void irq6() {
	outb(0x20, 0x20); //EOI
}

void irq7() {
	outb(0x20, 0x20); //EOI
}

void irq8() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq9() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq10() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq11() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq12() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq13() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq14() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}

void irq15() {
	outb(0xA0, 0x20);
	outb(0x20, 0x20); //EOI
}
/* ICW2 is vector offset
ICW3 is how master wired to slaves
ICW4 gives additional information about the enviroment */

void init_interrupts() {

	uint8 interrupt_mask_M = inb(PIC_M_DATA);
	uint8 interrupt_mask_S = inb(PIC_S_DATA);

	outb(PIC_M_COMMAND, ICW1_ICW4 | ICW1_INIT);
	io_wait();
	outb(PIC_S_COMMAND, ICW1_ICW4 | ICW1_INIT);
	io_wait();
	outb(PIC_M_DATA, 0x20);
	io_wait();
	outb(PIC_S_DATA, 0x28);
	io_wait();
	outb(PIC_M_DATA, 4);
	io_wait();
	outb(PIC_S_DATA, 2);
	io_wait();

	outb(PIC_M_DATA, ICW4_8086);
	io_wait();
	outb(PIC_S_DATA, ICW4_8086);
	io_wait();

	outb(PIC_M_DATA, interrupt_mask_M);
	outb(PIC_S_DATA, interrupt_mask_S);

	/*outb(0x20, 0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 40);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);*/

	IDT[32].offset_lower = ((size_t)(&irq0)) & 0xffff;
	IDT[32].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[32].zero = 0;
	IDT[32].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[32].offset_higher = (((size_t)(&irq0)) & 0xffff0000) >> 16;

	IDT[33].offset_lower = ((size_t)(&irq1)) & 0xffff;
	IDT[33].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[33].zero = 0;
	IDT[33].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[33].offset_higher = (((size_t)(&irq1)) & 0xffff0000) >> 16;

	IDT[34].offset_lower = ((size_t)(&irq2)) & 0xffff;
	IDT[34].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[34].zero = 0;
	IDT[34].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[34].offset_higher = (((size_t)(&irq2)) & 0xffff0000) >> 16;

	IDT[35].offset_lower = ((size_t)(&irq3)) & 0xffff;
	IDT[35].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[35].zero = 0;
	IDT[35].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[35].offset_higher = (((size_t)(&irq3)) & 0xffff0000) >> 16;

	IDT[36].offset_lower = ((size_t)(&irq4)) & 0xffff;
	IDT[36].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[36].zero = 0;
	IDT[36].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[36].offset_higher = (((size_t)(&irq4)) & 0xffff0000) >> 16;

	IDT[37].offset_lower = ((size_t)(&irq5)) & 0xffff;
	IDT[37].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[37].zero = 0;
	IDT[37].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[37].offset_higher = (((size_t)(&irq5)) & 0xffff0000) >> 16;

	IDT[38].offset_lower = ((size_t)(&irq6)) & 0xffff;
	IDT[38].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[38].zero = 0;
	IDT[38].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[38].offset_higher = (((size_t)(&irq6)) & 0xffff0000) >> 16;
	
	IDT[39].offset_lower = ((size_t)(&irq7)) & 0xffff;
	IDT[39].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[39].zero = 0;
	IDT[39].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[39].offset_higher = (((size_t)(&irq7)) & 0xffff0000) >> 16;

	IDT[40].offset_lower = ((size_t)(&irq8)) & 0xffff;
	IDT[40].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[40].zero = 0;
	IDT[40].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[40].offset_higher = (((size_t)(&irq8)) & 0xffff0000) >> 16;

	IDT[41].offset_lower = ((size_t)(&irq9)) & 0xffff;
	IDT[41].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[41].zero = 0;
	IDT[41].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[41].offset_higher = (((size_t)(&irq9)) & 0xffff0000) >> 16;

	IDT[42].offset_lower = ((size_t)(&irq10)) & 0xffff;
	IDT[42].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[42].zero = 0;
	IDT[42].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[42].offset_higher = (((size_t)(&irq10)) & 0xffff0000) >> 16;

	IDT[43].offset_lower = ((size_t)(&irq11)) & 0xffff;
	IDT[43].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[43].zero = 0;
	IDT[43].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[43].offset_higher = (((size_t)(&irq11)) & 0xffff0000) >> 16;

	IDT[44].offset_lower = ((size_t)(&irq12)) & 0xffff;
	IDT[44].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[44].zero = 0;
	IDT[44].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[44].offset_higher = (((size_t)(&irq12)) & 0xffff0000) >> 16;

	IDT[45].offset_lower = ((size_t)(&irq13)) & 0xffff;
	IDT[45].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[45].zero = 0;
	IDT[45].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[45].offset_higher = (((size_t)(&irq13)) & 0xffff0000) >> 16;

	IDT[46].offset_lower = ((size_t)(&irq14)) & 0xffff;
	IDT[46].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[46].zero = 0;
	IDT[46].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[46].offset_higher = (((size_t)(&irq14)) & 0xffff0000) >> 16;

	IDT[47].offset_lower = ((size_t)(&irq15)) & 0xffff;
	IDT[47].selector = 0x08;										// KERNEL_CODE_SEGMENT_OFFSET
	IDT[47].zero = 0;
	IDT[47].type_attr = 0x8E;										// INTERRUPT_GATE
	IDT[47].offset_higher = (((size_t)(&irq15)) & 0xffff0000) >> 16;

	unsigned long idt_address;
	unsigned long idt_ptr[2];

	idt_address = (unsigned long)IDT;
	idt_ptr[0] = (sizeof(struct IDT_entry) * 286) + ((idt_address & 0xffff) << 16);
	idt_ptr[1] = idt_address >> 16;

	asm("lidt (%0)"::"a"(idt_ptr));
}

void init_apic() {

}