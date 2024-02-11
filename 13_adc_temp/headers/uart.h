// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#ifndef uart0
#define uart0

void uartConfig( void );
unsigned char uartRx( void );
void uartTx( unsigned char x );
void uartTxStr( unsigned char *x );
void uartPrintByte( unsigned char data );
void uartPrintDW( unsigned int data );
unsigned char uartRxNoBlocking( void );

#endif