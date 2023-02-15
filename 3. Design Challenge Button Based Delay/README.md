# Design Challenge: Button-Based Delay
This is the part of the lab where I want you to start from what we have done to build something new. I will be providing you just a .c file which is pretty empty and you will need to build a system which achieves a certain task. While I might give you hints, you are the one who needs to actually design the algorithm and test it.

## System Description
You will need to build a system which has the LED blinking speed controlled by how long you hold down a button. The system should boot up and begin blinking at a specific speed (maybe like 4Hz). When the user presses the button, you should begin timing how long that button is being held down for. Once the button has been released, the LED should begin blinking using the amount of time the button was held down at the period of it blinking. For example, if I hold it down for 3 seconds, the LED should then blink every 3 seconds. If the user presses the other button, you can then reset the system back to a default speed.

## Pins/External Resources Needed
- Red LED, P1.0
- Button 0, P2.3
- Button 1, P4.1

## Peripherals Needed
- GPIO Peripheral
- Timer B Peripheral

## How to approach this
I would, as always, **WRITE DOWN OR DRAW YOUR SYSTEM AND FLOW**. It doesn't necessarily need a state machine, but *narating* and talking through what should be going on. You probably have seen be talk or write my code as functions first, and that is because I am actually speaking it out in my head what should be going on.

### Designing with Interrupts
This type of project where you will be balancing several interrupt routines requires a very different approach than what you might be used to. You will be designing a system which is **Event Driven**. This means that instead of thinking it through like "If the button is currently pressed, I am going to do this, then next I need to check this". You will end up thinking more like:
- What should my processor be doing by default (answer for this, most likely nothing since it is interrupt driven).
- The button interrupt is going to be going off when the button is pressed/released. What should I be doing in that event.
- The Timer interrupt is going to be controlling the LED independently of everything else, so how should that interrupt be designed.

## Button Pressing
Remember, you are timing how long a button is pressed down, so in your button interrupt, you will want to make sure you determine if the button is being pressed or released.

## Timers and CCR_ Registers
There are a total of **6 Capture/Compare Registers** in each timer peripheral. In the examples up to this point, we have used only one of the CCR register, but you can use several if you want to keep track of certain things. In the previous examples, we have used CCR0 to control the blinking speed. We can keep that the same, let's not mess with that. When trying to check the time, what you need to look into is:
- **MC:** How can you *pause* the timer? Or is there a way to store the current time?
- **TBCLR:** You can reset the timer counter, which might be useful for capturing time.
- **Capture Mode (advanced):** The other function you can do in your CCR register is actually capture the current time. You can try to look into how to do this.

## Counter Overflow - A Real Problem
If the counter inside the timer is a 16-bit number, that means the biggest number it can count up to is **65,535**. *"Ok, so..."*. If you have a clock that is 1MHz (1ns period), that means that the Timer Counter will overflow and reset every 65.535ms. **PROBLEM:** We will be holding down the button for much much much longer than that, so what can we do?

### Slower Clocks
You can access the **ACLK** clock source which is a real-time clock running at 32,768Hz. This would mean that you could capture just about 2 seconds of time, but we still need to do something else.

### Clock Dividers (ID) [Recommended Approach]
We can actually slow the clocks down using a set of clock dividers. We can control them with the **ID** fields in the Timer Control Registers. We can slow down the clock by factors of 2, 4, 8, 16, and even use an extended divider if need be.

### Timer Rollover Counts [Advise Against]
We can also use a Global variable to count how many times the clock has rolled over and then use that to determine the blinking speed. So if the timer rolls over, we can then increment the value in the variable. For this lab, this might not be the best way to start your path in designing with the timers, since now you have to bounce between several interrupt vectors and then manage a global variable.


# Deliverables
You will need to upload your code, well documented. You will also need a README for your code (you can replace this README) which will tell the user:
- How does your code work?
- How are the peripherals configured (what modes, what settings)?
- What design considerations did you have to take to account for things like the timer rollover?
- How should the user use your code?
