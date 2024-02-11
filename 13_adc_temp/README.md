# 13_adc_temp

On this example we access the Analog Digital Converter (ADC) to read the on-chip temperature sensor of the RP2040.
The input to the ADC is connected to a 5 input analog multiplexer. The temperature sensor is connected to the analog input 4, and there is no need to reconfigure the digital function of a GPIO.
We will use the one-shot sample mode of the ADC: ``Writing a 1 to CS.START_ONCE will immediately start a new conversion. CS.READY will go low, to show that a
conversion is currently in progress. After 96 cycles of clk_adc, CS.READY will go high. The 12-bit conversion result is
available in RESULT``.
The conversion of the raw data is not covered in this example. The RP2040 specs includes all the information to convert the raw data to Celsius degrees.
