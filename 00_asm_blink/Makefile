NAME    = asm-blink
CPU     = cortex-m0plus
ARMGNU  = arm-none-eabi
AFLAGS  = --warn --fatal-warnings -mcpu=$(CPU) -g
LDFLAGS = -nostdlib -nostartfiles
PICOSDK = ~/pico/pico-sdk

all: $(NAME).bin

$(NAME).o: $(NAME).s
	$(ARMGNU)-as $(AFLAGS) $(NAME).s -o $(NAME).o

$(NAME).bin: boot_stage2.ld $(NAME).s $(NAME).o
	$(ARMGNU)-ld $(LDFLAGS) --entry 0x20041f01 -T boot_stage2.ld $(NAME).o -o $(NAME).elf
	$(ARMGNU)-objdump -D $(NAME).elf > $(NAME).list
	$(ARMGNU)-objcopy -O binary $(NAME).elf $(NAME).bin
	$(PICOSDK)/tools/elf2uf2/elf2uf2 $(NAME).elf $(NAME).uf2

clean: 
	rm -f *.bin *.o *.elf *.list *.uf2
