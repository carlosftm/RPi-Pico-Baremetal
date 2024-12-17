# RPi-Pico-Baremetal

Do you pine for the nice days of 8-bit microcontrollers, when men were men and wrote their own device drivers?

This repository is a comprehensive guide to programming the Raspberry Pi Pico board based on the RP2040 microcontroller, without relying on the Pico-SDK, operating system or additional software layers. Here, you'll find examples to better understand the RP2040 boot process, how to writing a linker script to load the code to Flash memory or to RAM, configuring peripherals like GPIOs, clocks, PLLs, interrupts, and more.

By exploring baremetal programming, you'll gain a deep understanding of the RP2040's inner workings and unleash its full potential. Whether you're a beginner or an experienced developer, this project provides the knowledge and resources needed to excel in baremetal programming.

Key Features:
1. Clear examples: The project includes a variety of examples with detailed explanations with little complexity.
2. In-depth explanations: Understand the underlying concepts and mechanisms of baremetal programming.
3. Step-by-step tutorials: Practical, hands-on learning experience from setup to running code on the Raspberry Pi Pico.

Please note that in all the examples provided in this project, the code is intentionally written in an explicit manner, prioritizing clarity over optimization. While advanced programmers may find more efficient ways to implement the same functions with fewer lines of code, my goal here is to ensure that beginners can easily follow along without getting lost in abstract or complex code snippets. The emphasis is on understanding the concepts and principles behind each step, rather than on achieving the most optimized code.

I hope you find this GitHub project inspiring as you embark on your journey into the world of baremetal programming with the RP2040 microcontroller.

Enjoy exploring and building amazing things!


# Prerequisites:

To get started with this GitHub project, you will need to follow a few prerequisites. Here are the steps required to begin using the project effectively:

1. Biuld machine: This project has been developed and verified on a Linux machine. It is recommended to use a PC with Ubuntu or a Raspberry Pi with Raspbian. The Raspberry Pi Foundation provides strong support for Linux, making it ideal for code development related to the Raspberry Pi Pico.

2. Install Tools and SDK (2.1.0 or higher): Follow the instructions provided in the official Raspberry Pi Pico C/C++ SDK documentation to install the necessary build tools (gcc for ARM, CMake, etc.) and the Pico-SDK. The SDK contains essential files and utilities that will be required. Make sure to have these installed on your system before proceeding.

  * [pico-sdk](https://github.com/raspberrypi/pico-sdk): The [Getting started with Raspberry Pi Pico-series](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf) guide, describes how to install the C/C++ Pico SDK and all the necesary tools. I personally followed the steps described in `Appendix C: Manual toolchain setup`.

  * [picotool](https://github.com/raspberrypi/picotool): The Raspberry Pi Pico SDK (pico-sdk) version 2.0.0 and above uses picotool to do the ELF-to-UF2 conversion previously handled by the elf2uf2 tool in the SDK. If you installed the Pico SDK using the script from `Appendix C: Manual toolchain setup` of the [Getting started with Raspberry Pi Pico-series](https://datasheets.raspberrypi.com/pico/getting-started-with-pico.pdf), the picotool executable file must have been copied on the `/usr/local/bin`directory by the installation script.

3. RP2040 Based Board: You will need an RP2040 based board to work with this project. The examples and code have been tested on an original Raspberry Pi Pico board. While it is possible to use a different board, please note that you may need to make adaptations to the code. For instance, the on-board LED may be connected to a different GPIO, or the board may use a different external flash memory.

By following these prerequisites, you will set up your development environment and have the necessary tools and hardware to dive into the GitHub project successfully.

Keep in mind that you may need to update the Makefile of each example to match the directory where the `pico-sdk` and the `picotool` are installed.

# Tips:

- By using Microsoft Visual Studio Code with the Remote SSH extension on your Destop PC, you can work remotely on the source code and flash the Pico on the Raspberry Pi (via SSH). I incluided a json file to be able to build the examples by presing **ctrl+shift+b**.
- Serial port:
    - When working with serial port with RPI4, use "sudo raspi-config" -> 3 Interfaces Options -> I6 Serial port to disable shell messages on the serial port and to enable it.
    - if still there is unwanted data on the RPI4 serial port, then disable the following services: "sudo systemctl disable serial-getty@serial0.service"
- To generate the CMSIS Header file out of the rp2040.svd provided in the pico sdk:
    - SVDconv tool: https://github.com/Open-CMSIS-Pack/devtools
    - svd file: ~/pico/pico-sdk/src/rp2040/hardware_regs/rp2040.svd
    - commnad: ./SVDConv rp2040.svd --generate=header --fields=struct --fields=macro  --fields=enum --debug-headerfile
    - I had to add manually the peripheral declaration for the XOR, CLR and SET atomic access.

- Debugging with J-Link:
    - ![j-tag to RPi Pico connection](https://github.com/carlosftm/RPi-Pico-Baremetal/assets/13556362/58a44b32-9690-4f96-9ead-32db2a957067)


# References:
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
