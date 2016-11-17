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
    TRISC3 = 1;                 // RC3/SCK: Serial Clock.
    TRISC4 = 1;                 // RC4/SDI: Serial Data Input.
    TRISC5 = 1;                 // RC5/SDO: Serial Data Output.
    TRISA5 = 0;                 // RA5/SS:  Slave Select
    TRISAbits.TRISA0 = 0;
                                // SS pin has to be physically pulled high by a resistor
}

void SPI_setup_slave() {

    SSPSTAT = 0b00000000;       // SMP, CKE.
//    SSPSTATbits.SMP = 0;        // Bit 7.
//    SSPSTATbits.CKE = 0;        // Bit 6.
    SSPCON = 0b00100101;        // SSPEN, SKP and SSPM3-0(SPI Master mode, clock = FOSC/4).
//    SSPCONbits.WCOL     = 0;    // Bit 7.
//    SSPCONbits.SSPOV    = 0;    // Bit 6.
//    SSPCONbits.SSPEN    = 1;    // Bit 5.
//    SSPCONbits.CKP      = 0;    // Bit 4.
//   SSPCONbits.SSPM     = 0001; // Bit 3 - 0 (SPI Master mode, clock = FOSC/4).
    PIR1bits.SSPIF = 0;         // Clear flag of SSP interruption

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

unsigned char SPI_read(){
    unsigned char data;
   // while(SSPSTATbits.BF == 0);     // Wait for Buffer to be full.
     data = SSPBUF;              // Load data with the SSPBUF.
     return data;
}

void SPI_enable_interruptions(){
    PIE1bits.SSPIE = 1;          // Enable Master Synchronous Serial Port (MSSP) interruptions 
    INTCONbits.PEIE = 1;        // Enable peripheral interruptions
    PIR1bits.SSPIF = 0;         // Clean MSSP Interrupt Flag bit
    INTCONbits.GIE = 1;         // Enable Global interruptions
    
}

void interrupt slave_read()
{ 
    RC1 = 1;
    if(PIR1bits.SSPIF == 1)
    {
       SSPCONbits.CKP = 0;          // Clock Polarity Select bit, Idle state for clock is a low level
       unsigned char z;
       if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))     
        // If overflow (SSPOV): new byte is received while SSPBUF still holds the previous data
        //  or collision (WCOL): A write to SSPBUF was attempted while the SPI conditions were not valid 
        //                       for a transmission to be started
       {
           
             z = SSPBUF;            // Read the previous value to clear the buffer
             SSPCONbits.SSPOV = 0;  // Clear the overflow flag
             SSPCONbits.WCOL = 0;   // Clear the collision bit
             SSPCONbits.CKP = 0;    // Clock Polarity Select bit, Idle state for clock is a high level
             PORTAbits.RA0 = z;
       }
      if(SSPSTATbits.BF != 0)
      {
         z = SPI_read();
         PORTAbits.RA0 = z;
         RC0 = 1;
          
      }
       
       SSPIF = 0;                   // Clean Master Synchronous Serial Port (MSSP) Interrupt Flag bit
       __delay_ms(100);
       
    }
}

void main() {
    
    //unsigned char [] z = 0;
    TRISC0 = 0;
    TRISC1 = 0;
    
    
    
    IO_setup();
    SPI_setup_slave();
    SPI_enable_interruptions();
 
            
    while(1){
    
    }
}