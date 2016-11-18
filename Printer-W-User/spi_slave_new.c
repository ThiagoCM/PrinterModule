/*
 * File:   main.c
 * Author: Thiago, Carol, Tellicio, Isadora
 *
 * Created on 2 de Novembro de 2016, 16:24
 */

// SETUP

#pragma config FOSC = INTRC_CLKOUT// Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR pin function select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)
// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// Libraries and PIC frequency
#include <stdio.h>
#include <xc.h>
#define _XTAL_FREQ 4000000

// Defining clock, input and output
#define SCK RC3
#define SDI RC4
#define SDO RC5

// Defining a global variable
unsigned char z = 'b';

// ========================= SPI_SLAVE SETUP =========================


void IO_setup() {
    TRISC3 = 1;                 // RC3/SCK: Serial Clock.
    TRISC4 = 1;                 // RC4/SDI: Serial Data Input.
    TRISC5 = 1;                 // RC5/SDO: Serial Data Output.
    TRISA5 = 0;                 // RA5/SS:  Slave Select
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

// ======================== USART ========================


void USART_init(void)
{
    // p154 Asynchronous Transmission Set-up
    // 1 -  SPBRGH, SPBRG register pa SPBRGH, SPBRG register pair
    // Set BRGH to 1 so we have a fixed value for SPBRG using 4MHz on the oscillator.
    TXSTAbits.BRGH = 1;
    SPBRGH = 0;
    SPBRG = 12; 
    
    // 2 - Set SPEN bit and clear SYNC for asynchronous operation
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;     
    
    // 3 - Disable 9-bit reception
    TXSTAbits.TX9 = 0;
    
    // 4 - Enable the transmission
    TXSTAbits.TXEN = 1;
    
    // 5 - Enable interruptions
    PIE1bits.TXIE = 0;      
    INTCONbits.GIE = 0;
    INTCONbits.PEIE = 0;
    
    // 6 - Clear RCIF
    PIR1bits.RCIF = 0;

    
    // 7 - Load 8-bit data into TXREG to transmit 
}   

void USART_putc(unsigned char c)
{
    while (!PIR1bits.TXIF );    // wait until transmit shift register is empty
        TXREG = c;               // write character to TXREG and start transmission
}

void USART_puts(unsigned char *s)
{
    int i;
    for(i=0;s[i]!='\0';i++)
        USART_putc(s[i]);
}

// We're not setting up configurations for the printer. 
// For some reason, the printer is working fine with the PIC without using any pre-settable parameters.
// The PRINTER_init clears the printer buffer.


// ======================== PRINTER ========================
void PRINTER_init(){
    
    USART_putc(27);
    USART_putc(64); // Initialize the printer (the print buffer is cleared, etc)
    __delay_ms(1500); // Setting delay to be sure the printer has been set
}

void PRINTER_print(unsigned char *password) {
   
            // Centering text
            USART_putc(27);
            USART_putc(97);
            USART_putc(1);
            // Double width and double height
            USART_putc(29); 
            USART_putc(33);
            USART_putc(161);
            USART_puts("IC-UFAL"); 
            USART_putc(10);   // Print LF
            __delay_ms(100);

            unsigned char pswrd;
            pswrd = password;
            // Regular width and regular height
            USART_putc(29); 
            USART_putc(33);
            USART_putc(0);
            // Centering text
            USART_putc(27);
            USART_putc(97);
            USART_putc(1);
            USART_puts("Sua senha:");
            USART_putc(10);   // Print LF
            __delay_ms(100);

            // Centering text
            USART_putc(27);
            USART_putc(97);
            USART_putc(1);
            USART_puts("----------");
            USART_putc(10);   // Print LF
            __delay_ms(100);

            // Double width and double height
            USART_putc(29); 
            USART_putc(33);
            USART_putc(161);
            // Centering text
            USART_putc(27);
            USART_putc(97);
            USART_putc(1);
            USART_putc(password);
            USART_putc(10);   // Print LF
            __delay_ms(100);

            // Regular width and regular height
            USART_putc(29); 
            USART_putc(33);
            USART_putc(0);
            // Centering text
            USART_putc(27);
            USART_putc(97);
            USART_putc(1);
            USART_puts("----------");
            __delay_ms(100);
            USART_putc(10);   // Print LF
            USART_putc(10);   // Print LF
}


// ================ MAIN ================
void main() {
    
    USART_init();
    PRINTER_init();
    
    IO_setup();
    SPI_setup_slave();
    SPI_enable_interruptions();
    while(1);
}

// Setting up interruption

void interrupt slave_read()
{ 
    if(PIR1bits.SSPIF == 1)
    {
       SSPCONbits.CKP = 0;          // Clock Polarity Select bit, Idle state for clock is a low level
       if ((SSPCONbits.SSPOV) || (SSPCONbits.WCOL))     // Checking collision and overwrite
        // If overflow (SSPOV): new byte is received while SSPBUF still holds the previous data
        //  or collision (WCOL): A write to SSPBUF was attempted while the SPI conditions were not valid 
        //                       for a transmission to be started
       {
             z = SSPBUF;            // Read the previous value to clear the buffer
             SSPCONbits.SSPOV = 0;  // Clear the overflow flag
             SSPCONbits.WCOL = 0;   // Clear the collision bit
             SSPCONbits.CKP = 0;    // Clock Polarity Select bit, Idle state for clock is a high level
       }
      if(SSPSTATbits.BF != 0)
      {
            z = SPI_read();
            unsigned char * h;
            if(z != 'b'){
                h = z;
                z = 'b';
            PRINTER_print(h);
            }
      }
       SSPIF = 0;                   // Clean Master Synchronous Serial Port (MSSP) Interrupt Flag bit
       __delay_ms(100);
    }
}
