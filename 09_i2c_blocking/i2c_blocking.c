// Copyright (c) 2023 CarlosFTM
// This code is licensed under MIT license (see LICENSE.txt for details)
#include "RP2040.h"
#include <stdbool.h>
#include "uart.h"

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

/* configure i2c0 master */
static void I2c0Config( void )
{
    // Disable I2C
    I2C0->IC_ENABLE = 0;
    // Config Master mode at Fast speed and 7-bit addressing
    I2C0->IC_CON_b.MASTER_MODE = I2C0_IC_CON_MASTER_MODE_ENABLED;
    I2C0->IC_CON_b.SPEED = I2C0_IC_CON_SPEED_FAST;
    I2C0->IC_CON_b.IC_10BITADDR_SLAVE = I2C0_IC_CON_IC_10BITADDR_MASTER_ADDR_7BITS;
    I2C0->IC_CON_b.IC_10BITADDR_MASTER = I2C0_IC_CON_IC_10BITADDR_SLAVE_ADDR_7BITS;
    I2C0->IC_CON_b.IC_RESTART_EN = I2C0_IC_CON_IC_RESTART_EN_DISABLED;
    I2C0->IC_CON_b.IC_SLAVE_DISABLE = I2C0_IC_CON_IC_SLAVE_DISABLE_SLAVE_DISABLED;
    I2C0->IC_CON_b.STOP_DET_IFADDRESSED = I2C0_IC_CON_STOP_DET_IFADDRESSED_DISABLED;
    I2C0->IC_CON_b.TX_EMPTY_CTRL = I2C0_IC_CON_TX_EMPTY_CTRL_ENABLED;
    I2C0->IC_CON_b.RX_FIFO_FULL_HLD_CTRL = I2C0_IC_CON_RX_FIFO_FULL_HLD_CTRL_DISABLED;
    // Config High and Low counts: From RP2040 Datasheet (Fast mode (400kbps) with ref clk @ 12MHZ)
    I2C0->IC_FS_SCL_HCNT = 71;
    I2C0->IC_FS_SCL_LCNT = 16;
    I2C0->IC_FS_SPKLEN   = 1;
    I2C0->IC_SDA_HOLD    = 0;

    // Set GPIO0 and 1 to function 3 (I2C0)
    IO_BANK0->GPIO21_CTRL_b.FUNCSEL = 3;
    IO_BANK0->GPIO20_CTRL_b.FUNCSEL = 3;
    IO_BANK0->GPIO21_CTRL_b.FUNCSEL = 3;
    IO_BANK0->GPIO20_CTRL_b.FUNCSEL = 3;
    // Pull Up Enabled (I2C)
    PADS_BANK0->GPIO21_b.PUE = 1;
    PADS_BANK0->GPIO20_b.PUE = 1;

    // Enable I2C
    I2C0->IC_ENABLE = 1;
}

/* read i2c0 (1 Byte) */
void i2cRead( uint8_t slvAdd, uint8_t regAdd, uint8_t* data, uint32_t len )
{
    (void)len;  // ignore it for now.
    I2C0->IC_ENABLE = 0;
    I2C0->IC_TAR    = slvAdd;
    I2C0->IC_ENABLE = 1;

    I2C0->IC_DATA_CMD = ( ( I2C0_IC_DATA_CMD_RESTART_DISABLE << I2C0_IC_DATA_CMD_RESTART_Pos ) |
                          ( I2C0_IC_DATA_CMD_STOP_ENABLE << I2C0_IC_DATA_CMD_STOP_Pos ) |
                          ( I2C0_IC_DATA_CMD_CMD_WRITE << I2C0_IC_DATA_CMD_CMD_Pos ) |
                          regAdd );

    while ( I2C0->IC_RAW_INTR_STAT_b.TX_EMPTY != I2C0_IC_INTR_STAT_R_TX_EMPTY_ACTIVE );
    while ( I2C0->IC_RAW_INTR_STAT_b.STOP_DET != I2C0_IC_INTR_STAT_R_STOP_DET_ACTIVE );

    I2C0->IC_DATA_CMD = ( ( I2C0_IC_DATA_CMD_RESTART_DISABLE << I2C0_IC_DATA_CMD_RESTART_Pos ) |
                          ( I2C0_IC_DATA_CMD_STOP_ENABLE << I2C0_IC_DATA_CMD_STOP_Pos ) |
                          ( I2C0_IC_DATA_CMD_CMD_READ << I2C0_IC_DATA_CMD_CMD_Pos ) );

    while ( !( I2C0->IC_RXFLR) );

    data[0] = I2C0->IC_DATA_CMD_b.DAT;
}

/* write i2c0 registers */
void i2cWrite( uint8_t slvAdd, uint8_t* data, uint8_t len )
{
    I2C0->IC_ENABLE = 0;
    I2C0->IC_TAR    = slvAdd;
    I2C0->IC_ENABLE = 1;

    for (uint8_t cnt = 0; cnt < len; cnt++ )
    {

    bool lastByte     = ( ( len - cnt ) == 1 ? true : false );
    I2C0->IC_DATA_CMD = ( ( I2C0_IC_DATA_CMD_RESTART_DISABLE << I2C0_IC_DATA_CMD_RESTART_Pos ) |
                          ( ( lastByte ? I2C0_IC_DATA_CMD_STOP_ENABLE : I2C0_IC_DATA_CMD_STOP_DISABLE ) << I2C0_IC_DATA_CMD_STOP_Pos ) |
                          *data++ );
    while ( I2C0->IC_RAW_INTR_STAT_b.TX_EMPTY != I2C0_IC_INTR_STAT_R_TX_EMPTY_ACTIVE );
    }
}

/* 1 second delay */
void delay1Sec( void )
{
    for (unsigned int x = 0; x < 1000000; x++);
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
    // Config I2C0 (Master / Fast mode)
    I2c0Config();

    uartTxStr( "\r\n\n-- RPi Pico Baremetal --\r\n\n" );

    // Config BMP280 Sensor with basic cfg data
    unsigned  char devSlvAdd    = 0x76;               // BMP280 sensor i2c slave address

    unsigned char dataBuffer[3] = { 0xf4,             // data buffer with basic configuration
                                    0x27,
                                    0x00 };
 
    i2cWrite( devSlvAdd, &dataBuffer[0], 3 );         // Write the BMP280 configuration
    
    i2cRead( devSlvAdd, 0xd0, &dataBuffer[0], 1 );    // Read BMP280 device ID register
    
    uartTxStr( "BMP280 Device Id: \r\n") ;
    uartPrintByte( dataBuffer[0] );

    while( 1 )
    {
        SIO->GPIO_OUT_XOR_b.GPIO_OUT_XOR = ( 1 << 25 );     // XOR the LED pin
        i2cRead( devSlvAdd, 0xfa, &dataBuffer[0], 1 );      // Read BMP280 temperature MSB register
        i2cRead( devSlvAdd, 0xfb, &dataBuffer[1], 1 );      // Read BMP280 temperature LSB register
        uartTxStr( "\n\rI2C: read raw temperature: \r\n");
        uartPrintByte( dataBuffer[0] );
        uartPrintByte( dataBuffer[1] );
        delay1Sec();
    }

    return ( 0 );
}

