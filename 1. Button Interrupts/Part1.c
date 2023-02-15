/*
 * Part1.c
 *
 *  Created on: Feb 11, 2023
 *  Edited on: Feb 15, 2023
 *      Author: Russell Trafford and Christian Cipolletta
 *
 *  This code is a template which will change the color of the LED being blinked using the interrupt routine.
 */


#include <msp430.h>

char LED_Color = 0x01;                       // Global Variable to determine which LED should be blinking
char risingEdge = 0x00;                      // Global variable to determine if the interrupt should be looking for rising edge
char fallingEdge = 0x01;                     // // Global variable to determine if the interrupt should be looking for falling edge

void pinInit();


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer

    pinInit();



    // Disable the GPIO power-on default high-impedance mode
    // to activate previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;



    P2IFG &= ~BIT3;                         // P2.3 IFG cleared

    __bis_SR_register(GIE);                 // Enter LPM3 w/interrupt

    while(1)
    {
        if (LED_Color) {
            P1OUT ^= BIT0;                  // P1.0 = toggle
            P6OUT &= ~BIT6;
        }
        else {
            P6OUT ^= BIT6;                 // P6.6 = toggle
            P1OUT &= ~BIT0;
        }
        __delay_cycles(100000);
    }
}


/*
 * pinInit()
 *
 * Configures the following GPIO Pins:
 *  - P1.0: Output
 *  - P6.6: Output
 *  - P2.3: Input with Pullup Resistor
 */

void pinInit(){
      // Configure RED LED on P1.0 as Output
      P1OUT &= ~BIT0;                         // Clear P1.0 output latch for a defined power-on state
      P1DIR |= BIT0;                          // Set P1.0 to output direction

      // Configure Green LED on P6.6 as Output
      P6OUT &= ~BIT6;                         // Clear P6.6 output latch for a defined power-on state
      P6DIR |= BIT6;                          // Set P6.6 to output direction


      // Configure Button on P2.3 as input with pullup resistor
      P2OUT |= BIT3;                          // Configure P2.3 as pulled-up
      P2REN |= BIT3;                          // P2.3 pull-up register enable
      P2IES &= ~BIT3;                         // P2.3 Low --> High edge
      P2IE |= BIT3;                           // P2.3 interrupt enabled

}




// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P2.3 IFG

    if (risingEdge)       // @TODO Fill in this argument within the If statement to check if the interrupt was triggered off a rising edge.
    {
        LED_Color = 0;
        // @TODO Add code to change which edge the interrupt should be looking for next
        risingEdge = 0x00;      // Tells interrupt to look for falling edge by setting variable to 1
        fallingEdge = 0x01;
        P2IES &= ~BIT3;                         // P2.3 Low --> High edge
    }

    else if (fallingEdge) // @TODO Fill in this argument within the If statement to check if the interrupt was triggered off a falling edge.
    {
        LED_Color = 1;
        // @TODO Add code to change which edge the interrupt should be looking for next
        risingEdge = 0x01;      // Tells interrupt to look for rising edge by setting variable to 1
        fallingEdge = 0x00;
        P2IES |= BIT3;                         // P2.3 High --> Low edge
    }
}

