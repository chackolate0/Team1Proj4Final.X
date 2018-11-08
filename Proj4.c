/* ************************************************************************** */
/** Descriptive File Name

  @Company
 CPEG 222 Team 1
 * Alex Chacko & Matias Saavedra

  @File Name
 Proj4.c

  @Summary
    Brief description of the file.

  @Description
 Project 4 Final Stage
 */


// PIC32MX370F512L Configuration Bit Settings

// 'C' source line config statements

// DEVCFG3
// USERID = No Setting
#pragma config FSRSSEL = PRIORITY_7     // Shadow Register Set Priority Select (SRS Priority 7)
#pragma config PMDL1WAY = ON            // Peripheral Module Disable Configuration (Allow only one reconfiguration)
#pragma config IOL1WAY = ON             // Peripheral Pin Select Configuration (Allow only one reconfiguration)

// DEVCFG2
#pragma config FPLLIDIV = DIV_2        // PLL Input Divider (12x Divider)
#pragma config FPLLMUL = MUL_20         // PLL Multiplier (24x Multiplier)
#pragma config FPLLODIV = DIV_1       // System PLL Output Clock Divider (PLL Divide by 256)

// DEVCFG1
#pragma config FNOSC = FRCDIV           // Oscillator Selection Bits (Fast RC Osc w/Div-by-N (FRCDIV))
#pragma config FSOSCEN = ON             // Secondary Oscillator Enable (Enabled)
#pragma config IESO = ON                // Internal/External Switch Over (Enabled)
#pragma config POSCMOD = OFF            // Primary Oscillator Configuration (Primary osc disabled)
#pragma config OSCIOFNC = OFF           // CLKO Output Signal Active on the OSCO Pin (Disabled)
#pragma config FPBDIV = DIV_2           // Peripheral Clock Divisor (Pb_Clk is Sys_Clk/2)
#pragma config FCKSM = CSDCMD           // Clock Switching and Monitor Selection (Clock Switch Disable, FSCM Disabled)
#pragma config WDTPS = PS1048576        // Watchdog Timer Postscaler (1:1048576)
#pragma config WINDIS = OFF             // Watchdog Timer Window Enable (Watchdog Timer is in Non-Window Mode)
#pragma config FWDTEN = ON              // Watchdog Timer Enable (WDT Enabled)
#pragma config FWDTWINSZ = WINSZ_25     // Watchdog Timer Window Size (Window Size is 25%)

// DEVCFG0
#pragma config DEBUG = OFF              // Background Debugger Enable (Debugger is Disabled)
#pragma config JTAGEN = ON              // JTAG Enable (JTAG Port Enabled)
#pragma config ICESEL = ICS_PGx1        // ICE/ICD Comm Channel Select (Communicate on PGEC1/PGED1)
#pragma config PWP = OFF                // Program Flash Write Protect (Disable)
#pragma config BWP = OFF                // Boot Flash Write Protect bit (Protection Disabled)
#pragma config CP = OFF                 // Code Protect (Protection Disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

/* ************************************************************************** */

/* ************************************************************************** */
/* ************************************************************************** */
/* Section: Included Files                                                    */
/* ************************************************************************** */
/* ************************************************************************** */

/* This section lists the other files that are included in this file.
 */
#include <xc.h>
#include "btn.h"
#include "config.h"
#include "lcd.h"
#include <math.h>
#include "rgbled.h"
#include "ssd.h"
#include "swt.h"
#include "uart.h"
#include "ultr.h"
#include "utils.h"
#include "srv.h"

/* TODO:  Include other files here if needed. */

void delay_ms(int ms);

main(void){
    
    BTN_Init();
    delay_ms(100);
    LCD_Init();
    delay_ms(100);
    RGBLED_Init();
    delay_ms(100);
    SSD_Init();
    delay_ms(100);
    SWT_Init();
    delay_ms(100);
    UART_Init(9600);
    delay_ms(100);
    ULTR_Init(0,1,0,2);//Echo A1, Trigger A2
    delay_ms(100);
    SRV_Init();
    delay_ms(100);
    ADC_Init();
    LCD_WriteStringAtPos("Team: 1", 0, 0);
    while(1){
        SRV_GetPulse(0,4);
        SRV_SetPulseMicroseconds0(1500);
    }
}

void update_SSD(int value){
    int first,second,third,fourth,decPt1,decPt2,decPt3,decPt4;
    char SSD1 = 0b0000000; //SSD setting for 1st SSD (LSD)
    char SSD2 = 0b0000000; //SSD setting for 2nd SSD
    char SSD3 = 0b0000000; //SSD setting for 3rd SSD
    char SSD4 = 0b0000000; //SSD setting for 4th SSD (MSD)
    if (value>9999){
        first=floor(value/10000);
        second=floor((value%10000)/1000);
        third=floor((value%1000)/100);
        fourth=floor((value%100)/10);
        decPt1=0;
        decPt2=0;
        decPt3=1;
        decPt4=0;
    }
    else if (value>999){
        first=floor((value%10000)/1000);
        second=floor((value%1000)/100);
        third=floor((value%100)/10);
        fourth=floor((value%10));
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=1;
    }
    else{
        first=0;
        second=floor((value%1000)/100);
        third=floor((value%100)/10);
        fourth=floor((value%10));
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=1;
    }
    if(value<0){                          //Display nothing if object is out of range
        first=18;
        second=18;
        third=18;
        fourth=18;
        decPt1=0;
        decPt2=0;
        decPt3=0;
        decPt4=0;
    }
    SSD4=first;
    SSD3=second;
    SSD2=third;
    SSD1=fourth;
    SSD_WriteDigits(SSD1, SSD2, SSD3, SSD4,decPt1,decPt2,decPt3,decPt4);
}

void delay_ms(int ms) {
    int i, counter;
    for (counter = 0; counter < ms; counter++) {
        for (i = 0; i < 300; i++) {
        } //software delay ~1 millisec
    }
}