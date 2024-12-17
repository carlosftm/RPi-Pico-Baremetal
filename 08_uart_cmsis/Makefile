# Copyright (c) 2023 CarlosFTM
# This code is licensed under MIT license (see LICENSE.txt for details)

NAME    = uart_blocking
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi
AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
LDFLAGS = -nostdlib
INC_DIR = ./headers
CFLAGS  = -mcpu=$(CPU) -ffreestanding -nostartfiles -g -O0 -fpic -mthumb -c -I$(INC_DIR)
PICOSDK = ~/pico/pico-sdk
PICOTOOL = /usr/local/bin

all: $(NAME).uf2

boot2.bin : boot2.s memmap_boot2.ld
	$(ARMGNU)-as $(AFLAGS) boot2.s -o boot2.o
	$(ARMGNU)-ld $(LDFLAGS) -T memmap_boot2.ld boot2.o -o boot2.elf
	$(ARMGNU)-objcopy -O binary boot2.elf boot2.bin

boot2_patch.o : boot2.bin
	$(PICOSDK)/src/rp2040/boot_stage2/pad_checksum -p 256 -s 0xFFFFFFFF boot2.bin boot2_patch.s
	$(ARMGNU)-as $(AFLAGS) boot2_patch.s -o boot2_patch.o

$(NAME).o: $(NAME).c
	$(ARMGNU)-gcc $(CFLAGS) $(NAME).c -o $(NAME).o

$(NAME).bin : memmap.ld boot2_patch.o $(NAME).o
	$(ARMGNU)-ld $(LDFLAGS) -T memmap.ld boot2_patch.o $(NAME).o -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin

$(NAME).uf2 : $(NAME).bin
	$(PICOTOOL)/picotool uf2 convert $(NAME).bin $(NAME).uf2 -o 0x10000000 --family rp2040

clean: 
	rm -f *.bin *.o *.elf *.list *.uf2 boot2_patch.*
