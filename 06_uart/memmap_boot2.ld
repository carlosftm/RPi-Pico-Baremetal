MEMORY
{
    FLASH(rx) : ORIGIN = 0x10000000, LENGTH = 2048k
    RAM(rwx) : ORIGIN =  0x20000000, LENGTH = 256k
}
SECTIONS
{
    .text : {
		        . = ORIGIN(FLASH);
		        KEEP(*(.boot2))
            } > FLASH
}
