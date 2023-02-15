/*
 * Part2.c
 *
 *  Created on: Feb 11, 2023
 *  Edited on: Feb 15, 2023
 *      Author: Russell Trafford and Christian Cipolletta
 *
 *      This code will need to change the speed of an LED between 3 different speeds by pressing a button.
 */

#include <msp430.h>

void gpioInit();
void timerInit();

#define LOW 0;
#define MED 1;
#define HIGH 2;
char state = LOW;

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    gpioInit();
    timerInit();

    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);

}


void gpioInit(){
    // @TODO Initialize the Red or Green LED
    P6DIR |= BIT6;                  // Set Pin 6.6 (green led) to output
    P6OUT &= ~BIT6;                 // Set Pin 6.6 (green led) to low

    // @TODO Initialize Button 2.3
    P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
    P2REN |= BIT3;                          // P2.3 pull-up register enable
    P2IES &= ~BIT3;                         // P2.3 Low --> High edge
    P2IE |= BIT3;                           // P2.3 interrupt enabled

}

void timerInit(){
    // @TODO Initialize Timer B1 in Continuous Mode using ACLK as the source CLK with Interrupts turned on
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
    // @TODO Remember that when you service the GPIO Interrupt, you need to set the interrupt flag to 0.
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    // @TODO When the button is pressed, you can change what the CCR0 Register is for the Timer. You will need to track what speed you should be flashing at.

    switch(state) {
    case 0 :
        state = MED;
        break;
    case 1 :
        state = HIGH;
        break;
    case 2 :
        state = LOW;
        break;
    }
}


// Timer B1 interrupt service routine
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
    // @TODO You can toggle the LED Pin in this routine and if adjust your count in CCR0.
    P6OUT ^= BIT6;
    switch(state) {
    case 0 :
        TB1CCR0 += 50000;                         // Add Offset to TB1CCR0
        break;
    case 1 :
        TB1CCR0 += 10000;                         // Add Offset to TB1CCR0
        break;
    case 2 :
        TB1CCR0 += 1000;                         // Add Offset to TB1CCR0
        break;
    }
}


