/*
 * File:   main.c
 * Author: ettor
 *
 * Created on 27 settembre 2022, 11.16
 */

// DSPIC30F4011 Configuration Bit Settings

// 'C' source line config statements

// FOSC
#pragma config FPR = XT                 // Primary Oscillator Mode (XT)
#pragma config FOS = PRI                // Oscillator Source (Primary Oscillator)
#pragma config FCKSMEN = CSW_FSCM_OFF   // Clock Switching and Monitor (Sw Disabled, Mon Disabled)

// FWDT
#pragma config FWPSB = WDTPSB_16        // WDT Prescaler B (1:16)
#pragma config FWPSA = WDTPSA_512       // WDT Prescaler A (1:512)
#pragma config WDT = WDT_OFF            // Watchdog Timer (Disabled)

// FBORPOR
#pragma config FPWRT = PWRT_64          // POR Timer Value (64ms)
#pragma config BODENV = BORV20          // Brown Out Voltage (Reserved)
#pragma config BOREN = PBOR_ON          // PBOR Enable (Enabled)
#pragma config LPOL = PWMxL_ACT_HI      // Low-side PWM Output Polarity (Active High)
#pragma config HPOL = PWMxH_ACT_HI      // High-side PWM Output Polarity (Active High)
#pragma config PWMPIN = RST_IOPIN       // PWM Output Pin Reset (Control with PORT/TRIS regs)
#pragma config MCLRE = MCLR_EN          // Master Clear Enable (Enabled)

// FGS
#pragma config GWRP = GWRP_OFF          // General Code Segment Write Protect (Disabled)
#pragma config GCP = CODE_PROT_OFF      // General Segment Code Protection (Disabled)

// FICD
#pragma config ICS = ICS_PGD            // Comm Channel Select (Use PGC/EMUC and PGD/EMUD)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>

void exercise1(void){

    TRISBbits.TRISB0 = 0; // set the pin as output 
    
    while(1){  
        LATBbits.LATB0 = 1; // set the pin high 
    }
}

void exercise2(void){
    int pinValue;
    
    TRISEbits.TRISE8 = 1; // set the pin as input 
    TRISBbits.TRISB0 = 0; // set the pin as output 
    
    while(1){  
        pinValue = !PORTEbits.RE8; // read the value and modify it
        LATBbits.LATB0 = pinValue; // set the pin high 
    }
}

void exercise3(void){
    int prevButtonValue = 1;  // 1 button up, 0 button pressed
    int buttonValue;
    
    TRISEbits.TRISE8 = 1; // set the button S5 pin as input 
    TRISBbits.TRISB0 = 0; // set the led D3 pin as output 
    
    while(1){
        
        // storing the variable because it may change during execution
        buttonValue = PORTEbits.RE8;
        
        if (buttonValue != prevButtonValue && buttonValue == 0){
            
            
            LATBbits.LATB0 = !LATBbits.LATB0; // toggle led
        }
        
        prevButtonValue = buttonValue;
    }
}

#define COUNTER_MAX_VALUE 1000
void exercise4(void){
    int counter = COUNTER_MAX_VALUE;
    
    TRISEbits.TRISE8 = 1; // set the button S5 pin as input 
    TRISBbits.TRISB0 = 0; // set the led D3 pin as output 
    
    while(1){
        
        // If the button is pressed
        if (PORTEbits.RE8 == 0) {
            
            // Avoid underflow
            if (counter > -1){
                counter--;
            }
        } else {
            counter = COUNTER_MAX_VALUE;
        }
        
        // The button must be pressed for COUNT_MAX_VALUE cycles to toggle the led
        if (counter == 0){
            LATBbits.LATB0 = !LATBbits.LATB0; // toggle led
        }
        
    }
}

int main(void) {
    
    // exercise1();  // Light the led
    // exercise2();  // Press the button to light the led
    // exercise3();  // Press the button to toggle the led
    exercise4();  // Press the button to toggle the led without rimbalzo meccanico
    
    return 0;
}