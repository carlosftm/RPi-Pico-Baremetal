// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"
#include "uart.h"

#define GPIO_BUILT_IN_LED    (25)
#define UART0_IRQ            (20)

/* Handles UART0 interrupt */
void irqUart0( void )
{
        PPB->NVIC_ICER_b.CLRENA  = ( 1 << UART0_IRQ );                  // Interrupt disabled
        SIO->GPIO_OUT_SET_b.GPIO_OUT_SET = ( 1 << GPIO_BUILT_IN_LED );  // Toggle LED
        uartTx( uartRx() );                                             // TX the RX buffer
        PPB->NVIC_ICPR_b.CLRPEND = ( 1 << UART0_IRQ );                  // Interrupt Clear-Pending
        PPB->NVIC_ISER_b.SETENA  = ( 1 << UART0_IRQ );                  // Interrupt enable 
}

/* Handles unwanted interrupts */
void loopIrq( void )
{
        for ( volatile unsigned int x = 0; x < 300000; x++ );
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << GPIO_BUILT_IN_LED );
}

/* Handles hardfault interrupt */
void hardFault( void )
{
        for (volatile unsigned int x = 0; x < 150000; x++);
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << GPIO_BUILT_IN_LED );
}

/* Vector Table (Linker script has been upodated) */
__attribute__( ( used, section( ".vectors" ) ) ) void ( *vectors[] )( void ) =
{
    0,          //  0 stack pointer value (NA)
    0,          //  1 reset (NA)
    0,          //  2 NMI
    hardFault,  //  3 hardFault
    0,          //  4 reserved
    0,          //  5 reserved
    0,          //  6 reserved
    0,          //  7 reserved
    0,          //  8 reserved
    0,          //  9 reserved
    0,          // 10 reserved
    0,          // 11 SVCall
    0,          // 12 reserved
    0,          // 13 reserved
    0,          // 14 pendSV
    0,          // 15 sysTick
    loopIrq,          //  0 external Int
    loopIrq,          //  1 external Int
    loopIrq,          //  2 external Int
    loopIrq,          //  3 external Int
    loopIrq,          //  4 external Int
    loopIrq,          //  5 external Int
    loopIrq,          //  6 external Int
    loopIrq,          //  7 external Int
    loopIrq,          //  8 external Int
    loopIrq,          //  9 external Int
    loopIrq,          // 10 external Int
    loopIrq,          // 11 external Int
    loopIrq,          // 12 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    loopIrq,          // 13 external Int
    irqUart0,         // 20 external interrupt (UART0)
};

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
void resetSubsys( void )
{
    // Reset IO Bank
    RESETS_CLR->RESET_b.io_bank0 = 1;
    while ( RESETS->RESET_DONE_b.io_bank0 == 0 );

    // Reset PADS BANK
    RESETS_CLR->RESET_b.pads_bank0 = 1;
    while ( RESETS->RESET_DONE_b.pads_bank0 == 0 );
}

/* ***********************************************
 * Main function
 * ********************************************* */
__attribute__( ( used, section( ".boot.entry" ) ) ) int main( void )
{
    // Setup clocks (XOSC as source clk)
    setupClocks();
    // Reset Subsystems (IO / PADS and UART0)
    resetSubsys();

    // Enable UART= interrupt on NVIC
    PPB->NVIC_ICPR_b.CLRPEND = ( 1 << UART0_IRQ );    // Interrupt Clear-Pending 
    PPB->NVIC_ISER_b.SETENA  = ( 1 << UART0_IRQ );    // Interrupt Set-Enable 
    // Config UART0 (9600 8N1)
    uartConfig();

    // Set GPIO25 as SIO ( F5) and GPIO OE
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 5;
    SIO->GPIO_OE_SET_b.GPIO_OE_SET = ( 1 << GPIO_BUILT_IN_LED );

    uartTxStr( "\r\n\n UART Not Blocking \r\n\n" );

    while( 1 )
    {
        SIO->GPIO_OUT_CLR_b.GPIO_OUT_CLR = ( 1 << GPIO_BUILT_IN_LED );
        for ( volatile unsigned int x = 0; x < 200000; x++ );
    }

    return ( 0 );
}
