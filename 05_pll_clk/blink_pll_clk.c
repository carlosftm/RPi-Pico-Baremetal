// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)

#define PUT32(address,value)  ( *( ( volatile unsigned int* )address ) ) = value
#define GET32(address) *( volatile unsigned int* ) address

#define XOR (0x1000)
#define SET (0x2000)
#define CLR (0x3000)

// Resets
#define RESETS_BASE                 0x4000C000UL
#define RESETS_RESET                (RESETS_BASE + 0x00)
#define RESETS_RESET_DONE           (RESETS_BASE + 0x08)

// IO Bank
#define IO_BANK0_BASE               0x40014000UL
#define IO_BANK0_GPIO25_CTRL        (IO_BANK0_BASE + 0xCC)

// SIO
#define SIO_BASE                    0xD0000000UL
#define SIO_GPIO_OUT_XOR            (SIO_BASE + 0x1c)
#define SIO_GPIO_OE                 (SIO_BASE + 0x20)

// XOSC ( 12MHz on-board crystal oscillator )
#define XOSC_BASE                   0x40024000UL
#define XOSC_CTRL                   (XOSC_BASE + 0x00)
#define XOSC_STATUS                 (XOSC_BASE + 0x04)
#define XOSC_STARTUP                (XOSC_BASE + 0x0C)

// Clocks
#define CLOCKS_BASE                 0x40008000UL
#define CLK_REF_CTRL                (CLOCKS_BASE + 0x30)
#define CLK_REF_DIV                 (CLOCKS_BASE + 0x34)
#define CLK_SYS_CTRL                (CLOCKS_BASE + 0x3C)
#define CLK_PERI_CTRL               (CLOCKS_BASE + 0x48)

// PLL_SYS
#define PLL_SYS_BASE                0x40028000UL
#define PLL_SYS_CS                  (PLL_SYS_BASE + 0x00)
#define PLL_SYS_POW                 (PLL_SYS_BASE + 0x04)
#define PLL_SYS_FBDIV               (PLL_SYS_BASE + 0x08)

/* Blink LED */
void blinkLed(void)
{
    for( int x = 15; x > 0; x-- )
    {
        PUT32( SIO_GPIO_OUT_XOR, ( 1 << 25 ) );            // XOR GPIO
        for ( volatile unsigned int a = 200000; a > 0; a-- );
    }
}

/**********************************
 * Main Function
 * ****************************** */
__attribute__((used, section(".boot.entry"))) int main(void)
{
    // Setup LED
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 5 ) );               // IO BANK
    while ( GET32( RESETS_RESET_DONE ) & ( 1<< 5 ) == 0 );     // Reset Done?
    PUT32( IO_BANK0_GPIO25_CTRL, 0x05 );                       // IO PAD = FUNC 5 (GPIO)
    PUT32( SIO_GPIO_OE, ( 1 << 25 ) );                         // GPIO_OE GPIO25

    // Blink using ring oscillator as source clock
    blinkLed();

    // Enable the XOSC
    PUT32( XOSC_CTRL, 0xAA0 );                                     // Frequency range: 1_15MHZ
    PUT32( XOSC_STARTUP,0xc4);                                     // Startup delay ( default value )
    PUT32( ( XOSC_CTRL | SET ), 0xFAB000 );                        // Enable ( magic word )
    while( !( GET32( XOSC_STATUS ) & 0x80000000 ) );               // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    PUT32( CLK_REF_CTRL, 2 );                                      // CLK REF source = xosc_clksrc
    PUT32( CLK_SYS_CTRL, 0 );                                      // CLK SYS source = clk_ref
    PUT32( CLK_REF_DIV, ( 1 << 8 ) );                              // CLK REF Divisor = 1
    PUT32( CLK_PERI_CTRL, ( 1 << 11 ) | ( 4 << 5 ) );              // CLK PERI Enable & AUX SRC = xosc_clksrc

    // Blink using XOSC as source clock
    blinkLed();

    // Reset PLL_SYS
    PUT32( ( RESETS_RESET | CLR ), ( 1 << 12 ) );                  // PLL_SYS
    while ( GET32( RESETS_RESET_DONE ) & ( 1<< 12 ) == 0 );        // Reset Done?

    // Set the PLL_SYS dividers and power up the VCO
    PUT32( PLL_SYS_FBDIV, 0xFF);                                   // FBDIV = 255
    PUT32( ( PLL_SYS_POW | CLR ), ( 1 << 5 ) | ( 1 << 0));                   // PWR = VCO Power Down + PLL Power Down
    while ( GET32( PLL_SYS_CS ) & ( 1<< 31 ) == 0 );               // VCO locked?

    PUT32( ( PLL_SYS_POW | CLR ), ( 1 << 3 ) );                    // Power Up PLL Post Div
    PUT32 ( ( CLK_SYS_CTRL ), ( 1 << 0 ) );                        // CLK_SYS to use clk_sys_aux -> pll_sys

    // Blink for ever using PLL as source clock
    while(1)
    {
        blinkLed();
    }

    return 0;
}
