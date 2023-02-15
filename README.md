# Lab 3: Timers and Interrupts
During Lab 2, you probably noticed a few unexpected behaviors when trying to program a simple LED blink code. For starters, did you notice that it wasn't as straight forward to have the LED blink at a specific interval (it was not just a simple Clock/Blinking Rate to figure out the required delays). And when you had to deal with multiple LED's blinking at different rates, could you easily maintain the timing between the edges? What about if you wanted the LEDs to be on/off at rates (otherwise known as duty cycle)?

And what about that button? Do we really need to keep checking whether or not the button was pressed at the fastest rate possible? What about if we wanted to perform an action on just a falling or rising edge?

If you have not realized it, this lab is going to focus on how to remedy these problems using built in peripherals in the processors.

## Goals For the Lab
By the end of this lab, you should be able to:
* Configure the TIMER modules within the MSP430 processors to maintain more accurate timing.
* Begin to configure interrupts for GPIO.
* Utilize the built in clock dividers.
* Begin looking into Low Power Modes within the MSP430 Family

The main idea around this lab is the use of interrupts and showing that they can provide a new way of thinking about your system design. At the end of the day, you will be using the concepts developed in the previous lab and tying in the TIMER peripherals to perform what you were brute forcing in the last lab.

## Order of Exercises
Since GITHUB likes to alphabetize files, you need to perform the exercises in the following order:
1. Button Interrupt
2. TIMER A Blink
3. Button Based Delay
4. (Optional) Reaction Game

## Deliverables
By the end of the lab, you will need to provide at a minimum well documented main.c files for *EACH* of the exercises accompanying README files which explain exactly how to implement your code. The .c and README files should include the minimum amount of work as well as any extra work that you have done.

### README Files
Since most of these projects will have just a simple main.c file, you do not need to generate 20 README files. Instead, unless you go for a more advanced implementation of the exercises, you just need 1 README per exercise folder. 

### Header Files
You may find yourself by the end of this lab not having generated any header files, which in this case, ignore this section. If you have generated more than just a main.c you most likely have yourself a .h file or two. Remember from the first lab that any header files or libraries that you generate need to have their own README.md which tell the user what dependencies there are, how to install them into their projects, etc.

## Documentation
Since you will most likely be using pre-made code to make most of your code, so what I am going to require you to do is comment each line of code and what it is doing. If you are using code from TI themselves, it will most likely have some of the comments there. But I want you to use your own words when writing these comments. Dive into the datasheets, look into the MSP430.h file to see what registers are being manipulated and tell me why you need to configure them.
