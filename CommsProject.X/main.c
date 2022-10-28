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

#include "my_timer_lib.h"
#include <stdio.h>

// Definition of timers.
#define TIMER1 1 
#define TIMER2 2
#define TIMER3 3 
#define TIMER4 4 
#define TIMER5 5

int seconds = 0;

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    // button interrupt

    IFS0bits.INT0IF = 0; // reset interrupt flag
    IEC0bits.INT0IE = 0; // disable button interrupt
    tmr_setup_period(TIMER3, 20); // setup timer 3 for mechanical bouncing
}

void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt() {
    // timer 3 interrupt

    IFS0bits.T3IF = 0; // reset interrupt flag

    if (PORTEbits.RE8 == 1) { // if the button S5 is not pressed
        seconds = 0;
    }

    T3CONbits.TON = 0; // stop the timer

    // Mind the sequence !!!!
    IFS0bits.INT0IF = 0; // reset interrupt flag
    IEC0bits.INT0IE = 1; // enable button interrupt
}

void displayText(char text[]) {
    SPI1CONbits.MSTEN = 1; // master mode 
    SPI1CONbits.MODE16 = 0; // 8?bit mode 
    SPI1CONbits.PPRE = 3; // primary prescaler 
    SPI1CONbits.SPRE = 6; // secondary prescaler 
    SPI1STATbits.SPIEN = 1; // enable SPI
    
    SPI1BUF = 0x80;
    
    for(int i = 0; text[i] != '\0'; i++) {
        
        if (i % 16 == 0 && i > 15){
            
            if ((int)(i>>4) % 2 == 0){
                while (SPI1STATbits.SPITBF == 1); // wait until not full
                SPI1BUF = 0x80;
            }

            if ((int)(i>>4) % 2 != 0){
                while (SPI1STATbits.SPITBF == 1); // wait until not full
                SPI1BUF = 0xC0;
            }
        }
     
        while (SPI1STATbits.SPITBF == 1); // wait until not full
        SPI1BUF = text[i];
    }
}

void exercise1(void) {
    
    tmr_wait_ms(TIMER1, 1500);
    displayText("come quando fuori piove fraferraz non solleva una piuma");
    while(1);
}

void exercise2(void){
    tmr_wait_ms(TIMER1, 1500);
    
    tmr_setup_period(TIMER2, 1000);
    char str[100];
    int seconds = 0;
    while(1){
        sprintf(str, "sec = %d", seconds);
        displayText(str);
        seconds++;
        tmr_wait_period(TIMER2);
    }
}

void exercise3(void) {
    tmr_wait_ms(TIMER1, 1500);
    TRISEbits.TRISE8 = 1; // set the button S5 pin as input
    IEC0bits.INT0IE = 1; // enable button S5 interrupt
    IEC0bits.T3IE = 1; // enable timer 3 interrupt

    tmr_setup_period(TIMER2, 1000);
    char str[100];
    while (1) {
        sprintf(str, "sec = %d ", seconds);
        displayText(str);
        seconds++;
        tmr_wait_period(TIMER2);
    }
}

int main(void) {
    
    // exercise1();
    // exercise2();
    exercise3();
    
    return 0;
}