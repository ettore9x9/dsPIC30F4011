/*
 * File:   newmainXC16.c
 * Author: ettor
 *
 * Created on 1 ottobre 2022, 11.10
 */

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

// Definition of timers.
#define TIMER1 1 
#define TIMER2 2
#define TIMER3 3 
#define TIMER4 4 
#define TIMER5 5

void __attribute__((__interrupt__, __auto_psv__)) _T2Interrupt() {
    IFS0bits.T2IF = 0;                // reset interrupt flag
    LATBbits.LATB1 = !LATBbits.LATB1; // toggle led D4
}

void __attribute__((__interrupt__, __auto_psv__)) _INT0Interrupt() {
    // button interrupt
    
    IFS0bits.INT0IF = 0;           // reset interrupt flag
    IEC0bits.INT0IE = 0;           // disable button interrupt
    tmr_setup_period(TIMER3, 20);  // setup timer 3 for mechanical bouncing
}

void __attribute__((__interrupt__, __auto_psv__)) _T3Interrupt() {
    // timer 3 interrupt
    
    IFS0bits.T3IF = 0;                // reset interrupt flag
    
    if (PORTEbits.RE8 == 1) {         // if the button S5 is not pressed
        LATBbits.LATB1 = !LATBbits.LATB1; // toggle led D4
    }
    
    T3CONbits.TON = 0;   // stop the timer
    
    // Mind the sequence !!!!
    IFS0bits.INT0IF = 0; // reset interrupt flag of the led D4
    IEC0bits.INT0IE = 1; // enable timer 2 interrupt
}

void exercise1() {
    // Blink D3 led at 1Hz frequency, and blink D4 led at 2Hz frequency with interrupt
    
    TRISBbits.TRISB0 = 0; // set the D3 led pin as output 
    TRISBbits.TRISB1 = 0; // set the D4 led pin as output
    
    tmr_setup_period(TIMER1, 500); // setup timer 1 for toggle D3
    tmr_setup_period(TIMER2, 250); // setup timer 2 for toggle D4 with interrupt
    
    IEC0bits.T2IE = 1; // enable timer 2 interrupt
    
    while (1) {
        LATBbits.LATB0 = !LATBbits.LATB0; // toggle led D3
        tmr_wait_period(TIMER1); // wait until the period is expired
    }
}

void exercise2() {
    // Blink D3 led at 1Hz frequency, and blink D4 led when the button is pressed,
    // with an interrupt
    
    TRISBbits.TRISB0 = 0; // set the D3 led pin as output 
    TRISBbits.TRISB1 = 0; // set the D4 led pin as output
    TRISEbits.TRISE8 = 1; // set the button S5 pin as input
    
    tmr_setup_period(TIMER1, 500); // setup timer 1 for toggle D3
    
    IEC0bits.INT0IE = 1; // enable button S5 interrupt
    IEC0bits.T3IE = 1; // enable timer 3 interrupt
    
    while (1) {
        LATBbits.LATB0 = !LATBbits.LATB0; // toggle led D3
        tmr_wait_period(TIMER1); // wait until the period is expired
    }
}

int main() {
    
    // Wait 5 seconds before executing the exercise.
    tmr_wait_ms(TIMER1, 2000);
    
    //exercise1();
    exercise2();

}