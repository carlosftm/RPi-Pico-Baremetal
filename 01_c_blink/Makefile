# Copyright (c) 2024 CarlosFTM
# This code is licensed under MIT license (see LICENSE.txt for details)
NAME    = c-blink
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi
AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
LDFLAGS = -nostdlib
CFLAGS  = -mcpu=$(CPU) -nostartfiles -ffreestanding -g -O0 -fpic -mthumb -c
PICOSDK = ~/pico/pico-sdk
PICOTOOL = /usr/local/bin

all: $(NAME).bin

$(NAME)-asm.o: $(NAME)-asm.s
	$(ARMGNU)-as $(AFLAGS) $(NAME)-asm.s -o $(NAME)-asm.o

$(NAME)-main.o: $(NAME)-main.c
	$(ARMGNU)-gcc $(CFLAGS) $(NAME)-main.c -o $(NAME)-main.o

$(NAME).bin: boot_stage2.ld $(NAME)-asm.o $(NAME)-main.o
	$(ARMGNU)-ld $(LDFLAGS) --entry 0x20040001 -T boot_stage2.ld $(NAME)-asm.o $(NAME)-main.o -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin
	$(PICOTOOL)/picotool uf2 convert $(NAME).bin $(NAME).uf2 -o 0x20040000 --family rp2040

clean: 
	rm -f *.bin *.o *.elf *.list *.uf2

