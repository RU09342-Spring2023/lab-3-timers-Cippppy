/*
 * Part3.c
 *
 *  Created on: Feb 12, 2023
 *  Edited on: Feb 15, 2023
 *      Author: Christian Cipolletta
 *
 *      YOU NEED TO FILL IN THIS AUTHOR BLOCK
 */

#include <msp430.h>
#include "GPIO_Driver.h"

#define DEFAULT 0
#define RECORDING 1
#define PLAYBACK 2

unsigned char state = 0x00;
unsigned char count = 0x00;
unsigned char risingEdge = 0x00;
unsigned char fallingEdge = 0x01;

void pinInit();
void timerInit();

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    pinInit();
    timerInit();


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);
}

void pinInit() {
    gpioInit(1,0,1);                // Initializes Pin 1.0 (red led) as an output
    gpioWrite(1,0,0);               // Sets Pin 1.0 (red led) to low

    gpioInit(6,6,1);                // Initializes Pin 6.6 (green led) as an output
    gpioWrite(6,6,0);               // Sets Pin 6.6 (green led) to low

    gpioInit(4,1,0);                // Initializes Pin 4.1 (reset button) as an input
    P4IES |= BIT1;                         // P4.1 High --> Low edge
    P4IE |= BIT1;                           // P4.1 interrupt enabled

    gpioInit(2,3,0);                // Initializes Pin 2.3 (button) as an input
    P2IES |= BIT3;                         // P2.3 High --> Low edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled
}

void timerInit() {
    TB1CCTL0 = CCIE;                          // TBCCR0 interrupt enabled
    TB1CCR0 = 50000;
    TB1CTL = TBSSEL_1 | MC_2;                 // ACLK, continuous mode
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    switch(state) {
    case 0 :
        P2IFG &= ~BIT3;                         // Clear P2.3 IFG
        state = RECORDING;
        break;

    case 1 :
        P2IFG &= ~BIT3;                         // Clear P2.3 IFG
        if (risingEdge)
        {
            risingEdge = 0x00;      // Tells interrupt to look for falling edge by setting variable to 1
            fallingEdge = 0x01;
            P2IES &= ~BIT3;                         // P2.3 Low --> High edge
            state = PLAYBACK;
        }

        else if (fallingEdge)
        {
            risingEdge = 0x01;      // Tells interrupt to look for rising edge by setting variable to 1
            fallingEdge = 0x00;
            P2IES |= BIT3;                         // P2.3 High --> Low edge
//            state = PLAYBACK;
        }

        break;

    case 2 :
        P2IFG &= ~BIT3;                         // Clear P2.3 IFG
        state = RECORDING;
        break;
    }
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
    state = DEFAULT;
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    switch(state) {
    case 0 :
        P1OUT ^= BIT0;
        P6OUT &= ~BIT6;
        TB1CCR0 += 50000;                         // Add Offset to TB1CCR0
        count = 0;
        break;

    case 1 :
        P6OUT |= BIT6;
        P1OUT &= ~BIT0;
        count++;
        break;

    case 2 :
        P1OUT ^= BIT0;
        P6OUT &= ~BIT6;
        TB1CCR0 += count;                         // Add Offset to TB1CCR0
        break;
    }
}


