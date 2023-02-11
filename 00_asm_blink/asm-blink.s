;@ Copyright (c) 2023 CarlosFTM
;@ This code is licensed under MIT license (see LICENSE.txt for details)

	.cpu cortex-m0plus
	.thumb

/* vector table */
	.section .vectors, "ax"
	.align 2
	.global _vectors
_vectors:
	.word 0x20001000
	.word _reset

/* reset handler */
	.thumb_func
	.global _reset
_reset:
	ldr r0, =0x20001000  ;@ Stack Pointer
	mov sp, r0

	ldr  r3, =0x4000f000  ;@ Resets.reset (Atomic bitmask clear)
	movs r2, #32          ;@ IO_BANK0
	str  r2, [r3, #0]

	ldr  r3, =0x400140cc  ;@IO_BANK0.GPIO25_CTRL
	movs r2, #5           ;@Function 5 (SIO)
	str  r2, [r3, #0]

	ldr  r3, =0xd0000020  ;@SIO_BASE.GPIO_OE
	movs r2, #128         ;@GPIO25
	lsl  r2, r2, #18
	str  r2, [r3, #0]

_blink:
	ldr  r3, =0xd000001c  ;@SIO_BASE.GPIO_XOR
	movs r2, #128         ;@GPIO25
	lsl  r2, r2, #18
	str  r2, [r3, #0]

	ldr r0, =100000
	bl  _delay

	b   _blink

/* delay function */
.thumb_func
_delay:
	mov r4,r0
_loop:
	sub r4,r4,#1
	cmp r4, #0
	bne _loop
	bx  lr

.align 4
