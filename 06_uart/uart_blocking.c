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

/* Setup XOSC and set it a source clock */
static void setupClocks(void)
{
    // Enable the XOSC
    PUT32(XOSC_CTRL, 0xAA0);            // Frequency range: 1_15MHZ
    PUT32(XOSC_STARTUP, 0xc4);          // Startup delay ( default value )
    PUT32((XOSC_CTRL | SET), 0xFAB000); // Enable ( magic word )
    while (!(GET32(XOSC_STATUS) & 0x80000000)); // Oscillator is running and stable

    // Set the XOSC as source clock for REF, SYS and Periferals
    PUT32(CLK_REF_CTRL, 2);                     // CLK REF source = xosc_clksrc
    PUT32(CLK_SYS_CTRL, 0);                     // CLK SYS source = clk_ref
    PUT32(CLK_REF_DIV, (1 << 8));               // CLK REF Divisor = 1
    PUT32(CLK_PERI_CTRL, (1 << 11) | (4 << 5)); // CLK PERI Enable & AUX SRC = xosc_clksrc
}

/* reset the subsystems used in this program */
static void resetSubsys()
{
    // Reset IO Bank
    PUT32((RESETS_RESET | CLR), (1 << 5));
    while (GET32(RESETS_RESET_DONE) & (1 << 5) == 0)
        ;
    // Reset PADS BANK
    PUT32((RESETS_RESET | CLR), (1 << 8));
    while (GET32(RESETS_RESET_DONE) & (1 << 8) == 0)
        ;
    // Reset UART0
    PUT32((RESETS_RESET | CLR), (1 << 22));
    while (GET32(RESETS_RESET_DONE) & (1 << 22) == 0)
        ;
}

/* configures UART0 to 9600 8N1*/
static void configUart(void)
{
    PUT32((UART0_UARTIBRD), 78);                           // Baud rate integer part
    PUT32((UART0_UARTFBRD), 8);                            // Baud rate fractional part
    PUT32((UART0_UARTLCR_H), (0x3 << 5) | (1 << 4));       // Word len 8 + FIFO Enable
    PUT32((UART0_UARTCR), (1 << 9) | (1 << 8) | (1 << 0)); // UART Enable + TX and RX enable
}

/* UART receive character */
static unsigned char uartRx(void)
{
    while ((GET32((UART0_UARTFR)) & (1 << 4)) != 0)
        ;                               // wait for RX FIFO to not be empty
    return (char)(GET32(UART0_UARTDR)); // Read the RX data
}

/* UART Send single character */
static void uartTx(unsigned char x)
{
    while ((GET32((UART0_UARTFR)) & (1 << 5)) != 0)
        ;                   // wait until TX FIFO is not full
    PUT32(UART0_UARTDR, x); // Write the TX data
}

/* UART Send character string */
static void uartTxStr(unsigned char *x)
{
    // Write the string of data until the NULL char is detected
    while (*x != '\0')
    {
        uartTx(*x);
        *x++;
    }
}

/* ***********************************************
 * Main function
 * ********************************************* */
__attribute__((used, section(".boot.entry"))) int main(void)
{
    // Setup clocks (XOSC as source clk)
    setupClocks();
    // Reset Subsystems (IO / PADS and UART0)
    resetSubsys();
    // Config UART0 (9600 8N1)
    configUart();

    // Set GPIO0 and 1 to function 2 (UART0)
    PUT32((IO_BANK0_GPIO00_CTRL), 2);
    PUT32((IO_BANK0_GPIO01_CTRL), 2);
    // Set GPIO25 as SIO ( F5) and GPIO OE
    PUT32((IO_BANK0_GPIO25_CTRL), 5);
    PUT32(SIO_GPIO_OE_SET, (1 << 25));

    char *welcomeMsg = "\r\n\n"
                       "888   |           888 888                Y88b         /                 888       888\r\n"
                       "888___|  e88~~8e  888 888  e88~-_         Y88b       /   e88~-_  888-~_ 888  e88~-888\r\n"
                       "888   | d888  88b 888 888 d888   i         Y88b  e  /   d888   i 888    888 d888  888\r\n"
                       "888   | 8888__888 888 888 8888   |          Y88bd8b/    8888   | 888    888 8888  888\r\n"
                       "888   | Y888    , 888 888 Y888   '           Y88Y8Y     Y888   ' 888    888 Y888  888\r\n"
                       "888   |  `88___/  888 888  `88_-~             Y  Y       `88_-~  888    888  `88_/888\r\n\n";

    uartTxStr(welcomeMsg);

    while (1)
    {
        for (char cnt = 0; cnt < 20; cnt++)
        {
            uartTx(('0' + cnt));
            PUT32(SIO_GPIO_OUT_XOR, (1 << 25)); // XOR the LED pin
        }
        uartTxStr(" --> ");
        uartTx(uartRx()); // Wait for inoput (bloking function)
        uartTxStr("\r\n");
    }

    return (0);
}
