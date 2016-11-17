/*
 * File:   main.c
 * Author: Felipe
 *
 * Created on 2 de Novembro de 2016, 16:24
 */
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
void I2C_Master_Init(const unsigned long c)
{
    SSPCON = 0b00101000;        // SSP Module as Master
                                // Bit 0 - 3:   Set I2C master mode
                                // Bit 5:       Enables the synchronous serial port
    SSPCON2 = 0;                //
    SSPADD = 9;                 // Setting Clock Speed = (_XTAL_FREQ/(4*c))-1
                                // For c = 100.000, SSPADD = 9
    SSPSTAT = 0x80;             // 0x80??
    TRISC3 = 1;                 // Setting as input as given in datasheet
    TRISC4 = 1;                 // Setting as input as given in datasheet
}
void I2C_Master_Wait()
{
    // 0x04 = (0000 0100) and 0x1F = (0000 0100).    
    while ((SSPSTAT & 0x04) || (SSPCON2 & 0x1F));   // Transmit is in progress?
    // SSPSTAT[2] = Read/Write. 1 indicates that transmit is in progress and vice versa.
    // SSPCON2[4...0]
}
void I2C_Master_Start()
{
    I2C_Master_Wait();
    SEN = 1;                    // Initiate start condition on SCL & SDA pins 
                                //  and it will be automatically cleared by the hardware.
}
void I2C_Master_RepeatedStart()
{
    I2C_Master_Wait();
    RSEN = 1;                   // Initiate repeated start condition on both SCL & SDA pins 
                                //  and it will automatically cleared in hardware.
}
void I2C_Master_Stop()
{
    I2C_Master_Wait();
    PEN = 1;                    // Initiate stop condition on both SCL & SDA pins 
                                //  and it will be automatically cleared in hardware.
}
void I2C_Master_Write(unsigned d)
{
    I2C_Master_Wait();
//    while(!BF);               // BF (Buffer Full) will set when we write data 
                                //  to SSPBUF register and it is cleared when the data is shifted out
    SSPBUF = d;                 // Synchronous Serial Port Receive Buffer/Transmit Register
}
unsigned short I2C_Master_Read(unsigned short a)
{
    unsigned short temp;
    I2C_Master_Wait();
    RCEN = 1;                   // Enable receive mode for I2C
    I2C_Master_Wait();
    temp = SSPBUF;              // Read data from SSPBUF
    I2C_Master_Wait();          
    ACKDT = (a)?0:1;            // Value transmitted when the user initiates an 
                                //  acknowledge sequence at the end of a receive
    ACKEN = 1;                  // Initiate Acknowledge sequence on SDA and SCL pins, 
                                //  and transmit ACKDT data bit
    return temp;
}
void main(void) {
    nRBPU = 0;                  // Enable PORTB internal pull up resistor
    TRISB = 0xFF;               // PORTB as input
    TRISA = 0x00;               // PORTA as output
    PORTA = 0x00;               // All LEDs OFF
    I2C_Master_Init(100000);    // Initialize I2C Master with standard mode 100KHz clock
    __delay_ms(250);
    unsigned char x = 0x00;
    while(1)
    {
        I2C_Master_Start();     // Start condition
        I2C_Master_Write(0x30); // 7 bit address + Write (0011 0000)
        x = x ^ 0x01;           // ?
        I2C_Master_Write(x);    // Write data
        I2C_Master_Stop();      // Stop condition
        __delay_ms(500);        
       I2C_Master_Start();      // Start condition
       I2C_Master_Write(0x31);  // 7 bit address + Write (0011 0000)
       PORTA = I2C_Master_Read(0); // Read + Acknowledge
       I2C_Master_Stop();       // Stop condition
       __delay_ms(500);
    }
}