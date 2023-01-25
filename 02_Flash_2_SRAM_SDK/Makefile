NAME    = c-blink
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi
AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
LDFLAGS = -nostdlib -nostartfiles
CFLAGS  = -mcpu=$(CPU) -ffreestanding -g -O0 -fpic -mthumb -c
PICOSDK = ~/pico/pico-sdk

all: $(NAME).bin

$(NAME)-bs2.o: bs2_default_padded_checksummed.S
	$(ARMGNU)-as $(AFLAGS) bs2_default_padded_checksummed.S -o $(NAME)-bs2.o

$(NAME)-asm.o: $(NAME)-asm.s
	$(ARMGNU)-as $(AFLAGS) $(NAME)-asm.s -o $(NAME)-asm.o

$(NAME)-main.o: $(NAME)-main.c
	$(ARMGNU)-gcc $(CFLAGS) $(NAME)-main.c -o $(NAME)-main.o

$(NAME).bin: memmap.ld $(NAME)-asm.o $(NAME)-bs2.o $(NAME)-main.o
	$(ARMGNU)-ld $(LDFLAGS) -T memmap.ld $(NAME)-bs2.o $(NAME)-asm.o $(NAME)-main.o -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin
	$(PICOSDK)/tools/elf2uf2/elf2uf2 $(NAME).elf $(NAME).uf2

clean: 
	rm -f *.bin *.o *.elf *.list *.uf2
