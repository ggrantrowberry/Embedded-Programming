#include "simonDisplay.h"
#include "buttonHandler.h"
#include <stdbool.h>
#include "supportFiles/display.h"
#include <stdio.h>
#include "supportFiles/utils.h"

#define ADC_MAX 1
#define ERASE true
#define DRAW false
#define INIT_VALUE 0

// States for the controller state machine.
enum buttonHandler_st_t {
    init_st, // Start here, transition out of this state on the first tick.
    wait_for_touch_st, //Waits for a touch
    adc_timer_st, //Debounces
    is_touched_st, //When the display is touched
    end_st //The end of the state machine

} buttonHandlerCurrentState;


//Determines if the buttonHandler state machine is on
static bool buttonHandlerEnabled = true;

//Flag for releasedTouch
static bool releaseDetectedFlag = false;

//Last touched region
uint8_t lastRegionTouched = INIT_VALUE;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static buttonHandler_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != buttonHandlerCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = buttonHandlerCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(buttonHandlerCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("BH_init_st\n\r");
        break;
      case wait_for_touch_st:
        printf("BH_wait_for_touch_st\n\r");
        break;
      case adc_timer_st:
        printf("BH_adc_timer_st\n\r");
        break;
      case is_touched_st:
        printf("BH_is_touched_st\n\r");
        break;
      case end_st:
        printf("BH_end_st\n\r");
        break;

     }
  }
}



/////////.h Functions/////////

// Get the simon region numbers. See the source code for the region numbering scheme.

uint8_t buttonHandler_getRegionNumber(){


    //Returns the region
    return lastRegionTouched;


}

// Turn on the state machine. Part of the interlock.
void buttonHandler_enable(){

    //Enables the state machine
    buttonHandlerEnabled = true;
}

// Turn off the state machine. Part of the interlock.
void buttonHandler_disable(){

    buttonHandlerEnabled = false;
}

// The only thing this function does is return a boolean flag set by the buttonHandler state machine. To wit:
// Once enabled, the buttonHandler state-machine first waits for a touch. Once a touch is detected, the
// buttonHandler state-machine computes the region-number for the touched area. Next, the buttonHandler
// state-machine waits until the player removes their finger. At this point, the state-machine should
// set a bool flag that indicates the the player has removed their finger. Once the buttonHandler()
// state-machine is disabled, it should clear this flag.
// All buttonHandler_releasedDetected() does is return the value of this flag.
// As such, the body of this function should only contain a single line of code.
bool buttonHandler_releaseDetected(){

    //Returns the value of the flag
    return releaseDetectedFlag;
}

////////State Machine///////////



int8_t adcCounter = INIT_VALUE;

void buttonHandler_init() {

  buttonHandlerCurrentState = init_st;
}

void buttonHandler_tick() {

  //  debugStatePrint();

  // Perform state update first.
  switch(buttonHandlerCurrentState) {

  //Initial state
    case init_st:

        //Checks to see if the state machine is enabled
        if(buttonHandlerEnabled){

            //Clears old touch data
            display_clearOldTouchData();

            //Sets the flag
            releaseDetectedFlag = false;


        //Sets the current state to wait for touch
        buttonHandlerCurrentState = wait_for_touch_st;

        }

      break;

      //Wait state
    case wait_for_touch_st:

        //Sets the releaseDetectedFlag
        releaseDetectedFlag = false;


        //Sets the counter to 0
        adcCounter = INIT_VALUE;

        //Checks to see if the SM is enabled
        if(!buttonHandlerEnabled){

            //Sets the current state to init
            buttonHandlerCurrentState = init_st;

            //Break
            break;

        } else {

            //Checks to see if the display is touched
            if(display_isTouched()){

                //Sets the current state to adc
                buttonHandlerCurrentState = adc_timer_st;

            }

        }

      break;

    case adc_timer_st:

        //Checks to see if the SM is enabled
        if(!buttonHandlerEnabled){

            //Sets the current state to init
            buttonHandlerCurrentState = init_st;

            //Break
            break;

        } else {

            if(display_isTouched() && adcCounter == ADC_MAX){

                //Sets the current state to touched
                buttonHandlerCurrentState = is_touched_st;

                //X coordinate of touch
                      int16_t x;

                      //Y coordinate of touch
                      int16_t y;

                      //Z coordinate of touch
                      uint8_t z;

                  //Gets the touchedPoint and puts them into x, y, and z
                  display_getTouchedPoint(&x, &y, &z);

                //Sets the last region touched
                lastRegionTouched = simonDisplay_computeRegionNumber(x,y);

                //Draw square
                simonDisplay_drawSquare(buttonHandler_getRegionNumber(), DRAW);

            }
        }

      break;
    case is_touched_st:

        //Checks to see if the SM is enabled
        if(!buttonHandlerEnabled){

            //Sets the current state to init
            buttonHandlerCurrentState = init_st;

            //Break
            break;

        } else {

            //Checks to see if the display is touched
            if(!display_isTouched()){


                //Sets the current state to touched
                buttonHandlerCurrentState = end_st;

                //Sets the releaseDetectedFlag
                releaseDetectedFlag = true;

                //Erases square
                simonDisplay_drawSquare(buttonHandler_getRegionNumber(), ERASE);

                //Redraws button
                simonDisplay_drawButton(buttonHandler_getRegionNumber(), DRAW);

            }
        }

      break;

    case end_st:

        //Sets the releaseDetectedFlag
        releaseDetectedFlag = false;

        //Checks to see if the button Handler is enabled
        if(!buttonHandlerEnabled){

            //Sets the state back to init
            buttonHandlerCurrentState = init_st;

            //Break
            break;


        }

        break;

    default:
      printf("clockControl_tick state update: hit default\n\r");
      break;
  }

  // Perform state action next.
  switch(buttonHandlerCurrentState) {

  //Init state
    case init_st:

        //Sets it to false
        releaseDetectedFlag = false;


      break;

      //Waiting for touch
    case wait_for_touch_st:

        //sets adc counter to 0
        adcCounter = INIT_VALUE;

      break;


        case adc_timer_st:

            //Increments adc Counter
            adcCounter++;

          break;

        case is_touched_st:

            //Nothing
          break;

        case end_st:
            //Nothing

            break;


     default:
      printf("clockControl_tick state action: hit default\n\r");
      break;
  }
}



#define RUN_TEST_TERMINATION_MESSAGE1 "buttonHandler_runTest()"  // Info message.
#define RUN_TEST_TERMINATION_MESSAGE2 "terminated."              // Info message.
#define RUN_TEST_TEXT_SIZE 2            // Make text easy to see.
#define RUN_TEST_TICK_PERIOD_IN_MS 100  // Assume a 100 ms tick period.
#define TEXT_MESSAGE_ORIGIN_X 0                  // Text is written starting at the right, and
#define TEXT_MESSAGE_ORIGIN_Y (DISPLAY_HEIGHT/2) // middle.

// buttonHandler_runTest(int16_t touchCount) runs the test until
// the user has touched the screen touchCount times. It indicates
// that a button was pushed by drawing a large square while
// the button is pressed and then erasing the large square and
// redrawing the button when the user releases their touch.

void buttonHandler_runTest(int16_t touchCountArg) {
    int16_t touchCount = 0;                 // Keep track of the number of touches.
    display_init();                         // Always have to init the display.
    display_fillScreen(DISPLAY_BLACK);      // Clear the display.
    // Draw all the buttons for the first time so the buttonHandler doesn't need to do this in an init state.
    // Ultimately, simonControl will do this when the game first starts up.
    simonDisplay_drawAllButtons();
    buttonHandler_init();                   // Initialize the button handler state machine
    buttonHandler_enable();
    while (touchCount < touchCountArg) {    // Loop here while touchCount is less than the touchCountArg
        buttonHandler_tick();               // Advance the state machine.
        utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
        if (buttonHandler_releaseDetected()) {  // If a release is detected, then the screen was touched.
            touchCount++;                       // Keep track of the number of touches.
            // Get the region number that was touched.
            printf("button released: %d\n\r", buttonHandler_getRegionNumber());
            // Interlocked behavior: handshake with the button handler (now disabled).
            buttonHandler_disable();
            utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
            buttonHandler_tick();               // Advance the state machine.
            buttonHandler_enable();             // Interlocked behavior: enable the buttonHandler.
            utils_msDelay(RUN_TEST_TICK_PERIOD_IN_MS);
            buttonHandler_tick();               // Advance the state machine.
        }
    }
    display_fillScreen(DISPLAY_BLACK);        // clear the screen.
    display_setTextSize(RUN_TEST_TEXT_SIZE);  // Set the text size.
    display_setCursor(TEXT_MESSAGE_ORIGIN_X, TEXT_MESSAGE_ORIGIN_Y); // Move the cursor to a rough center point.
    display_println(RUN_TEST_TERMINATION_MESSAGE1); // Print the termination message on two lines.
    display_println(RUN_TEST_TERMINATION_MESSAGE2);
}
