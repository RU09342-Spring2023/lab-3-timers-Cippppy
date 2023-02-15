# Timers and Timer Modules
Up to this point, timing has been, imprecise. We have been using delay cycles, random loops, and more, but all of the options **keep the processor occupied**. Loops are dependent on checking a condition constantly, taking up clock cycles, and they are not exact, especially if there are branching conditions within the loop. In our most basic example of blinking and LED initially:
```c
P1OUT ^= BIT0;
__delay_cycles(100000);
P1OUT ^= BIT0;
__delay_cycles(100000);
```
Or something like:
```c
i = 0;
while (i<=100000)
{
  i++;
}
P1OUT ^= BIT0;
```

We are spending over *200,000* clock cycles just to manage blinking this LED. If we want things to be precise or be able to let the processor do things while waiting to blink the LED, why don't we leverage another peripheral to do this: **TIMERS**

## What would we want in a timer module
Before we actually take a look what is in the Timer module, what would we want from it? Looking at the previous examples:
- We are trying to space out something happening by a certain amount of time. (Setting a timer)
- We are waiting for that amount of time to trigger moving on to the next instruction. (Setting an Alarm/Interrupt)
- We would like to time how long something is taking. (Capturing or recording an amount of time).

In a pseudocode fashion, we would really like to maybe be able to setup our code more like:
```c
void main(){
  gpioInit();
  setTimerClock();    // Set how fast our clock should run
  setTimerAlertFor(); // Tell the timer to alert us after a certain amount of time
  while(1){
    ...
  }
}

void timer_interrupt(){
  turnOffAlarm();
  doTaskWhenAlarmGoesOff();
}
```

This actually sort of flows pretty naturally, and when we get into it, it makes a lot of our main function simpler to write. To do this, we need some simple blocks to build off of:
- A clock, or something which can generate a clock signal of a known frequency.
- A counter, which can increment based on the clock signal.
- A comparator, which can let us tell if a certain amount of time has passed.

## TIMER B Modules
Let's start taking a look at the Timer B Module.
### Timer Block
This is the part of the timer that is actually keeping time. It is based on a clock generator, which can be controlled with dividers to be slower or quicker. This increments a Counter which is effectively counting how many of the clock cycles have gone by.
![Timer B Timer Block](https://i.gyazo.com/41bde011330cc3404d67aca1dcb7809e.png)

The register which controls this is **TBXCTL**. There are several fields within the register which is important in basic configuration:
- **TBSSEL:** Selects the clock source. For most of our early examples, we will be using the *SMCLK* which is a 1MHz clock.
- **ID:** Internal divider, which can be set to 00 (/1), 01 (/2), 10 (/4), 11 (/8). This can be further divided using the **IDEX** register.
- **MC:** Determines if the counter should not count (00), count up to a user set value (01), count up to a module set value (10), count up to a number then back down to zero (11).

To use these configurations, there are several Macros which have been defined to access these specific fields within the **TBXCTL** register.
```c
TBXCTL = ID_0 | TBSSEL_2 | MC_UP;
```
These definitions can be found in the MSP430FR2355.h file, and you can search for the following header.

![TB0 Registers](https://i.gyazo.com/df9136af276a8e0284bf5a7b2a808bdc.png)

### Capture and Compare Block
This seems more complex, but it at its core allows you to *Capture* the current time in the counter, and also *Compare* the current time to another value. Towards the bottom, we also have a circuit which can **control a pin directly**, but more on that later.

![TB0 CCR](https://i.gyazo.com/f2f668a54df03189981ba6b275e4dbef.png)

There are quite a few control signals here, but if we would like to have this thing **Alarm** us with an interrupt, to the bottom right, you can see **TBxCCRx CCIFG**. This is *Timer B X Capture Compare Register X Capture Compare Interrupt Flag*.

The core of the Compare function is a **Comparator** which compares the Timer Counter within a value set in a register **TBxCCR6**. Its result can then be set to the **CCIFG** output through a Mux, controlled by the **CAP** field within the **TBxCCTLn** Register.

## Using the Timer Module to Blink the LED

This is starting to seem like a lot, so let's look at an example from the TI Resource Explorer. We will be looking at [msp430fr235x_tb1_05.c](https://dev.ti.com/tirex/explore/node?node=A__AF3BxGU2G61YHOGQapaPEA__msp430ware__IOGqZri__LATEST) example, which is replicated below.

```c
#include <msp430.h>

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;                            // Set Pin as output
    P1OUT |= BIT0;

    // Disable the GPIO power-on default high-impedance mode to activate
    // previously configured port settings
    PM5CTL0 &= ~LOCKLPM5;

    // Timer1_B3 setup
    TB1CCTL0 = CCIE;                          // TBCCR0 interrupt enabled
    TB1CCR0 = 50000;
    TB1CTL = TBSSEL_1 | MC_2;                 // ACLK, continuous mode

    __bis_SR_register(LPM3_bits | GIE);       // Enter LPM3 w/ interrupt
}

// Timer B1 interrupt service routine
#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector = TIMER1_B0_VECTOR
__interrupt void Timer1_B0_ISR(void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(TIMER1_B0_VECTOR))) Timer1_B0_ISR (void)
#else
#error Compiler not supported!
#endif
{
    P1OUT ^= BIT0;
    TB1CCR0 += 50000;                         // Add Offset to TB1CCR0
}
```


### Configuring the Timer
Let's focus on the lines of code which are configuring the Timer Module.

#### Enabling the Timer Interrupt: TB1CCTL0 = CCIE;
Taking a look at the MSP430FR2355 Family User Guide, we can see what the **TB1CCTL0** register controls.

![TB1CCTL0](https://i.gyazo.com/2247f89f8d5f8327e12625d741f4a69c.png)

*Note:* This is a **DESTRUCTIVE ASSIGNMENT**, meaning that it will take the current contents of the **TB1CCTL0** register and obliterate all of it to zeros except for the bits in the **CCIE** Macro.

#### Set the number for the counter to count to: TB1CCR0 = 50000;
If we are going to use the Interrupt, we would like it to interrupt us when we get up to a specific number. We need to set the **TB1CCR0** register for the comparator to alert us when we hit that number. Take a look below if you want to see when the Interrupt would fire off.

![TimerB Interrupt Timing](https://i.gyazo.com/4df6c91163aaa3a23b2b3f3e01d60f6c.png)

#### Turning the Timer on with an ACLK source: TB1CTL = TBSSEL_1 | MC_2;
We are doing two main configurations here. First, **MC_2** turns the Timer into Continuous mode where the counter in the Timer keeps counting up to its maximum. **TBSSEL_1** choses the internal **ACLK** which is an internal 32,768Hz clock.

### Handling the Interrupt
When the Timer counts up to 50,000, the comparator will trigger the interrupt signal, and we will be in the interrupt routine. If we want the LED to Blink every period, we do the toggle in the interrupt. And then we can increment the CCR0 register by 50,000.

Essentially, you can think of it like this: the counter in the timer is like the clock we know. If I told you every 5 minutes I want you to do something and we started at 10am, the next time you should do it is 10:05am. However... do you need to know the exact time in the world... *or just that 5 minutes has passed?* Every 5 minutes, you do the thing, and then say I am going to do this again in 5 minutes. To the global counter, that is going to be interpretted as 10:10am, but to you, you just said "hey, I will do this again in 5 minutes."

# Assignment: Selecting Blinking Speed from a button
You will need to fill in the file attached to this folder to change the blinking speed of the LED between 3 different speeds when you press a button. Think of it as a controller that cycles from slow, mid, and fast, and back to slow for button presses.

For the button, you can use either polling or interrupts to determine the button press, but I think you **really should be using interrupts for this**.

You will need to look at the examples from part 1 and the example code presented in this part of the lab to accomplish this.

## Hint: What is your button interrupt going to do?
There are several ways you could approach this, but I would like to point specifically to the **TB1CCR0** register which controls how often the Timer Interrupt will be going off. You may want to consider how your Button Interrupt could possibly change or control that. 
