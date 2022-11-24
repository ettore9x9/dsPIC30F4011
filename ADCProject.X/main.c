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

void exercise1() {
    char buf[10];
    int bits;
    float percent;
    
    init_spi();
    tmr_wait_ms(TIMER1, 1500);
    
    // sampling mode: manual
    ADCON1bits.ASAM = 0; // start
    ADCON1bits.SSRC = 0; // end
    // selecting the channel to convert
    ADCON2bits.CHPS = 0;
    // chose the positive input of the channels
    ADCHSbits.CH0SA = 0b0010;

    // select the AN2 pin as analogue
    ADPCFG = 0xfffb;

    // turn the ADC on
    ADCON1bits.ADON = 1;

    while (1) 
    {
        ADCON1bits.SAMP = 1; // start sampling
        tmr_wait_ms(TIMER1, 100);
        ADCON1bits.SAMP = 0; // end sampling, start conversion
        while (!ADCON1bits.DONE); // wait for the end of the conversion
        bits = ADCBUF0;
        percent = bits/10.23;
        sprintf(buf, "%db  ", bits);
        lcd_write(0, buf); // read the sampled value
        sprintf(buf, "%.0f%% ", percent);
        lcd_write(16, buf); // read the sampled value
    }
}

void exercise2() {
    char buf[10];
    unsigned int bits;
    double temp;
    init_spi();
    tmr_wait_ms(TIMER1, 1500);
    // Setting the Tad
    ADCON3bits.ADCS = 8; // Tad = 4.5 Tcy
    // sampling mode: manual sampling, automatic conversion
    ADCON1bits.ASAM = 0; // start
    ADCON1bits.SSRC = 7; // end
    
    ADCON3bits.SAMC = 16; // auto sampling time
    // selecting the channel to convert
    ADCON2bits.CHPS = 0;
    // chose the positive input of the channels
    ADCHSbits.CH0SA = 0b0011;

    // select the AN3 pin as analogue
    ADPCFG = 0xfff7;

    // turn the ADC on
    ADCON1bits.ADON = 1;

    while (1) {
        ADCON1bits.SAMP = 1; // start sampling
        while (!ADCON1bits.DONE); // wait for the end of the conversion
        bits = ADCBUF0;
        temp = bits * 0.48828125 - 50;
        sprintf(buf, "T=%ub  ", bits);
        lcd_write(0, buf); // read the sampled value
        sprintf(buf, "T=%.1fC  ", temp);
        lcd_write(16, buf); // read the sampled value
        tmr_wait_ms(TIMER1,1000);
    }
}
 
void exercise3() {
    char buf[10];
    unsigned int bits;
    double temp;
    init_spi();
    tmr_wait_ms(TIMER1, 1500);
    // Setting the Tad
    ADCON3bits.ADCS = 8; // Tad = 4.5 Tcy
    // sampling mode: automatic sampling, automatic conversion
    ADCON1bits.ASAM = 1; // start
    ADCON1bits.SSRC = 7; // end
    
    ADCON3bits.SAMC = 16; // auto sampling time
    
    // selecting the channel to convert
    ADCON2bits.CHPS = 0b01;
    // chose the positive input of the channels
    ADCHSbits.CH0SA = 0b0010;
    ADCHSbits.CH123SA = 1;

    // select the AN2 and AN3 pin as analogue
    ADPCFG = 0xfff3;
    
    ADCON1bits.SIMSAM = 1;
    // turn the ADC on
    ADCON1bits.ADON = 1;

    while (1) {
        ADCON1bits.SAMP = 1; // start sampling
        while (!ADCON1bits.DONE); // wait for the end of the conversion
        // handling potentiometer
        bits = ADCBUF0;
        temp = bits/10.23;
        sprintf(buf, "P=%ub ", bits);
        lcd_write(0, buf); // read the sampled value
        sprintf(buf, "P=%.1f%% ", temp);
        lcd_write(16, buf); // read the sampled value
        
        // handling termometer
        bits = ADCBUF1;
        temp = bits * 0.48828125 - 50;
        sprintf(buf, "T=%ub ", bits);
        lcd_write(8, buf); // read the sampled value
        sprintf(buf, "T=%.1fC ", temp);
        lcd_write(24, buf); // read the sampled value
        tmr_wait_ms(TIMER1, 500);
    }
}

int main(void)
{
    // exercise1();
    // exercise2();
    exercise3();
}