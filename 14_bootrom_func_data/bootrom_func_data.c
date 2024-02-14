// Copyright (c) 2024 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"
#include <stdbool.h>
#include "uart.h"

/* From the Pico SDK */
#define rom_hword_as_ptr(rom_address) (void *)(uintptr_t)(*(uint16_t *)(uintptr_t)(rom_address))

/* ROM content address*/
#define ROM_TABLE_LOOKUP 0x00000018
#define ROM_DATA_TABLE   0x00000016
#define ROM_FUNC_TABLE   0x00000014

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

    RESETS_CLR->RESET_b.i2c0 = 1;
    while ( RESETS->RESET_DONE_b.i2c0 == 0 );
}

/* configure LED */
void ledConfig( void )
{
    // Set GPIO25 as SIO (F5) and GPIO OE
    IO_BANK0->GPIO25_CTRL_b.FUNCSEL = 5;
    SIO->GPIO_OE_SET_b.GPIO_OE_SET = ( 1 << 25 );
}

/* 1 second delay */
void delaySec( int sec )
{
    for (unsigned int x = 0; x < ( sec * 1000000 ); x++);
}

/* generate the code for the data or function lookup table*/
uint32_t genCode( char c1, char c2 ) {
    return ( ( c2 << 8 ) | c1 );
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

    uartTxStr( "\r\n\n-- RPi Pico Baremetal --\r\n\n" );
    uartTxStr( "Read Bootrom data and execute functions\r\n\n\n") ;

    /* 1. Read and print the copyright string */
    volatile int* pRomTableLookup = ROM_TABLE_LOOKUP;                                       // pointer to rom_table_lookup() function
    uint32_t ( *romTableLookup )( uint16_t* dataTable, uint32_t code ) = *pRomTableLookup;  // function pointer assignment
    uint16_t *pDataTable = ( uint16_t* ) rom_hword_as_ptr( ROM_DATA_TABLE );                // pointer to the rom_data_table
    int code = genCode('C', 'R');                                                           // generate the code for the copyright_string
    char* copyrightString = (*romTableLookup)(pDataTable, code);                            // get the pointer to the copyright_string

    uartTxStr( "Copyright string from bootrom:\r\n") ;
    while(*copyrightString != 0)
    {
        uartTx(*copyrightString);
        copyrightString++;
    }
    uartTxStr("\r\n");

    /* 2. Call the bootrom popcount32() function */
    uint16_t *pFuncTable = ( uint16_t* ) rom_hword_as_ptr( ROM_FUNC_TABLE );                // pointer to the rom_func_table
    code = genCode('P', '3');                                                               // generate the code for the popcount32() function
    uint32_t* pFunction = ( *romTableLookup )( pFuncTable, code );                          // get pointer to the popcount32() function
    uint32_t ( *popCount32 )( uint32_t value ) = pFunction;                                 // function pointer assignment
    uint32_t result1s = ( *popCount32 )( 0xFF000001 );                                      // call popcount32() function

    uartTxStr( "\n\rCalculate number of binary 1 on 0xFF00001: \r\n");
    uartPrintDW( result1s );

    /* 3. Call the bootrom memset() function to initialize a 64byte memory block */
    unsigned char charArray[64];
    *pFuncTable = ( uint16_t* ) rom_hword_as_ptr( ROM_FUNC_TABLE );                         // pointer to the rom_func_table
    code = genCode('M', 'S');                                                               // generate the code for the memset() function
    pFunction = ( *romTableLookup )( pFuncTable, code );                                    // get pointer to the memset() function
    uint8_t (*_memset)(uint8_t *ptr, uint8_t c, uint32_t n) = pFunction;                    // function pointer assignment

    unsigned char initChar = 'a';
    while( initChar < 'd' )
    {
        (*_memset)(&charArray[0], initChar++, 64);                                          // call memset() function
        for ( int cnt = 0; cnt < 64; cnt++ )
        {
            uartTx(charArray[cnt]);
        }
        uartTxStr( "\n\r");
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << 25 );     // XOR the LED pin
        delaySec( 1 );
    }

    return ( 0 );
}

