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
#define _XTAL_FREQ 4000000

void RECEIVER_init(void)
{
    // p154 Asynchronous Transmission Set-up
    // 1 -  SPBRGH, SPBRG register pa SPBRGH, SPBRG register pair
    TXSTAbits.BRGH = 1;
    SPBRGH = 0;
    SPBRG = 12;  //TODO
    
    // 2 - Set SPEN bit and clear SYNC for asynchronous operation   
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;  
    
    // 3 - If interrupts are desired
    PIE1bits.RCIE = 1;      
    INTCONbits.GIE = 1;
    INTCONbits.PEIE = 1;
    
    // 4 - Disable 9-bit reception
    RCSTAbits.RX9 = 0;
    
    // 5 -  Enable reception 
    RCSTAbits.CREN = 1;
    
    // 6 - Clear RCIF
    PIR1bits.RCIF = 0;
    
    // 7 - Read RCSTA to get error flags
}

void main()
{
    TRANSMITTER_init();
    USART_putc(27);
    USART_putc(118); // Asking if printer is working fine
    
    
}

void interrupt ISR(void)
{
    if (PIR1bits.RCIF)  // check if receive interrupt has fired
    {
        unsigned char t;
        t = RCREG;      // read received character to buffer

        if(t == 0x04){
            //out of paper
            //Transmit by I2C to User Module about the paper
            //so, they can block the button
        }
        
        if(t == 0x40)
        {
            //temperature is high
            //Transmit by I2C to User Module about the temperature
            //so, they can block the button
        }
        if(t == 0)
        {
            //printer offline. might be out of paper
            //Transmit by I2C to User Module about the status
            //so, they can block the button
        }
        
        

        PIR1bits.RCIF = 0;      // reset receive interrupt flag
    }
}
