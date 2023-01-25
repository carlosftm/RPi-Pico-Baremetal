#define PUT32(address,value)  (*((volatile unsigned int*)address))=value
#define GET32(address) *(volatile unsigned int*)address

int main(void) {

        PUT32( 0x4000f000, ( 1 << 5 ) );               // IO BANK

        while ( GET32(0x4000c008) & ( 1<< 5 ) == 0 );  // Reset Done?

        PUT32( 0x400140cc, 0x05 );                     // IO PAD = FUNC 5 (GPIO)
        PUT32( 0xd0000020, ( 1 << 25 ) );              // GPIO_OE

        while( 1 )
        {
                PUT32( 0xd000001c, ( 1 << 25 ) );      // XOR GPIO
                for ( volatile unsigned int a = 50000; a > 0; a-- );
        }

        return 0;
}
