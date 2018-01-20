#include "clockDisplay.h"
#include "supportFiles/display.h"
#include <stdio.h>
#include <stdint.h>

//Period will be 25ms

#define ADC_COUNTER_MAX_VALUE 2 //Max value of the adc Counter.
#define AUTO_COUNTER_MAX_VALUE 20 //Max value of auto Counter
#define RATE_COUNTER_MAX_VALUE 4 //Max value of rate Counter
#define SEC_COUNTER_MAX_VALUE 40 //This is when it knows to increment seconds

//Different state that the state machine can be in
enum clockControl_st_t {
    init_st, //This is the starting state. It will initialize everything. Transition out after one tick
    never_been_touched_st, //If the screen hasn't been touched
    waiting_for_touch_st, //Wait here for a touch
    adc_Counter_running_st, //Waits for adc to settle
    auto_Counter_running_st, //Waits for the auto-update delay. User is holding down auto-inc/dec
    rate_Counter_running_st, //waits for the rate time to expire
    rate_Counter_expired_st //Adds a second to the clock
} currentState;

int32_t adcCounter = 0;
int32_t autoCounter = 0;
int32_t rateCounter = 0;
int32_t secCounter = 0;

// Call this before you call clockControl_tick().
void clockControl_init(){
    //Sets currentState to init
    currentState = init_st;
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static clockControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case never_been_touched_st:
        printf("never_touched_st\n\r");
        break;
      case waiting_for_touch_st:
        printf("waiting_for_touch_st\n\r");
        break;
      case adc_Counter_running_st:
        printf("adc_Counter_running_st\n\r");
        break;
      case auto_Counter_running_st:
        printf("auto_Counter_running_st\n\r");
        break;
      case rate_Counter_running_st:
        printf("rate_Counter_running_st\n\r");
        break;
      case rate_Counter_expired_st:
        printf("rate_Counter_expired_st\n\r");
        break;
     }
  }
}

void clockControl_tick() {

  //debugStatePrint();
  // Perform state update first. Mealy
  switch(currentState) {

      //The initial state
     case init_st:

      //Changes the state to waiting_for_touch_st
      currentState = never_been_touched_st;

      break;

     case never_been_touched_st:

         //Has been touched?
      if(display_isTouched()){

          currentState = waiting_for_touch_st;
      }

      break;

    //Waits for touch
    case waiting_for_touch_st:

        //Checks to see if the display has been touched
        if(display_isTouched()){

            //Clears the old touch data
            display_clearOldTouchData();

            //Transitions currentState to adc_Counter_running_st
            currentState = adc_Counter_running_st;
        }

        if(!display_isTouched() && secCounter == SEC_COUNTER_MAX_VALUE){

            //Increments by one second
            clockDisplay_advanceTimeOneSecond();

            //Sets secCounter to 0
            secCounter = 0;

        } else {

            //Increments secCounter
           secCounter++;

        }

      break;

    case adc_Counter_running_st:

        //Checks to see if the display isn't touched and the counter has reached max value
        if(!display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE){

            //Changes the display
            clockDisplay_performIncDec();

            //Changes the state
            currentState = waiting_for_touch_st;

            //Sets secCounter to 0
            secCounter = 0;

        }

        //Checks to see if it is touched and the counter has reached the max value
        if(display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE){

            //Changes the state
            currentState = auto_Counter_running_st;
        }

      break;

    case auto_Counter_running_st:

        //Checks to see if the display has been touched
        if(!display_isTouched()){

            //Updates clock display
            clockDisplay_performIncDec();

            //Changes state
            currentState = waiting_for_touch_st;

            //Sets secCounter to 0
            secCounter = 0;

        }

        //Is the display touched and is the auto counter at max
        if(display_isTouched() && autoCounter == AUTO_COUNTER_MAX_VALUE){

            //Updates clock display
            clockDisplay_performIncDec();

            //Changes state
            currentState = rate_Counter_running_st;

        }

      break;

    case rate_Counter_running_st:

        //Is the display still touched
        if(!display_isTouched()){

            //Changes the state
            currentState = waiting_for_touch_st;

            //Sets secCounter to 0
            secCounter = 0;

        }

        //Is display still touched and is the rate counter at max value
        if(display_isTouched() && rateCounter == RATE_COUNTER_MAX_VALUE){

            //Change State
            currentState = rate_Counter_expired_st;

        }

      break;

    //Transitions out of this state
    case rate_Counter_expired_st:

        //Is the display still touched
        if(display_isTouched()){

            //Increments the display
            clockDisplay_performIncDec();

            //Changes the state
            currentState = rate_Counter_running_st;
        }

        //When the display isn't touched
        if(!display_isTouched()){

           //Changes the current state
            currentState = waiting_for_touch_st;

            //Sets secCounter to 0
            secCounter = 0;
        }
      break;

    default:
      printf("clockControl_tick state update: hit default\n\r");
      break;
  }

  // Perform state action next.
  switch(currentState) {

    //It's empty because there are no actions
    case init_st:

      break;

    //It's empty because there are no actions
    case never_been_touched_st:

      break;

    //Waits for touch
    case waiting_for_touch_st:

        //Sets adcCounter to 0
        adcCounter = 0;

        //Sets autoCounter to 0
        autoCounter = 0;

        //Sets rateCounter to 0
        rateCounter = 0;

      break;

    //Wait for adc
    case adc_Counter_running_st:

        //increments adcCounter
        adcCounter++;

      break;

    //Wait for auto Counter
    case auto_Counter_running_st:

        //increments autoCounter
        autoCounter++;

      break;

    //Wait in rate Counter
    case rate_Counter_running_st:

        //increments rateCounter
        rateCounter++;

      break;

    //Resets rateCounter
    case rate_Counter_expired_st:

        //Sets rateCounter to 0
        rateCounter = 0;

      break;

     default:
      printf("clockControl_tick state action: hit default\n\r");
      break;
  }
}
