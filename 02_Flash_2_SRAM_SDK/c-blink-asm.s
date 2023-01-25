/* SPDX-License-Identifier: MIT */
	.cpu cortex-m0plus
	.thumb

/* vector table */
	.section .vectors, "ax"
	.align 2
	.global __vectors
__vectors:
	.word 0x20001000       ;@Vector table, start with SP and then reset vector
	.word reset

/* reset handler */
	.thumb_func
	.global reset
reset:
	ldr r0, =0x20001000    ;@Set the SP
	mov sp, r0
	bl main
	b .
