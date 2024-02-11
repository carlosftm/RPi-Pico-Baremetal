// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"
#include <stdbool.h>
#include "uart.h"

/* Setup XOSC and set it a source clock */
static void setupClocks( void )
{
    // Enable the XOSC
    XOSC->CTRL            = 0xAA0;          // Frequency range: 1_15MHZ
    XOSC->STARTUP_b.DELAY = 0xC4;           // Startup delay ( default value )
    XOSC_SET->CTRL        = 0xFAB000;       // Enable ( magic word )
    while( !(XOSC->STATUS_b.STABLE & 1 ) ); // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    CLOCKS->CLK_REF_CTRL_b.SRC = 2;         // CLK REF source = xosc_clksrc
    CLOCKS->CLK_SYS_CTRL_b.SRC = 0;         // CLK SYS source = clk_ref
    CLOCKS->CLK_REF_DIV_b.INT  = 1;         // CLK REF Divisor = 1
    CLOCKS->CLK_PERI_CTRL_b.AUXSRC = 4;     // CLK PERI AUX SRC = xosc_clksrc
    CLOCKS->CLK_PERI_CTRL_b.ENABLE = 1;     // CLK PERI Enable
    CLOCKS_SET->CLK_ADC_CTRL_b.AUXSRC = 3;  // CLK ADC AUX SRC = xosc_clksrc
    CLOCKS_SET->CLK_ADC_CTRL_b.ENABLE = 1;  // CLK ADC Enable
}

/* reset the subsystems used in this program */
static void resetSubsys( void )
{
    // Reset IO Bank
    RESETS_CLR->RESET_b.io_bank0 = 1;
    while ( RESETS->RESET_DONE_b.io_bank0 == 0 );

    // Reset PADS BANK
    RESETS_CLR->RESET_b.pads_bank0 = 1;
    while ( RESETS->RESET_DONE_b.pads_bank0 == 0 );

    RESETS_CLR->RESET_b.i2c0 = 1;
    while ( RESETS->RESET_DONE_b.i2c0 == 0 );

    RESETS_CLR->RESET_b.adc = 1;
    while ( RESETS->RESET_DONE_b.adc == 0 );
}

/* configure LED */
void ledConfig( void )
{
    // Set GPIO25 as SIO (F5) and GPIO OE
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 5;
    SIO->GPIO_OE_SET_b.GPIO_OE_SET = ( 1 << 25 );
}

/* configure ADC */
static void adcConfig( void )
{
    ADC_SET->CS = 0;           // Init ADC CS to 0
    ADC_SET->CS_b.TS_EN  = 1;  // Enable Temperature Sensor
    ADC_SET->CS_b.AINSEL = 4;  // Select the analog mux input 4
    ADC_SET->CS_b.EN     = 1;  // Power on ADC and enable its clock

}

/* read ADC conversion */
static uint32_t adcRead( void )
{
    ADC_SET->CS_b.START_ONCE = 1;    //Start a single conversion
    while ( ADC->CS_b.READY == 0 );  //wait for conversion to be compleated

    return ( ADC->RESULT );          // returns the result of most recent ADC conversion
}

/* 1 second delay */
void delaySec( int sec )
{
    for (unsigned int x = 0; x < ( sec * 1000000 ); x++);
}

/* ***********************************************
 * Main function
 * ********************************************* */
__attribute__( ( used, section( ".boot.entry" ) ) ) int main( void )
{
    // Setup clocks (XOSC as source clk)
    setupClocks();
    // Reset Subsystems (IO / PADS)
    resetSubsys();
    // Config UART0 (9600 8N1)
    uartConfig();
    // Config LED
    ledConfig();
    // Config I2C0 (Master / Fast mode)
    adcConfig();

    uartTxStr( "\r\n\n-- RPi Pico Baremetal --\r\n\n" );
    uartTxStr( "Read the temperature sensor\r\n") ;

    unsigned int dataBuffer;
    while( 1 )
    {
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << 25 );     // XOR the LED pin
        uartTxStr( "\n\rADC: raw temperature value: ");
        dataBuffer = adcRead();
        uartPrintDW( dataBuffer );
        delaySec( 1 );
    }

    return ( 0 );
}

