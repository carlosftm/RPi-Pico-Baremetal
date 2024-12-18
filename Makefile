# Copyright (c) 2024 CarlosFTM
# This code is licensed under MIT license (see LICENSE.txt for details)
SUBDIRS = 00_asm_blink 01_c_blink 02_Flash_2_SRAM_SDK 03_systick 04_systick_isr 05_pll_clk 06_uart 07_multicore 08_uart_cmsis 09_i2c_blocking 10_pwm 11_ext_int 12_uart_irq 13_adc_temp 14_bootrom_func_data

.PHONY: all clean $(SUBDIRS) $(addsuffix -clean, $(SUBDIRS))

all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	@if [ -f $@/*.uf2 ]; then echo "-- Build successful for $@\n\n"; else echo "** Build failed for $@\n\n"; fi

clean: $(addsuffix -clean, $(SUBDIRS))

$(addsuffix -clean, $(SUBDIRS)):
	$(MAKE) -C $(patsubst %-clean,%,$@) clean
