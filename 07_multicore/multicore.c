// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#define PUT32(address, value) (*((volatile unsigned int *)address)) = value
#define GET32(address) *(volatile unsigned int *)address

#define XOR (0x1000)
#define SET (0x2000)
#define CLR (0x3000)

// Resets
#define RESETS_BASE 0x4000C000UL
#define RESETS_RESET (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE (RESETS_BASE + 0x08)

// IO Bank
#define IO_BANK0_BASE 0x40014000UL
#define IO_BANK0_GPIO00_CTRL (IO_BANK0_BASE + 0x04)
#define IO_BANK0_GPIO01_CTRL (IO_BANK0_BASE + 0x0C)
#define IO_BANK0_GPIO25_CTRL (IO_BANK0_BASE + 0xCC)

// SIO
#define SIO_BASE 0xD0000000UL
#define SIO_GPIO_OUT_XOR (SIO_BASE + 0x1c)
#define SIO_GPIO_OE (SIO_BASE + 0x20)
#define SIO_GPIO_OE_SET (SIO_BASE + 0x24)

// XOSC ( 12MHz on-board crystal oscillator )
#define XOSC_BASE 0x40024000UL
#define XOSC_CTRL (XOSC_BASE + 0x00)
#define XOSC_STATUS (XOSC_BASE + 0x04)
#define XOSC_STARTUP (XOSC_BASE + 0x0C)

// Clocks
#define CLOCKS_BASE 0x40008000UL
#define CLK_REF_CTRL (CLOCKS_BASE + 0x30)
#define CLK_REF_DIV (CLOCKS_BASE + 0x34)
#define CLK_SYS_CTRL (CLOCKS_BASE + 0x3C)
#define CLK_PERI_CTRL (CLOCKS_BASE + 0x48)

// UART0
#define UART0_BASE 0x40034000UL
#define UART0_UARTDR (UART0_BASE + 0x00)
#define UART0_UARTFR (UART0_BASE + 0x18)
#define UART0_UARTIBRD (UART0_BASE + 0x24)
#define UART0_UARTFBRD (UART0_BASE + 0x28)
#define UART0_UARTLCR_H (UART0_BASE + 0x2c)
#define UART0_UARTCR (UART0_BASE + 0x30)

// other defines
# define DELAY_1s (1200000)
# define DELAY_100ms (DELAY_1s/10)
# define DELAY_1ms (DELAY_1s/1000)

/* Handle unconfigured interrupts*/
void irqLoop( void )
{
    while ( 1 );
}

/* Handler of the Tick interrupt */
void irqSysTick( void )
{
    PUT32( SIO_GPIO_OUT_XOR, ( 1 << 25 ) );  // XOR the LED pin
}

/* Vector Table (Linker script has been upodated) */
__attribute__( ( used, section( ".vectors" ) ) ) void ( *vectors[] )( void ) =
{
    0,          //  0 stack pointer value (NA)
    irqLoop,    //  1 reset (NA)
    irqLoop,    //  2 NMI
    irqLoop,    //  3 hardFault
    0,          //  4 reserved
    0,          //  5 reserved
    0,          //  6 reserved
    0,          //  7 reserved
    0,          //  8 reserved
    0,          //  9 reserved
    0,          // 10 reserved
    irqLoop,    // 11 SVCall
    0,          // 12 reserved
    0,          // 13 reserved
    irqLoop,    // 14 pendSV
    irqSysTick, // 15 sysTick
};

/* Delay function*/
void delay(int ms)
{
    for( int z = 0; z < ms; z++ );                   // Delay
}

/* Setup XOSC and set it a source clock */
static void setupClocks(void)
{
    // Enable the XOSC
    PUT32( XOSC_CTRL, 0xAA0 );                       // Frequency range: 1_15MHZ
    PUT32( XOSC_STARTUP, 0xc4 );                     // Startup delay ( default value )
    PUT32( ( XOSC_CTRL | SET ), 0xFAB000 );          // Enable ( magic word )
    while( !( GET32( XOSC_STATUS ) & 0x80000000 ) ); // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    PUT32( CLK_REF_CTRL, 2 );                         // CLK REF source = xosc_clksrc
    PUT32( CLK_SYS_CTRL, 0 );                         // CLK SYS source = clk_ref
    PUT32( CLK_REF_DIV, ( 1 << 8 ) );                 // CLK REF Divisor = 1
    PUT32( CLK_PERI_CTRL, ( 1 << 11 ) | ( 4 << 5 ) ); // CLK PERI Enable & AUX SRC = xosc_clksrc
}

/* reset the subsystems used in this program */
static void resetSubsys()
{
    // Reset IO Bank
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 5 ) );
    while( GET32( RESETS_RESET_DONE ) & ( 1 << 5 ) == 0 );
    // Reset PADS BANK
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 8 ) );
    while( GET32( RESETS_RESET_DONE ) & (1 << 8) == 0 );
    // Reset UART0
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 22 ) );
    while( GET32( RESETS_RESET_DONE ) & ( 1 << 22 ) == 0 );
}

/* configures UART0 to 9600 8N1*/
static void configUart(void)
{
    PUT32( ( UART0_UARTIBRD ), 78 );                                 // Baud rate integer part
    PUT32( ( UART0_UARTFBRD ), 8 );                                  // Baud rate fractional part
    PUT32( ( UART0_UARTLCR_H ), ( 0x3 << 5 ) | ( 1 << 4 ) );         // Word len 8 + FIFO Enable
    PUT32( ( UART0_UARTCR ), ( 1 << 9 ) | ( 1 << 8 ) | ( 1 << 0 ) ); // UART Enable + TX and RX enable
}

/* Configures the GPIO functions for LED and UART*/
void configGpio( void )
{
    // Set GPIO0 and 1 to function 2 (UART0)
    PUT32( ( IO_BANK0_GPIO00_CTRL), 2 );
    PUT32( ( IO_BANK0_GPIO01_CTRL), 2 );
    // Set GPIO25 as SIO ( F5) and GPIO OE
    PUT32( ( IO_BANK0_GPIO25_CTRL), 5 );
    PUT32( SIO_GPIO_OE_SET, ( 1 << 25 ) );
}

/* UART receive character */
static unsigned char uartRx(void)
{
    while( ( GET32( ( UART0_UARTFR ) ) & ( 1 << 4 ) ) != 0 ); // wait for RX FIFO to not be empty
    return( ( char )( GET32( UART0_UARTDR ) ) );              // Read the RX data
}

/* UART Send single character */
static void uartTx( unsigned char x )
{
    while( ( GET32( ( UART0_UARTFR ) ) & ( 1 << 5 ) ) != 0 ); // wait until TX FIFO is not full
    PUT32( UART0_UARTDR, x );                                 // Write the TX data
}

/* UART Send character string */
static void uartTxStr( unsigned char *x )
{
    // Write the string of data until the NULL char is detected
    while( *x != '\0' )
    {
        uartTx( *x );
        *x++;
    }
}

/* Print an int variable in hex */
void printReg( int reg)
{
    unsigned char out;

    uartTxStr("0x");
    for ( int y = 7; y > -1; y-- )
    {
        out = (char)( ( reg >> ( 4 * y ) & 0x0000000F ) );
        if( out < 10)
        {
            out = out + '0';
        }
        else
        {
            out = ( out - 10 ) + 'A';
        }    
        uartTx( out );
    }
    uartTxStr( "\r\n" );
}


/* ***********************************************
 * Main function Core1
 * ********************************************* */
void mainCore1( void )
{
    int core0data = 0;
    int cpuId = GET32( 0xd0000000 );
    uartTxStr( "\r\nActive Core: " );
    printReg( cpuId );
    while( 1 )
    {
        while ( ( GET32( 0xd0000050 ) & 0x01 ) == 0 );  // loop until Rx FIFO is not empty
        core0data = GET32(0xd0000058);
        uartTxStr( "\r\nData from Core0 = " );
        uartTx( ( '0' + core0data ) );
        uartTxStr( "\r\n" );
    }
}


/* ***********************************************
 * Main function Core0
 * ********************************************* */
__attribute__( ( used, section( ".boot.entry" ) ) ) int main( void )
{
    setupClocks();
    resetSubsys();
    configUart();
    configGpio();

    uartTxStr( "[ Multicore Example ]\r\n\n" );

    // empty Rx FIFO for inter-core communication
    for(int loop = 0; loop < 2; loop++ )
    {
        while( ( GET32( 0xd0000050 ) & 0x01 ) == 1 )  // Loop until Rx FIFO is empty
        {
            GET32( ( 0xd0000000 + 0x58 ) );
        }
        asm( "sev" );
    }

    // According to specs these are the values that need to be sent to enable Core1.
    // Both Cores moves trough a state machine by exaching messages over the
    // inter-processor FIFOS
    PUT32( ( 0xd0000000 + 0x54 ), 0 );
    asm( "sev" );
    GET32( (0xd0000000 + 0x58 ) );

    PUT32( ( 0xd0000000 + 0x54 ), 0 );
    asm( "sev" );
    GET32( (0xd0000000 + 0x58 ) );

    PUT32( ( 0xd0000000 + 0x54 ), 1 );
    GET32( (0xd0000000 + 0x58 ) );

    PUT32( ( 0xd0000000 + 0x54 ), 0X20000100 );      // Vector Table
    GET32( (0xd0000000 + 0x58 ) );
    
    PUT32( ( 0xd0000000 + 0x54 ), 0X20003000 );      // Core1 stack pointer
    GET32( (0xd0000000 + 0x58 ) );

    PUT32( ( 0xd0000000 + 0x54 ), (int)mainCore1 );  // Core1 main function
    GET32( (0xd0000000 + 0x58 ) );

    asm( "sev" );

    // At this point Core1 must be active. Core0 will toggle the LED and send a
    // sequential number to Core1 over the FIFO.
    while( 1 )
    {
        for(int tmp = 0; tmp < 10; tmp++)
        {
            PUT32( SIO_GPIO_OUT_XOR, ( 1 << 25 ) );              // XOR the LED pin

            delay( 5 * DELAY_100ms );

            while( ( GET32( 0xd0000050 ) & ( 1 << 1 ) == 0 ) );  // Loop until FIFO is not full

            PUT32( ( 0xd0000000 + 0x54 ), tmp );                 // write the counter value in the FIFO
        }
    }

    return ( 0 );
}
