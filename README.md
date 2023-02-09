# RPi-Pico-Baremetal

Do you pine for the nice days of 8-bit microcontrollers, when men were men and wrote their own device drivers?


Tips:
- Serial port:
    - When working with serial port with RPI4, use "sudo raspi-conmfig" -> 3 Interfaces Options -> I6 Serial port to disable shell messages on the serial port and to enable it.
    - if still there is unwanted data on the RPI4 serial port, then disable the following services: "sudo systemctl disable serial-getty@serial0.service"
