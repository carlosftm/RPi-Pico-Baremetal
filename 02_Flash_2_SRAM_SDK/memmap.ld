/* SPDX-License-Identifier: MIT */
MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 256k
}

ENTRY(reset)

SECTIONS
{
    /*	
       boot2 section is for embed the precompiled boot2.
       this code from memmap_default.ld in pico-sdk
    */
    .boot2 : {
        __boot2_start__ = .;
        KEEP(*(.boot2))
        __boot2_end__ = .;
    } > FLASH

    ASSERT(__boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size")
    .text : {
        KEEP (*(.vectors))
    	KEEP(*(.text*))
    } > FLASH
}
