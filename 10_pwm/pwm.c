// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"
#include <stdbool.h>

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

    RESETS_CLR->RESET_b.pwm = 1;
    while ( RESETS->RESET_DONE_b.pwm == 0 );
}

/* 5ms delay */
void delay5ms( void )
{
    for (unsigned int x = 0; x < 5000; x++);
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
    // Config LED
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 4;    // PWM function

    PWM->CH4_DIV_b.INT     = 0xFF;
    PWM->CH4_DIV_b.FRAC    = 1;
    PWM->CH4_CC_b.B        = 0xFF;
    PWM->CH4_TOP_b.CH4_TOP = 0xFF;
    PWM->CH4_CSR_b.EN      = 1;

    unsigned char count = 0xFF;
    while( 1 )
    {
        while( count > 0 )
        {
            PWM->CH4_CC_b.B = count--;
            delay5ms();
        }
        while( count < 0xFF )
        {
            PWM->CH4_CC_b.B = count++;
            delay5ms();
        }
    }

    return ( 0 );
}

