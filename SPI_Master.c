#include <xc.h>
// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#define _XTAL_FREQ 4000000

#define SCK RC3
#define SDI RC4
#define SDO RC5
//TODO #define ENABLE  ??

void IO_setup() {
    TRISC3 = 0;                 // RC3/SCK: Serial Clock.
    TRISC4 = 1;                 // RC4/SDI: Serial Data Input.
    TRISC5 = 0;                 // RC5/SDO: Serial Data Output.
    TRISA5 = 0;                 // RA5/SS:  Slave Select
                                // SS pin has to be physically pulled high by a resistor
}

void SPI_setup() {
    SSPCONbits.SSPEN = 0;   

    SSPSTAT = 0b01000000;       // SMP, CKE.
    SSPCON = 0b00110010;        // SSPEN, SKP and SSPM3-0(SPI Master mode, clock = FOSC/4).
    PIR1bits.SSPIF = 0;         // Clear flag of SSP interruption
    SSPCONbits.SSPEN = 1;

}  

void SPI_reset() {
unsigned char z;
        SSPCONbits.SSPEN = 0;   // Reset SPI module
        SSPCONbits.SSPEN = 1;   // Reset SPI module
        z = SSPBUF;             // Read data from SSPBUF
        //?? BF = 0;                  // Set buffer as empty
        PIR1bits.SSPIF = 0;     // Clear flag of SSP interruption
        SSPCONbits.SSPEN = 0;   // Reset SPI module
        SSPCONbits.SSPEN = 1;   // Reset SPI module
}

void SPI_send(unsigned char data){
    SSPSTATbits.BF = 0;
    SSPBUF = data;              // Load SSPBUF with the transmitting data.
    while(!SSPSTATbits.BF);     // Wait for Buffer to be full.
}

void SPI_enable_interruptions(){
    PIE1bits.SSPIE =1;          // Enable Master Synchronous Serial Port (MSSP) interruptions 
    INTCONbits.PEIE = 1;        // Enable peripheral interruptions
    PIR1bits.SSPIF = 0;         // Clean MSSP Interrupt Flag bit
}

void main() {

    unsigned char data = 255;

    SPI_enable_interruptions();
    
    IO_setup();
            
    SPI_setup();
    
    while(1){
        SPI_send(data);
    }
}