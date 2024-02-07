# 00_asm_blink

This is the most basic example of the project.

1. Let's start by compiling the example by calling "make all". Several files are generated out of the .s (assembly) file:
  - .bin (binary file): it contains machine code instructions in binary format, which is the native language of the processor.
  - .elf (Executable and Linkable Format): An ELF file is a common file format. It contains machine code instructions, symbol tables, and debugging information.
  - .uf2 (USB Flashing Format): The UF2 file format is a file container developed by Microsoft for easily flashing firmware onto microcontrollers. It is designed to simplify the process of updating firmware by allowing the microcontroller to appear as a USB mass storage device. A .uf2 file consists of firmware data and metadata, and it can be dragged and dropped onto the microcontroller's virtual USB drive to initiate the firmware update process.
2. Now enter the Pico board to the Bootloader Mode by pressing and holding the BOOTSEL button while connecting the Pico to your computer via USB. Release the button after connecting. Alternatively, you can connect a jumper wire between the BOOTSEL and GND (ground) pins on the Pico and then connect it to your computer via USB.
3. The Pico board will appear as a mass storage device on your computer. Drag and drop the .uf2 file to the root directory of that drive. Once the .uf2 file is ransfered to the Pico board, the Pico will restar and execute the code.

Done! you should see the onboard LED flashing.

But, what is actually happening? how is it possible that my code is executed just by dropping a file on a usb drive?

The answer to this question is: **Bootrom**

As explained on the boot sequence and bootrom section of the rp2040 datasheet, the bootrom is divided in 2 stages:

1. *Bootrom*: When the Pico board is powered up, first piece of code that is executed is the bootrom. The RP2040 has a built-in Boot ROM (Read-Only Memory) that contains a small piece of firmware. This Boot ROM is responsible for performing the initial boot-up tasks. And one of the task is to boot the Pico board as a USB mass storage device. When a UF2 file is copied to the drive, it is downloaded and written to Flash or RAM, and the device is automatically rebooted.
  The bootrom copies 256 bytes from SPI to internal SRAM (SRAM5) and check for valid CRC32 checksum. If the checksum is correct (252 bytes of code + 4 bytes of the checksum), the code (flash second stage) is executed.
   According tothe *RP2040 Datasheet*:

2. *Flash Second Stage*: this is the first 252 bytes of code of our code. If our code is simple and fits on 252 bytes, we are done! There is nothing else to do. But if our code is bigger than 252 bytes, then we need to configure the Synchronous Serial Interface (SSI) controller which is used to communicate with external Flash devices. Until the SSI is correctly configured for the attached flash device, it is not possible to access flash via the XIP (execute-in-place ) address window.
  Once XIP is confgured, the code could be executed directly from FLASH. An alternative is to shadow an image from the external flash memory into the SRAM, and execute from SRAM. The last step of the flash second stage is to call the main function of the code.

As mentioned on top of this file, this is the most simple example, therefore the flash second stage is our main code. To blink an LED, 252 bytes of code is enough.

The linke script ``boot_stage2.ld`` is take from the Pico-SDK and instructs the linker to set the origin address of our code to 0x20041f00, i.e. the last 256 bytes of the SRAM. That means, our code runs on SRAM, and is not copied into the flash memory.


