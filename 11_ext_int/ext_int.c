// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#include "RP2040.h"

#define IO_IRQ_BANK0       13
#define GPIO_BUILT_IN_LED  25

/* function declaration */
void irqiobank0( void );

/* Vector Table (Linker script has been upodated) */
__attribute__( ( used, section( ".vectors" ) ) ) void ( *vectors[] )( void ) =
{
    0,          //  0 stack pointer value (NA)
    0,          //  1 reset (NA)
    0,          //  2 NMI
    0,          //  3 hardFault
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
    0,          // 00 external Int
    0,          // 01 external Int
    0,          // 02 external Int
    0,          // 03 external Int
    0,          // 04 external Int
    0,          // 05 external Int
    0,          // 06 external Int
    0,          // 07 external Int
    0,          // 08 external Int
    0,          // 09 external Int
    0,          // 10 external Int
    0,          // 11 external Int
    0,          // 12 external Int
    irqiobank0, // 13 external Int
};

/* function definition */
void irqiobank0( void )
{
    if ( IO_BANK0->INTR1_b.GPIO15_EDGE_LOW == 1 )                        // check if the raw interrupt register for GPIO15 endge low int.
    {
        IO_BANK0->INTR1_b.GPIO15_EDGE_LOW = 1;                           // Clear by writing raw interrupt
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << GPIO_BUILT_IN_LED );   // XOR the LED pin  // XOR the LED pin
        PPB->NVIC_ICPR_b.CLRPEND = ( 1 << IO_IRQ_BANK0 );                    // Clear pending
    }
}

/* Setup XOSC and set it a source clock */
static void setupClocks( void )
{
    // Enable the XOSC
    XOSC->CTRL            = 0xAA0;          // Frequency range: 1_15MHZ
    XOSC->STARTUP_b.DELAY = 0xC4;           // Startup delay ( default value )
    XOSC_SET->CTRL        = 0xFAB000;       // Enable ( magic word )
    while( !( XOSC->STATUS_b.STABLE & 1 ) ); // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    CLOCKS->CLK_REF_CTRL_b.SRC = 2;         // CLK REF source = xosc_clksrc
    CLOCKS->CLK_SYS_CTRL_b.SRC = 0;         // CLK SYS source = clk_ref
    CLOCKS->CLK_REF_DIV_b.INT  = 1;         // CLK REF Divisor = 1
    CLOCKS->CLK_PERI_CTRL_b.AUXSRC = 4;     // CLK PERI AUX SRC = xosc_clksrc
    CLOCKS->CLK_PERI_CTRL_b.ENABLE = 1;     // CLK PERI Enable
}

/* reset the subsystems used in this program */
static void resetSubsys()
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

    // Set GPIO25 as SIO (F5) and GPIO OE
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 5;
    SIO->GPIO_OE_SET_b.GPIO_OE_SET = ( 1 << 25 );

    IO_BANK0->GPIO15_CTRL_b.FUNCSEL = 5;                                     // Set GPIO15 as SIO (F5)

                                                                             // GPIO15 pad configuration:
    PADS_BANK0->GPIO15 = ( ( 1 << PADS_BANK0_GPIO15_OD_Pos ) |               //  Output disabled
                           ( 1 << PADS_BANK0_GPIO15_IE_Pos ) |               //  Input enabled
                           ( 1 << PADS_BANK0_GPIO15_PUE_Pos )|               //  Pull up
                           ( 1 << PADS_BANK0_GPIO15_SCHMITT_Pos ) );         //  Schmitt trigger

    IO_BANK0->INTR1_b.GPIO15_EDGE_LOW = 1;               // write to clear raw interrupt: edge-low
    IO_BANK0->PROC0_INTE1_b.GPIO15_EDGE_LOW = 1;         // interrupt enabled: edge-low
    PPB->NVIC_ICPR_b.CLRPEND = ( 1 << IO_IRQ_BANK0 );    // Interrupt Clear-Pending 
    PPB->NVIC_ISER_b.SETENA  = ( 1 << IO_IRQ_BANK0 );    // Interrupt Set-Enable 

    while ( 1 )
    {
        // loop here and wait for the tick interrupt
    }

    return ( 0 );
}
