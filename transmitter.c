// PIC16F883 Configuration Bit Settings

// 'C' source line config statements

// CONFIG1
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

// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits (Write protection off)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.


#include <stdio.h>
#include <xc.h>
#define _XTAL_FREQ 8000000

int zero = 0; // Defines variables to setup printing
int heatTime = 100;
int heatInterval = 80;
char printDensity = 15; 
char printBreakTime = 15;

void USART_start(void) {
    // p154 Asynchronous Transmission Set-up
    // 1 -  SPBRGH SPBRG register pair
    SPBRGH = 1;
    SPBRG = 12; 
    
    // 2 - Set SPEN bit and clear SYNC for asynchronous operation
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;     
    
    // 3 - Disable 9-bit reception
    TXSTAbits.TX9 = 0;
    
    // 4 - Enable the transmission
    TXSTAbits.TXEN = 1;
    
    // 5 - Enable interruptions
    PIE1bits.TXIE = 1;      
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    // 6 - Clear RCIF
    PIR1bits.RCIF = 0;
    
    // 7 - Load 8-bit data into TXREG to transmit 
}   

void USART_char(unsigned char c) {
    while(!PIR1bits.TXIF);    // wait until transmit shift register is empty
        TXREG = c;               // write character to TXREG and start transmission
}

void USART_int(unsigned int i) {
    while(!PIR1bits.TXIF);
        TXREG = i;
}

void USART_string(unsigned char *s) {
    while (*s)
    {
        USART_char(*s);     // send character pointed to by s
        s++;                // increase pointer location to the next character
    }
}

void PRINTER_start() {
    //Modify the print speed and heat
    USART_int(27);
    USART_int(55);
    USART_int(7); //Default 64 dots = 8*('7'+1)
    USART_int(heatTime); //Default 80 or 800us
    USART_int(heatInterval); //Default 2 or 20us
    //Modify the print density and timeout
    USART_int(18);
    USART_int(35);
    // int printSetting = (printDensity<<4) | printBreakTime;
    USART_int(zero); //Combination of printDensity and printBreakTime
}

void USART_password() {
    // Centering text
    USART_int(27);
    USART_int(97);
    USART_int(1);

    // Double width and double height
    USART_int(29); 
    USART_int(33);
    USART_int(161);
    USART_string("IC-UFAL"); 
    USART_int(10);   // Print LF
    __delay_ms(1500);

    // Centering text
    USART_int(27);
    USART_int(97);
    USART_int(1);
    // Regular width and regular height
    USART_int(29); 
    USART_int(33);
    USART_int(zero);
    USART_string("Sua senha:");
    __delay_ms(1500);

    // Centering text
    USART_int(27);
    USART_int(97);
    USART_int(1);
    USART_string("----------");
    __delay_ms(1500);

    // Centering text
    USART_int(27);
    USART_int(97);
    USART_int(1);
    // Double width and double height
    USART_int(29); 
    USART_int(33);
    USART_int(161);
    USART_string("P0003");
    __delay_ms(1500);

    // Centering text
    USART_int(27);
    USART_int(97);
    USART_int(1);
    USART_string("----------");
    __delay_ms(1500);
    USART_int(10);   // Print LF
    USART_int(10);   // Print LF
 
    while (1>0); // do nothing
}

void main() {
   USART_start();
   
   //PRINTER_start();
   
   //USART_password();
   
   // TODO ask printer about its status
   // 27 118 2: paper status
    
    while(1){
        USART_int(10);
        //__delay_ms(0.05);
    }
    
    return;
}
