// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"

/* configures UART0 to 9600 8N1*/
void uartConfig( void )
{
    // Reset UART0
    RESETS_CLR->RESET_b.uart0 = 1;
    while ( RESETS->RESET_DONE_b.uart0 == 0 );

    UART0->UARTIBRD_b.BAUD_DIVINT = 78;
    UART0->UARTFBRD_b.BAUD_DIVFRAC = 8;
    UART0->UARTLCR_H = ( ( 1 << UART0_UARTLCR_H_FEN_Pos ) | ( 3 << UART0_UARTLCR_H_WLEN_Pos ) );
    UART0->UARTCR = ( ( 1 << UART0_UARTCR_RXE_Pos ) | ( 1 << UART0_UARTCR_TXE_Pos ) | ( 1 << UART0_UARTCR_UARTEN_Pos ) );

    // Set GPIO0 and 1 to function 2 (UART0)
    IO_BANK0->GPIO0_CTRL_b.FUNCSEL = 2;
    IO_BANK0->GPIO1_CTRL_b.FUNCSEL = 2;
}

/* UART receive character */
unsigned char uartRx( void )
{
    while ( UART0->UARTFR_b.RXFE != 0 );           // wait for RX FIFO to not be empty
    return( ( char )( UART0->UARTDR_b.DATA ) );    // Read the RX data
}

/* UART Send single character */
void uartTx( unsigned char x )
{
    while( UART0->UARTFR_b.TXFF != 0 );            // wait until TX FIFO is not full
    UART0->UARTDR_b.DATA = x;                      // Write the TX data
}

/* UART Send character string */
void uartTxStr( unsigned char *x )
{
    // Write the string of data until the NULL char is detected
    while( *x != '\0' )
    {
        uartTx( *x );
        *x++;
    }
}

/* Converts a byte in hex*/
unsigned char bin2hex(unsigned char input)
{
    unsigned char retVal = 'q';

    if ( ( input & 0x0F ) <= 9 )
    {
      retVal = ( input & 0x0F ) + '0';
    }
    else if ( ( input & 0x0F ) >= 0x0a )
    {
      retVal = ( ( input & 0x0F ) - 10 ) + 'a';
    }

    return retVal;
}

/* Prints a char (byte) in ASCII */
void uartByteToAscii( unsigned char data )
{
    uartTx( bin2hex( data >> 4 ));
    uartTx( bin2hex( data ));
}

/* Prints a char (byte) variable as string */
void uartPrintByte( unsigned char data )
{
    uartTxStr( "[0x" );
    uartByteToAscii( data );
    uartTxStr("]\n\r");

}

/* Prints a int (double word) variable as string */
void uartPrintDW( unsigned int data )
{
    uartTxStr( "[0x" );
    uartByteToAscii( ( char )( ( data & 0xFF000000 ) >> 24 ));
    uartByteToAscii( ( char )( ( data & 0x00FF0000 ) >> 16 ));
    uartByteToAscii( ( char )( ( data & 0x0000FF00 ) >>  8 ));
    uartByteToAscii( ( char )( ( data & 0x000000FF ) >>  0 ));
    uartTxStr("]\n\r");
}