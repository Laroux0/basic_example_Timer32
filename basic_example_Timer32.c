// This application uses switch 1 and LED 1, and green LED2 on Launchpad as well as Timer32
// The operation is such that if you press the left button/switch on Launchpad, as long as you keep the button pressed,
// the LED is on. As soon as you release the button, LED is off and stays off until the button is pressed again.
// Meanwhile, the green LED blinks periodically.
// This example uses Driverlib functions.
// It also builds a HAL for buttons and LEDS but Timer32 does not use a HAL.

// This application uses #ifdef, #else and #endif
// These are preprocessor directives to the compiler similar to #define
// Their goal is include or exclude part of the code based on our desire in the next run
// For us, we like to see how using Timer32 is much more useful in an embedded system
// compared to simply waiting in a loop.
// At first, we run the code without without defining Timer32. So, the loop is used
// If you press the switch 1, you will see it doesn't work properly because the
// processor is blocked in the loop. If you uncomment //#define Timer32 then,
// you get the real Timer32 code and you will see its benefit.


#define Timer32

#include <ti/devices/msp432p4xx/driverlib/driverlib.h>



// According to the schematics on page 37 of the Launchpad user guide,
// left button (button 1) is connected to pin 1 (bit1)
#define LEFT_BUTTON (1<<1)

// According to the schematics on page 37 of the Launchpad user guide,
// left LED (LED 1) is connected to pin 0 (bit0)
#define LEFT_LED (1<<0)

// According to the schematics on page 37 of the Launchpad user guide,
// When a button is pressed, it is grounded (logic 0)
#define PRESSED 0


// The below values are the counter values to reach the desired one second,
// half a second, or two second period with 3MHz clock
#define ONE_SEC_COUNT  3000000
#define HALF_SEC_COUNT 1500000
#define TWO_SEC_COUNT  6000000

void TurnOn_Launchpad_LED1();
void TurnOff_Launchpad_LED1();
void TurnOn_Launchpad_Green_LED2();
void TurnOff_Launchpad_Green_LED2();
void Toggle_Launchpad_Green_LED2();
char S1isPressed();

// This function initializes all the peripherals
void initialize();

// In this example, the high-level function, main, is readable.
// It is also easy to port this function to other platforms with similar peripherals such as an LED and a switch
int main(void)
{
    // initialize all the peripherals
    initialize();

    while (1) {
        // If the button is not pressed, keep the LED off
        if (!S1isPressed())
            TurnOff_Launchpad_LED1();

        // otherwise, turn the LED on
        else
            TurnOn_Launchpad_LED1();

#ifdef Timer32
        // If the timer32 has expired
        if (Timer32_getValue(TIMER32_0_BASE) == 0)
        {
            // Toggle the LED
            Toggle_Launchpad_Green_LED2();

            // since the timer is in one-shot mode, we have to restart it to get a periodic behavior
            Timer32_setCount(TIMER32_0_BASE, ONE_SEC_COUNT);
            Timer32_startTimer(TIMER32_0_BASE, true);
        }
#else
        int i;
        // approximately 1 second delay by simple waiting in a loop
        for (i=0;i<240000;i++);
        Toggle_Launchpad_Green_LED2();
#endif
    }
}

// The HAL itself is written using Driverlib, so it is much easier to implement.
void initialize()
{

    // Stop watchdog timer
    // We do this at the beginning of all our programs for now.Later we learn more about it.
    WDT_A_hold(WDT_A_BASE);

    // Initializing LEDs
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);

    // Initializing S1 (switch 1 or button 1),
    GPIO_setAsInputPinWithPullUpResistor (GPIO_PORT_P1, GPIO_PIN1);

    // Initialize the timer
    Timer32_initModule(TIMER32_0_BASE, // There are two timers, we are using the one with the index 0
                       TIMER32_PRESCALER_1, // The prescaler value is 1; The clock is not divided before feeding the counter
                       TIMER32_32BIT, // The counter is used in 32-bit mode; the alternative is 16-bit mode
                       TIMER32_PERIODIC_MODE); //This options is irrelevant for a one-shot timer
    Timer32_setCount(TIMER32_0_BASE, ONE_SEC_COUNT);  // The count related to the period of the timer
    Timer32_startTimer(TIMER32_0_BASE, true); // start the timer in one-shot mode. This means the timer stops when it reaches 0
}


void TurnOn_Launchpad_LED1()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

void TurnOff_Launchpad_LED1()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);
}

char S1isPressed()
{
    return ((GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN1) == PRESSED));
}

void TurnOn_Launchpad_Green_LED2()
{
    GPIO_setOutputHighOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void TurnOff_Launchpad_Green_LED2()
{
    GPIO_setOutputLowOnPin(GPIO_PORT_P2, GPIO_PIN1);
}

void Toggle_Launchpad_Green_LED2()
{
    GPIO_toggleOutputOnPin(GPIO_PORT_P2, GPIO_PIN1);
}













