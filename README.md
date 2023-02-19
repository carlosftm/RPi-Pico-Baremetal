# RPi-Pico-Baremetal

Do you pine for the nice days of 8-bit microcontrollers, when men were men and wrote their own device drivers?


Tips:
- Use a Raspberry Pi with Raspbian as your development machine. Most official guides are tested on Raspbian, so you may not find any issues on your Linux environment. 
    -  By using Microsoft Visual Studio Code with the Remote SSH extension on your Destop PC, you can work remotely on the source code and flash the Pico on the Raspberry Pi (via SSH).
- The UF2 converter provided in the pico-sdk follows a strict memory map. As I'm not using the linkler script provided in the SDK, in some cases my .elf file did not fulfill the converter memory rules. For that reason I searched for an open source alternative and found the ELF-to-UF2 converter from Microsoft. By using the --wait option, the converter waits the RPI-RP2 drive to be mounted to copy the generated UF2 file there. Very convenient.
- Serial port:
    - When working with serial port with RPI4, use "sudo raspi-config" -> 3 Interfaces Options -> I6 Serial port to disable shell messages on the serial port and to enable it.
    - if still there is unwanted data on the RPI4 serial port, then disable the following services: "sudo systemctl disable serial-getty@serial0.service"
- To generate the CMSIS Header file out of the rp2040.svd provided in the pico sdk:
    - SVDconv tool: https://github.com/Open-CMSIS-Pack/devtools
    - svd file: ~/pico/pico-sdk/src/rp2040/hardware_regs/rp2040.svd
    - commnad: ./SVDConv rp2040.svd --generate=header --fields=struct --fields=macro  --fields=enum --debug-headerfile
    - I had to add manually the peripheral declaration for the XOR, CLR and SET atomic access.

References:
- Raspberry Pi
    - Github
        - https://github.com/raspberrypi/pico-sdk
        - https://github.com/raspberrypi/pico-examples
        - https://github.com/microsoft/uf2
    - Documentation
        - https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf
        - https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf
- ARM
    - Documentation
        - https://developer.arm.com/documentation/ddi0484/latest
        - https://developer.arm.com/documentation/dui0662/b/The-Cortex-M0--Processor
- UF2
    - Covenrter
        - https://github.com/microsoft/uf2
- ASCII art: https://manytools.org/hacker-tools/ascii-banner/

- BMP280 Digital Pressure Sensor
    - Datasheet
        - https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bmp280-ds001.pdf
