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

#define TIMER_FOR_BUTTON_S5 2

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

volatile SlidingText st;
int counter_flag = 1;
int sliding_motion = 1;

void onBtnS5Released() {
    sliding_motion = !sliding_motion;
}
 
void task1()
{   
    int charIndex = st.offset + st.current;
    
    if(st.current == 16){
        counter_flag = 0;
        st.current = 0;
        while (SPI1STATbits.SPITBF == 1);
        SPI1BUF = 0x80;
        return;
    }
    
    if (counter_flag){
        if (charIndex == st.textLen){
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = ' ';
        } else if (charIndex > st.textLen) {
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = st.text[charIndex - st.textLen - 1];
        } else {
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = st.text[charIndex];
        }
        st.current++;
    }
}
 
void task2()
{
    counter_flag = 1;
    st.offset = (st.offset + 1) % st.textLen;
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
    st = (SlidingText){
        "This is a very long string",
        0, 0, 26
    };
    
    while(1)
    {
        scheduler();
        tmr_wait_period(TIMER1);
    }
}

void task4() {
    int charIndex = st.offset + st.current;

    if (st.current == 16) {
        counter_flag = 0;
        st.current = 0;
        while (SPI1STATbits.SPITBF == 1);
        SPI1BUF = 0x80;
        return;
    }

    if (counter_flag) {
        if (charIndex >= st.textLen && charIndex <= st.textLen + 14) {
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = ' ';
        } else if (charIndex > st.textLen + 14) {
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = st.text[charIndex - st.textLen - 15];
        } else {
            while (SPI1STATbits.SPITBF == 1);
            SPI1BUF = st.text[charIndex];
        }
        st.current++;
    }
}

void task5() {
        if(sliding_motion) {
        counter_flag = 1;
        st.offset = (st.offset + 1) % (st.textLen + 15);
    }
}

void exercise2() {
    int bits;
    int sliding_speed = 50;

    // Setting the Tad
    ADCON3bits.ADCS = 8; // Tad = 4.5 Tcy
    // sampling mode: manual sampling, automatic conversion
    ADCON1bits.ASAM = 0; // start
    ADCON1bits.SSRC = 7; // end
    
    ADCON3bits.SAMC = 16; // auto sampling time
    // selecting the channel to convert
    ADCON2bits.CHPS = 0;
    // chose the positive input of the channels
    ADCHSbits.CH0SA = 0b0010;

    // select the AN2 pin as analogue
    ADPCFG = 0xfffb;

    // turn the ADC on
    ADCON1bits.ADON = 1;

    init_btn_s5(&onBtnS5Released); // init the button S5 handler
    init_spi();
    TRISBbits.TRISB0 = 0; // set the pin as output
    tmr_wait_ms(TIMER1, 1500);
    tmr_setup_period(TIMER1, 5); // initialize heatbeat timer
    schedInfo[0] = (Heartbeat){0, 1, task4}; // task 1 runs every heartbeat
    schedInfo[1] = (Heartbeat){0, 50, task5}; // task 2 runs every 50 heartbeat
    schedInfo[2] = (Heartbeat){0, 100, task3}; // task 3 runs every 100 heartbeat
    st = (SlidingText){
        "This is a very long string",
        0, 0, 26
    };

    while (1) {
        ADCON1bits.SAMP = 1; // start sampling
        while (!ADCON1bits.DONE); // wait for the end of the conversion
        bits = ADCBUF0;
        sliding_speed = (int)(1.5*(bits/10.23) + 50);
        schedInfo[1].N = sliding_speed;
        scheduler();
        tmr_wait_period(TIMER1);
    }
}

int main(void)
{
    // exercise1();
    exercise2();
}