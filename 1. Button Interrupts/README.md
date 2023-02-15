# Button Interrupts and Edges
In the previous lab we introduced more of the buttons and started talking about interrupts, but I wanted to take another chance to look at some of the peculiar things you can do with the settings. For example, we can actually change the edge sensitivity to look for different parts of the button press, and possibly use that later to time certain things.

## Edge Selection
Edge sensitivity is controlled by the **PXIES** register and can allow us to determine whether our interrupt should fire on the rising or falling edge. If we look at the GPIO peripheral map, we can see how it interfaces with the rest of the interrupt generation circuit in order to alert our peripheral.

![P1 Interrupt](https://i.gyazo.com/63c30393ec86ae4b84922ec10b623b97.png)

## Why do we care - The anatomy of a button press
We have two different edges we care about: **Rising** (when our signal goes from a **0** to a **1**) and **Falling** (when our signal goes from **1** to a **0**). When we press a button, our voltage signal looks like the following:

![Button Signal](https://i.gyazo.com/e3a792e3ecd3bed528bc4fd19a5a61a1.png)

- First, Falling Edge
When the button is idling, thanks to a pull up resistor, it is sitting at Vcc or a logic 1. When the button gets pressed, the button state transitions from a 1 to a 0 as it gets connected to ground. For *as long the button is being held down* the output of the button remains a 0.

- Second, Rising Edge
When the button is released, it will spring back up to a 1, giving us a rising edge.

## Leveraging the interrupt for our main
One of the ways many people approach their code without interrupts is to try and determine what state a certain thing is in. For example, in the main code, trying to track with a flag or variable what position the button is currently in. With an interrupt, we can actually let the interrupt routine handle changing the button state variable. Take the following for example:
```c
#include <msp430.h>

char Current_LED = 0;                       // Global Variable to track which LED should be currently on

int main(void)
{
  gpioInit();   // Code for setting up the LED and the button
  while(1){
    if (Current_LED){
      P1OUT ^= BIT0;  // Toggle the RED LED
    }
    else{
      P6OUT ^= BIT6;  // Toggle the GREEN LED
    }
    __delay_cycles(100000);
  }
}

// Port 2 interrupt service routine
#pragma vector=PORT2_VECTOR
__interrupt void Port_2(void)
{
    P2IFG &= ~BIT3;                         // Clear P1.3 IFG
    // If we were interrupted while the rising edge sensitivity was set
    if (risingEdge){
      Current_LED = 0;
      P6OUT &= ~BIT6;
      // SetTheEdgeSensitivityToFalling
    }
    // If we were interrupted while the falling edge sensitivity was set
    else if (fallingEdge){
      Current_LED = 1;
      P1OUT &= ~BIT0;
      // SetTheEdgeSensitivityToRising
    }
}

```

In this code, we are letting the Interrupt handle what LED should be blinking at a given time by changing a global variable. We also **within the interrupt routine** change which edge the interrupt is sensitive to.

# Assignment for Part 1: Changing LED Color based on Button State
You will take the shell code given to you in this part and need to add code where there are "@TODO" comments to make the code function in the following way:
- Upon starting, the RED LED should be blinking while the button is not pressed.
- When the button is pressed, the interrupt should change which LED is blinking, and change the edge direction in the edge sensitivity register.
- While the button is pressed the LED should stay blinking Green.
- When the button is released, the Green LED should turn off and the Red LED should be blinking, changing the edge direction of the edge sensitivity register.

TODO statements will be in comments, but also should show up as a little checklist or clipboard on the left side of your editor.
![ToDo in CCS](https://i.gyazo.com/ed9b4529623b98bdb5b603f0a4e0c28b.png)
