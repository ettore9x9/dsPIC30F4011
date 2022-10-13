/*
 * File:   newmainXC16.c
 * Author: ettor
 *
 * Created on 4 ottobre 2022, 11.29
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

#define TIMER_VALUE 20

void exercise1()
{
    // toggle the led every 500ms
    
    TRISBbits.TRISB0 = 0; // set the led pin as output 
    tmr_setup_period(TIMER1, 500); // setup the timer
    
    while (1) {
        LATBbits.LATB0 = !LATBbits.LATB0; // toggle led
        tmr_wait_period(TIMER1); // wait until the period is expired
    }
}

void exercise2()
{
    // turn on and off the led in a given pattern
    
    TRISBbits.TRISB0 = 0; // set the led pin as output
    
    LATBbits.LATB0 = 1; // turn on the led
    tmr_wait_ms(TIMER1, 1000); // wait 1s
    
    LATBbits.LATB0 = 0; // turn off the led
    tmr_wait_ms(TIMER1, 5000); // wait 5s
    
    LATBbits.LATB0 = 1; // turn on the led
    tmr_wait_ms(TIMER1, 500); // wait 0.5s
    
    LATBbits.LATB0 = 0; // turn off the led
    while(1); // do nothing, avoiding restarting
}

void exercise3() 
{
    // observe behavior of different delays
    
    TRISBbits.TRISB0 = 0; // set the led pin as output 
    tmr_setup_period(TIMER1, 500); // setup the timer
    int delay = 2000; // choose a delay
    while(1) {
        tmr_wait_ms(TIMER2, delay); // wait for the delay
        LATBbits.LATB0 = !LATBbits.LATB0; // toggle led
        tmr_wait_period(TIMER1); // wait until the timer is expired
    }
}

void exercise4()
{
    // advanced exercise
    
    TRISBbits.TRISB0 = 0; // set the led pin as output 
    TRISEbits.TRISE8 = 1; // set the button pin as input 
    
    tmr_setup_period(TIMER1, 100); // setup the timer for the period
    
    int pulses = 0; // counter for the pulses
    int counter = 0; // number of cycles of the timer
    
    LATBbits.LATB0 = 0; // LED off
    
    while (1)
    {

        // If the button is pressed
        if(PORTEbits.RE8 == 0)
        {
            //tmr_wait_ms(TIMER2, TIMER_VALUE); // the timer to count for the button press
            
            //if(PORTEbits.RE8 == 0) {  // if the button is pressed
            pulses++;
                    // pulses = (pulses - 1) % 3 + 1;
                //}
        }

        pulses = (pulses - 1) % 3 + 1;
        
        // toggle the LED
        if ( 2 * pulses > counter)
        {
            LATBbits.LATB0 = !LATBbits.LATB0; // toggle LED
        }
        
        counter++;
        counter = counter % 10;
        tmr_wait_period(TIMER1);
    }
}

int main()
{
    
    // Wait 5 seconds before executing the exercise.
    //tmr_wait_ms(TIMER1, 5000);
    
    //exercise1();
    //exercise2();
    //exercise3();
    exercise4();

}