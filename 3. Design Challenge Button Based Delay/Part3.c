/*
 * Part3.c
 *
 *  Created on: Feb 12, 2023
 *  Edited on: Feb 19, 2023
 *      Author: Christian Cipolletta
 *
 *      This code utilizes interrupts to blink an LED equal to the amount of time a button is pressed
 */

#include <msp430.h>
#include "GPIO_Driver.h"    // MSP430FR2355 GPIO driver created in Lab 1

#define DEFAULT 8000        // Sets default led blinking time to 8000

volatile unsigned int count;                    // Used to count how long Pin 2.3 is pressed
volatile unsigned char risingEdge = 0x01;
volatile unsigned char fallingEdge = 0x00;
volatile unsigned char counting = 0x00;

void pinInit();
void timerInit();

void main(){

    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    pinInit();
    timerInit();
    count = 8000;
    counting = 0x00;


    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    __bis_SR_register(LPM3_bits | GIE);
    __enable_interrupt();

}

/*
 * Method to initialize the needed I/O Pins
 */
void pinInit() {
    gpioInit(1,0,1);                // Initializes Pin 1.0 (red led) as an output
    gpioWrite(1,0,0);               // Sets Pin 1.0 (red led) to low

    gpioInit(6,6,1);                // Initializes Pin 6.6 (green led) as an output
    gpioWrite(6,6,0);               // Sets Pin 6.6 (green led) to low

    gpioInit(4,1,0);                // Initializes Pin 4.1 (reset button) as an input
    P4IES |= BIT1;                  // P4.1 High --> Low edge
    P4IE |= BIT1;                   // P4.1 interrupt enabled

    gpioInit(2,3,0);                // Initializes Pin 2.3 (button) as an input
    P2IES |= BIT3;                  // P2.3 High --> Low edge
    P2IE |= BIT3;                   // P2.3 interrupt enabled
}

/*
 * Method to initialize the needed Timers
 */
void timerInit() {
    // Initializes Timer to count how long Pin 2.3 (button) is pressed
    TB0CCTL0 = CCIE;                          // TB0CCR0 interrupt enabled
    TB0CCR0 = 1;
    TB0CTL = TBSSEL_1 | MC_2 | ID_3;          // ACLK, continuous mode, /8

    // Initializes Timer to blink Pin 1.0 (red led)
    TB1CCTL0 = CCIE;                          // TB1CCR0 interrupt enabled
    TB1CCR0 = 8000;
    TB1CTL = TBSSEL_1 | MC_2 | ID_3;          // ACLK, continuous mode, /8
}


/*
 * INTERRUPT ROUTINES
 */

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    if (risingEdge)                 // If Pin 2.3 is low (button is being held down)
    {
        risingEdge = 0x00;          // Tells interrupt to look for falling edge by setting variable to 1
        fallingEdge = 0x01;
        P2IES &= ~BIT3;             // P2.3 Low --> High edge
        P6OUT |= BIT6;              // Pin 6.6 (green led) is set to high
        P1OUT &= ~BIT0;             // Pin 1.0 (red led) is set to low
        counting = 0x01;
        count = 0;
    }

    else if (fallingEdge)           // If Pin 2.3 is high
    {
        risingEdge = 0x01;          // Tells interrupt to look for rising edge by setting variable to 1
        fallingEdge = 0x00;
        P2IES |= BIT3;              // P2.3 High --> Low edge
        P6OUT &= ~BIT6;             // Pin 6.6 (green led) is set to low
        counting = 0x00;
    }
}

// Port 4 interrupt service routine (For reset button)
#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    P4IFG &= ~BIT1;                         // Clear P4.1 IFG
    count = DEFAULT;                        // Sets led blinking speed to the default value
    counting = 0x00;
}

// Timer B0 interrupt service routine (For counting how long button is pressed)
#pragma vector = TIMER0_B0_VECTOR
__interrupt void Timer0_B0_ISR(void) {
    if(counting) {              // If Pin 2.3 (button) is low, increment count by 1
        count++;
    }
    else {                      // If Pin 2.3 (button) is high, count is equal to count
        count = count;
    }
    TB0CCR0 += 1;               // Add Offset to TB0CCR0
}

// Timer B1 interrupt service routine (For blinking led)
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
{
        P1OUT ^= BIT0;                      // Toggle Pin 1.0 (red led)
        TB1CCR0 += count;                   // Add Offset to TB1CCR0 equal to count
}
