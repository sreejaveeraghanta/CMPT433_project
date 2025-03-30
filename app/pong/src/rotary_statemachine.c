// Sample state machine for one GPIO pin.

#include "rotary_statemachine.h"
#include "hal/gpio.h"

#include "audioMixer.h"
#include "sounds.h"

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>

// Pin config info: GPIO 24 (Rotary Encoder PUSH)
//   $ gpiofind GPIO24
//   >> gpiochip0 10

//   $ gpiofind GPIO16
//   >> gpiochip2  7

//   $ gpiofind GPIO24
//   >> gpiochip2  8
#define GPIO_CHIP_ROTARY   GPIO_CHIP_2
#define GPIO_LINE_ROTARY_A 7
#define GPIO_LINE_ROTARY_B 8
#define DELAY_DEBOUNCE_IN_NS 100000000
#define GPIO_NUMBER_OF_LINES 2

static bool isInitialized = false;
static bool isClockwise = false;
static bool isCounterClockwise = false;

struct GpioLine* s_lineA = NULL;
struct GpioLine* s_lineB = NULL;

static pthread_t statemachineThreadId;

/*
    Define the Statemachine Data Structures
*/
struct stateEvent {
    struct state* pNextState;
    void (*action)();
};
struct state {
    struct stateEvent rising_A;
    struct stateEvent falling_A;
    struct stateEvent rising_B;
    struct stateEvent falling_B;
};

// Timing function
static long long getTimeInNanoS(void) 
{
    struct timespec spec;
    clock_gettime(CLOCK_BOOTTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec + seconds * 1000*1000*1000;
	assert(nanoSeconds > 0);
    
    static long long lastTimeHack = 0;
    assert(nanoSeconds > lastTimeHack);
    lastTimeHack = nanoSeconds;

    return nanoSeconds;
}

/*
    START STATEMACHINE
*/

static void no_direction(void)
{
    isClockwise = false;
    isCounterClockwise = false;
}

static void increment_volume(void)
{
    if(isClockwise){
        increase_volume();
        printf("Increment volume = %d\n", AudioMixer_getVolume());
    }
    no_direction();
}

static void decrement_volume(void)
{
    if(isCounterClockwise){
        decrease_volume();
        printf("Decrement volume = %d\n", AudioMixer_getVolume());
    }
    no_direction();
}

static void clockwise(void)
{
    isClockwise = true;
    isCounterClockwise = false;
}

static void counterclockwise(void)
{
    isClockwise = false;
    isCounterClockwise = true;
}

// Joystick and Rotary same states
struct state states[] = {
    { // Rest sets direction based on move
        .rising_A = {&states[0], NULL},
        .falling_A = {&states[1], clockwise},
        .rising_B = {&states[0], NULL},
        .falling_B = {&states[3], counterclockwise},
    },

    { // State 1
        .rising_A = {&states[0], decrement_volume},
        .falling_A = {&states[1], NULL},
        .rising_B = {&states[1], NULL},
        .falling_B = {&states[2], NULL},
    },

    { // State 2
        .rising_A = {&states[3], NULL},
        .falling_A = {&states[2], NULL},
        .rising_B = {&states[1], NULL},
        .falling_B = {&states[2], NULL},
    },

    { // State 3
        .rising_A = {&states[3], NULL},
        .falling_A = {&states[2], NULL},
        .rising_B = {&states[0], increment_volume},
        .falling_B = {&states[3], NULL},
    },
};

/*
    END STATEMACHINE
*/

struct state* pCurrentState = &states[0];

// TODO: This should be on a background thread!
static void* BtnStateMachine_doState_thread(void* _args)
{
    (void)_args;
    assert(isInitialized);



    //printf("\n\nWaiting for an event...\n");
    while (true) {
        struct gpiod_line_bulk bulkEvents;

        //int numEvents = Gpio_waitForLineChange(s_lineBtn, &bulkEvents);
        struct GpioLine* lines[] = {s_lineA, s_lineB};
        int numEvents = Gpio_waitForLineChange(lines, GPIO_NUMBER_OF_LINES, &bulkEvents);


        // Iterate over the event
        for (int i = 0; i < numEvents; i++)
        {
            // Get the line handle for this event
            struct gpiod_line *line_handle = gpiod_line_bulk_get_line(&bulkEvents, i);

            // Get the number of this line
            unsigned int this_line_number = gpiod_line_offset(line_handle);

            // Get the line event
            struct gpiod_line_event event;
            if (gpiod_line_event_read(line_handle,&event) == -1) {
                perror("Line Event");
                exit(EXIT_FAILURE);
            }


            // Run the state machine
            bool isRising = event.event_type == GPIOD_LINE_EVENT_RISING_EDGE;

            // Can check with line it is, if you have more than one...
            // bool isBtn = this_line_number == GPIO_LINE_NUMBER;
            // assert (isBtn);

            bool isLineA = this_line_number == GPIO_LINE_ROTARY_A;
            bool isLineB = this_line_number == GPIO_LINE_ROTARY_B;
            assert (isLineA || isLineB);

            struct stateEvent* pStateEvent = NULL;

            if (isLineA){
                if(isRising){
                    pStateEvent = &pCurrentState->rising_A;
                }
                else{
                    pStateEvent = &pCurrentState->falling_A;
                }
            }
            else if (isLineB){
                if(isRising){
                    pStateEvent = &pCurrentState->rising_B;
                }
                else{
                    pStateEvent = &pCurrentState->falling_B;
                }
            }
            else {
                // skip
                continue;
            } 

            // Do the action
            if (pStateEvent->action != NULL) {
                pStateEvent->action();
            }
            pCurrentState = pStateEvent->pNextState;

            // DEBUG INFO ABOUT STATEMACHINE
            #if 0
            int newState = (pCurrentState - &states[0]);
            double time = event.ts.tv_sec + event.ts.tv_nsec / 1000000000.0;
            printf("State machine Debug: i=%d/%d  line num/dir = %d %8s -> new state %d     [%f]\n", 
                i, 
                numEvents,
                this_line_number, 
                isRising ? "RISING": "falling", 
                newState,
                time);
            #endif
        }
    }
return NULL;
}

void RotaryStateMachine_init()
{
    assert(!isInitialized);
    Gpio_initialize();
    s_lineA = Gpio_openForEvents(GPIO_CHIP_ROTARY, GPIO_LINE_ROTARY_A);
    s_lineB = Gpio_openForEvents(GPIO_CHIP_ROTARY, GPIO_LINE_ROTARY_B);
    isInitialized = true;

    // Launch statemachine thread:
	pthread_create(&statemachineThreadId, NULL, BtnStateMachine_doState_thread, NULL);
}
void RotaryStateMachine_cleanup()
{
    assert(isInitialized);
    pthread_cancel(statemachineThreadId);
    isInitialized = false;
    Gpio_close(s_lineB);
    Gpio_close(s_lineA);
    Gpio_cleanup();
}