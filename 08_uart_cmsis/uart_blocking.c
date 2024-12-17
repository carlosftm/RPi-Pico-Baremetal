// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"

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

    // Reset UART0
    RESETS_CLR->RESET_b.uart0 = 1;
    while ( RESETS->RESET_DONE_b.uart0 == 0 );
}

/* configures UART0 to 9600 8N1*/
static void configUart(void)
{
    UART0->UARTIBRD_b.BAUD_DIVINT = 78;
    UART0->UARTFBRD_b.BAUD_DIVFRAC = 8;
    UART0->UARTLCR_H = ( ( 1 << UART0_UARTLCR_H_FEN_Pos ) | ( 3 << UART0_UARTLCR_H_WLEN_Pos ) );
    UART0->UARTCR = ( ( 1 << UART0_UARTCR_RXE_Pos ) | ( 1 << UART0_UARTCR_TXE_Pos ) | ( 1 << UART0_UARTCR_UARTEN_Pos ) );
}

/* UART receive character */
static unsigned char uartRx(void)
{
    while ( UART0->UARTFR_b.RXFE != 0 );           // wait for RX FIFO to not be empty
    return( ( char )( UART0->UARTDR_b.DATA ) );    // Read the RX data
}

/* UART Send single character */
static void uartTx( unsigned char x )
{
    while( UART0->UARTFR_b.TXFF != 0 );            // wait until TX FIFO is not full
    UART0->UARTDR_b.DATA = x;                      // Write the TX data
}

/* UART Send character string */
static void uartTxStr( unsigned char *x )
{
    // Write the string of data until the NULL char is detected
    while( *x != '\0' )
    {
        uartTx( *x );
        x++;
    }
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
    // Config UART0 (9600 8N1)
    configUart();

    // Set GPIO25 as SIO ( F5) and GPIO OE
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 5;
    SIO->GPIO_OE_SET_b.GPIO_OE_SET = ( 1 << 25 );

    // Set GPIO0 and 1 to function 2 (UART0)
    IO_BANK0->GPIO0_CTRL_b.FUNCSEL = 2;
    IO_BANK0->GPIO1_CTRL_b.FUNCSEL = 2;


    char *welcomeMsg = "\r\n\n"
                       "888   |           888 888                Y88b         /                 888       888\r\n"
                       "888___|  e88~~8e  888 888  e88~-_         Y88b       /   e88~-_  888-~_ 888  e88~-888\r\n"
                       "888   | d888  88b 888 888 d888   i         Y88b  e  /   d888   i 888    888 d888  888\r\n"
                       "888   | 8888__888 888 888 8888   |          Y88bd8b/    8888   | 888    888 8888  888\r\n"
                       "888   | Y888    , 888 888 Y888   '           Y88Y8Y     Y888   ' 888    888 Y888  888\r\n"
                       "888   |  `88___/  888 888  `88_-~             Y  Y       `88_-~  888    888  `88_/888\r\n\n";

    uartTxStr( welcomeMsg );

    while( 1 )
    {
        for( char cnt = 0; cnt < 20; cnt++ )
        {
            uartTx( ( '0' + cnt ) );
            SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << 25 );   // XOR the LED pin
        }
        uartTxStr( " --> " );
        uartTx( uartRx() );                         // Wait for inoput (bloking function)
        uartTxStr( "\r\n" );
    }

    return ( 0 );
}
