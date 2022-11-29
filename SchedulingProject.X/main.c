/*
 * File:   main.c
 * Authors: Carlone Matteo, Maragliano Matteo, Musumeci Mattia, Sani Ettore
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

#define TIMER_FOR_BUTTON_S5 3
#define TIMER_FOR_BUTTON_S6 4

#include "my_timer_lib.h"
#include "my_print_lib.h"
#include "my_circular_buffer_lib.h"
#include "my_btn_lib.h"
#include <stdio.h>

#define TASK_COUNT 3
 
typedef struct
{
    int n;  // number of periods elapsed from last task call
    int N;  // after how many periods should the task be called
    void (*task)(void);  // the task
} Heartbeat;

typedef struct {
    char text[64]; // string to write
    unsigned short int offset; // the first character to be written
    unsigned short int current; // current character to write
    unsigned short int textLen;
} SlidingText;

volatile SlidingText slidingText;
 
void task1()
{
    unsigned short int charIndex = slidingText.offset + slidingText.current;
    if(slidingText.current >= 16 || charIndex >= slidingText.textLen)
        return;
    
    while (SPI1STATbits.SPITBF == 1);
    SPI1BUF = slidingText.text[charIndex];
    slidingText.current++;
}
 
void task2()
{
    SPI1BUF = 0x80;
    slidingText.current = 0;
    slidingText.offset = (slidingText.offset + 1) % slidingText.textLen;
}
 
void task3()
{
    LATBbits.LATB0 = !LATBbits.LATB0; // toggle led
}
 
 
Heartbeat schedInfo[TASK_COUNT];
 
void scheduler()
{
    for(int i=0; i<TASK_COUNT; ++i)
    {
        if(++schedInfo[i].n < schedInfo[i].N)
            continue;
        schedInfo[i].task();
        schedInfo[i].n = 0;
    }
}
  
void exercise1()
{
    init_spi();
    TRISBbits.TRISB0 = 0; // set the pin as output
    tmr_wait_ms(TIMER1,1500);
    tmr_setup_period(TIMER1, 5); // initialize heatbeat timer
    schedInfo[0] = (Heartbeat){0,1,task1}; // task 1 runs every heartbeat
    schedInfo[1] = (Heartbeat){0,50,task2}; // task 2 runs every 50 heartbeat
    schedInfo[2] = (Heartbeat){0,100,task3}; // task 3 runs every 100 heartbeat
    slidingText = (SlidingText){
        "                This is a very long string ",
        0, 0, 43    
    };
    
    while(1)
    {
        scheduler();
        tmr_wait_period(TIMER1);
    }
}

void exercise2() {
}
 
void exercise3() {
}

int main(void)
{
    exercise1();
    // exercise2();
    // exercise3();
}