MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx)  : ORIGIN = 0x20000000, LENGTH = 256k
}
SECTIONS
{
    .text : {
        . = ORIGIN(RAM);
        __boot2_start__ = .;
        KEEP(*(.boot2))
        __boot2_end__ = .;
        KEEP(*(.boot.entry))
        KEEP(*(.text*))
        __end_code_ = .;
    } > RAM
            
    ASSERT(__boot2_end__ - __boot2_start__ == 256,
        "ERROR: Pico second stage bootloader must be 256 bytes in size")
}
