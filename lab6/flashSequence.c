#include "flashSequence.h"
#include <stdbool.h>
#include "globals.h"
#include "simonDisplay.h"
#include "supportFiles/display.h"
#include <stdio.h>
#include "supportFiles/utils.h"

#define INIT_VALUE 0
#define DRAW false
#define ERASE true
#define MAX_ERASE_VAL 5
#define ITER_LENGTH_FIX 1

// States for the controller state machine.
enum flashSequence_st_t {
    init_st, // Start here, transition out of this state on the first tick.
    flash_st, //Flashes the pattern
    erase_st,
    end_st
} flashSequenceCurrentState;

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrintFS() {
  static flashSequence_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != flashSequenceCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = flashSequenceCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(flashSequenceCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("FS_init_st\n\r");
        break;
      case flash_st:
        printf("FS_flash_st\n\r");
        break;
      case erase_st:
        printf("FS_erase_st\n\r");
        break;
      case end_st:
        printf("FS_end_st\n\r");
        break;

     }
  }
}


//Determines if the flashSequence state machine is on
static bool flashSequenceEnabled = true;

//Determines if the flashSequence state machine is complete
static bool flashSequenceIsComplete = false;

//EraseCount
static uint8_t eraseCount = INIT_VALUE;

//Keeps track of the flashes
static uint16_t flashCount = INIT_VALUE;

// Turns on the state machine. Part of the interlock.
void flashSequence_enable(){

    //Sets the flashSequenceEnabled to true
    flashSequenceEnabled = true;
}

// Turns off the state machine. Part of the interlock.
void flashSequence_disable(){

    //Sets the flashSequenceEnabled to false
    flashSequenceEnabled = false;
}

// Other state machines can call this to determine if this state machine is finished.
bool flashSequence_isComplete(){

    //Returns flashSequenceIsComplete
    return flashSequenceIsComplete;
}

// Initialize the state machine
void flashSequence_init(){

    //Sets the current state to init
    flashSequenceCurrentState = init_st;

}

// Standard tick function.
void flashSequence_tick(){

   // debugStatePrintFS();

    // Perform state update first.
     switch(flashSequenceCurrentState) {

     //Initial state
       case init_st:

           //Checks to see if the state machine is enabled
           if(flashSequenceEnabled){

               //Sets the flashCount to 0
               flashCount = INIT_VALUE;

               //Sets the current state to wait for touch
               flashSequenceCurrentState = flash_st;

               //Sets the flashSequenceIsComplete flag
               flashSequenceIsComplete = false;

           }

         break;

         //flash state
       case flash_st:

           //Checks to see if the SM is enabled
           if(!flashSequenceEnabled){

               //Sets the current state to init
               flashSequenceCurrentState = init_st;

           } else {

               //Sets the current state to erase
               flashSequenceCurrentState = erase_st;

               //Draw square
               simonDisplay_drawSquare(globals_getSequenceValue(flashCount), DRAW);

           }

         break;

       case erase_st:

           //Checks to see if the SM is enabled
           if(!flashSequenceEnabled){

               //Sets the current state to init
               flashSequenceCurrentState = init_st;

           } else {

               //Checks to see if it is ready to erase
               if(eraseCount == MAX_ERASE_VAL){

                   //If the flashCount is at the end of the sequence
                 if(flashCount == (globals_getSequenceIterationLength()-ITER_LENGTH_FIX)){

                     //Sets the current state back to init
                     flashSequenceCurrentState = end_st;

                 } else {

                   //Sets the current state to flash
                   flashSequenceCurrentState = flash_st;

                 }

                 //Erases square
                 simonDisplay_drawSquare(globals_getSequenceValue(flashCount), ERASE);

                 //Draw square
                 simonDisplay_drawButton(globals_getSequenceValue(flashCount), DRAW);

                 //Sets erase count back to 0
                 eraseCount = INIT_VALUE;

                 //Increments flashCount
                 flashCount++;

               }
           }

         break;
       case end_st:

           if(flashSequenceEnabled){

               //Sets the flashSequenceIsComplete flag
               flashSequenceIsComplete = true;

               //Sets the flash count
               flashCount = INIT_VALUE;

               //Sets the state back to init
               flashSequenceCurrentState = init_st;

           }


           break;

       default:
         printf("clockControl_tick state update: hit default\n\r");
         break;
     }

     // Perform state action next.
     switch(flashSequenceCurrentState) {

     //Init state
       case init_st:

           //Nothing

         break;

         //flash st
       case flash_st:

           //Nothing

         break;

       case erase_st:

           //Increments eraseCount
           eraseCount++;

         break;

       case end_st:

         break;

        default:
         printf("clockControl_tick state action: hit default\n\r");
         break;
     }
}



// This will set the sequence to a simple sequential pattern.
// It starts by flashing the first color, and then increments the index and flashes the first
// two colors and so forth. Along the way it prints info messages to the LCD screen.
#define TEST_SEQUENCE_LENGTH 8  // Just use a short test sequence.
uint8_t flashSequence_testSequence[TEST_SEQUENCE_LENGTH] = {
    SIMON_DISPLAY_REGION_0,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_3,
    SIMON_DISPLAY_REGION_2,
    SIMON_DISPLAY_REGION_1,
    SIMON_DISPLAY_REGION_0};    // Simple sequence.
#define INCREMENTING_SEQUENCE_MESSAGE1 "Incrementing Sequence"  // Info message.
#define RUN_TEST_COMPLETE_MESSAGE "Runtest() Complete"          // Info message.
#define MESSAGE_TEXT_SIZE 2     // Make the text easy to see.
#define TWO_SECONDS_IN_MS 2000  // Two second delay.
#define TICK_PERIOD 75          // 200 millisecond delay.
#define TEXT_ORIGIN_X 0                  // Text starts from far left and
#define TEXT_ORIGIN_Y (DISPLAY_HEIGHT/2) // middle of screen.

// Print the incrementing sequence message.
void flashSequence_printIncrementingMessage() {
  display_fillScreen(DISPLAY_BLACK);  // Otherwise, tell the user that you are incrementing the sequence.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Roughly centered.
  display_println(INCREMENTING_SEQUENCE_MESSAGE1);// Print the message.
  utils_msDelay(TWO_SECONDS_IN_MS);   // Hold on for 2 seconds.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
}

// Run the test: flash the sequence, one square at a time
// with helpful information messages.
void flashSequence_runTest() {
  display_init();                   // We are using the display.
  display_fillScreen(DISPLAY_BLACK);    // Clear the display.
  globals_setSequence(flashSequence_testSequence, TEST_SEQUENCE_LENGTH);    // Set the sequence.
  flashSequence_init();               // Initialize the flashSequence state machine
  flashSequence_enable();             // Enable the flashSequence state machine.
  int16_t sequenceLength = 1;         // Start out with a sequence of length 1.
  globals_setSequenceIterationLength(sequenceLength);   // Set the iteration length.
  display_setTextSize(MESSAGE_TEXT_SIZE); // Use a standard text size.
  while (1) {                             // Run forever unless you break.
    flashSequence_tick();             // tick the state machine.
    utils_msDelay(TICK_PERIOD);   // Provide a 1 ms delay.
    if (flashSequence_isComplete()) {   // When you are done flashing the sequence.
      flashSequence_disable();          // Interlock by first disabling the state machine.
      flashSequence_tick();             // tick is necessary to advance the state.
      utils_msDelay(TICK_PERIOD);       // don't really need this here, just for completeness.
      flashSequence_enable();           // Finish the interlock by enabling the state machine.
      utils_msDelay(TICK_PERIOD);       // Wait 1 ms for no good reason.
      sequenceLength++;                 // Increment the length of the sequence.
      if (sequenceLength > TEST_SEQUENCE_LENGTH)  // Stop if you have done the full sequence.
        break;
      // Tell the user that you are going to the next step in the pattern.
      flashSequence_printIncrementingMessage();
      globals_setSequenceIterationLength(sequenceLength);  // Set the length of the pattern.
    }
  }
  // Let the user know that you are finished.
  display_fillScreen(DISPLAY_BLACK);              // Blank the screen.
  display_setCursor(TEXT_ORIGIN_X, TEXT_ORIGIN_Y);// Set the cursor position.
  display_println(RUN_TEST_COMPLETE_MESSAGE);     // Print the message.
}
