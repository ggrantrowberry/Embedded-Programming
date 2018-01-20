#include "simonControl.h"
#include "supportFiles/display.h"
#include "flashSequence.h"
#include "verifySequence.h"
#include <stdlib.h>
#include <stdint.h>
#include "globals.h"
#include "simonDisplay.h"
#include <string.h>
#include <stdio.h>
#include "buttonHandler.h"

#define END_LEVEL_MAX 50 //The End level counter max value
#define LEVEL_SEQ_OFFSET 3 //How many flashes the first level should have
#define BUTTON_COUNT 4 //There are four buttons 0,1,2,3
#define MESSAGE_MAX 50 //The time that it will show a message
#define MESSAGE_END 100 //Should be twice message max
#define HALF 2 //for dividing by two
#define INIT_VALUE 0 //For initializing things to zero
#define INIT_VALUE_ONE 1 //For initlaizing things to one
#define TIME_OUT_ERROR_MESSAGE "Game Over. \n You took too long." //Its the time out error message
#define USER_INPUT_ERROR_MESSAGE "Game Over. \n Incorrect Color."
#define INCREMENT_ITER 1

//Welcome Screen display properties
#define SIMON_NAME_CURSOR_X 50
#define SIMON_NAME_CURSOR_Y 60
#define SIMON_NAME_TEXT_SIZE 4
#define TOUCH_SCREEN_CURSOR_X 40
#define TOUCH_SCREEN_CURSOR_Y 130
#define TOUCH_SCREEN_TEXT_SIZE 2

//Message screen display properties
#define MESSAGE_TEXT_SIZE 3
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH 320
#define TEXT_WIDTH 6 * MESSAGE_TEXT_SIZE
#define MESSAGE_SIZE 100
#define DRAW false
#define ERASE true
#define EDGE_BUFFER 15

// States for the controller state machine.
enum simonControl_st_t {

    init_st, // Start here, transition out of this state on the first tick.
    welcome_st, //Displays the welcome screen
    flash_st, //Flashes the pattern
    verify_st, //Verifies sequence
    end_level_st, //When a level is finished
    error_st //If the user makes an error

} simonControlCurrentState;



static uint16_t endLevelCounter = 0; //Counts time spent on end level
static uint16_t level = 0; //Keeps track of what level
static uint16_t messageCounter = 0; //Counts for message length
static bool timeOutError = false; //Time out error flag
static bool userInputError = false; //User input error flag
char message[MESSAGE_SIZE]; //For the messages
static uint16_t longestSequence = 0; //Keeps track of your longest sequence
static uint16_t sRandCounter; //Used for seeding the random generator

///////Helper Functions//////////

void simonControl_generateSequence(){

    //How long the sequence should be
    uint16_t levelSequenceLength = level + LEVEL_SEQ_OFFSET;

    //Increments the counter so we get a different sequence everytime.
    sRandCounter++;

    printf("SrandCounter: %d\n", sRandCounter);

    //Seeds the random number generator
    srand(sRandCounter);

    //Makes an array for the flash sequence
    uint8_t flashSequence[levelSequenceLength];

    //Makes a new sequence
    for(int i = 0; i < levelSequenceLength; i++){

        //Puts a random button in the array
        flashSequence[i] = rand() % BUTTON_COUNT;

        printf("Sequence at %d : %d\n", i, flashSequence[i]);

    }

    //Sets the sequence
    globals_setSequence(flashSequence, levelSequenceLength);

    //Sets the iteration length to zero
    globals_setSequenceIterationLength(INIT_VALUE_ONE);

}

//Prints a message
void simonControl_printMessage(char message[], bool erase){

    //The text color
    uint16_t textColor = DISPLAY_WHITE;

    //If we want to erase it
    if(erase){

        //Sets the text color to black
        textColor = DISPLAY_BLACK;

    }

    //calculates y cursor
    uint16_t cursorY = SCREEN_HEIGHT/HALF;

    //calculates x cursor
    //int16_t cursorX = (SCREEN_WIDTH/HALF) - ((strlen(message)/HALF)*TEXT_WIDTH);

    //Sets the x cursor
    int16_t cursorX = EDGE_BUFFER;

    //Checks to see if it has gone out of bounds
    if(cursorX < INIT_VALUE){

        //Sets the x cursor to the edge
        cursorX = EDGE_BUFFER;

    }

    printf("Cursor X: %d\n", cursorX);
    //Sets the cursors
    display_setCursor(cursorX, cursorY);

    //Sets the color
    display_setTextColor(textColor);

    //Prints
    display_println(message);
}

//Prints the state
void debugStatePrintSC() {
  static simonControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != simonControlCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = simonControlCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(simonControlCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("SC_init_st\n\r");
        break;
      case flash_st:
        printf("SC_flash_st\n\r");
        break;
      case welcome_st:
        printf("SC_welcome_st\n\r");
        break;
      case end_level_st:
        printf("SC_end_level_st\n\r");
        break;
      case error_st:
        printf("SC_error_st\n\r");
        break;
      case verify_st:
        printf("SC_verify_st\n\r");
        break;

     }
  }
}
///////.h Functions///////////

//Sets the up the welcome display
void simonControl_welcomeDisplay(bool firstTime, bool erase){

    //If this is the first time the welcome displays is being placed.
    if(firstTime){
        display_init(); // Always have to init the display.
        display_fillScreen(DISPLAY_BLACK); //Fills the screen with black
    }

    //For the textcolor
    uint16_t textColor;

    //Is it supposed to erase
    if(erase){

        //Black
        textColor = DISPLAY_BLACK;

    } else {

        //White
        textColor = DISPLAY_WHITE;

    }

    //Sets the text color
    display_setTextColor(textColor);

    //Sets the cursor
    display_setCursor(SIMON_NAME_CURSOR_X, SIMON_NAME_CURSOR_Y);

    //Sets the text size
    display_setTextSize(SIMON_NAME_TEXT_SIZE);

    //Prints simon says
    display_println("SIMON SAYS");

    //Sets the cursor
    display_setCursor(TOUCH_SCREEN_CURSOR_X, TOUCH_SCREEN_CURSOR_Y);

    //Sets the text size
    display_setTextSize(TOUCH_SCREEN_TEXT_SIZE);

    //Prints simon says
    display_println("Touch Screen To Play");

}

// Initialize the state machine
void simonControl_init(){

    //Sets the current state to init
    simonControlCurrentState = init_st;

    //Sets the counter to 0
    sRandCounter = INIT_VALUE;


}

// Standard tick function.
void simonControl_tick(){


    debugStatePrintSC();

    // Perform state update first.
     switch(simonControlCurrentState) {

     //Initial state
       case init_st:

           //Sets the current state
           simonControlCurrentState = welcome_st;

         break;

         //welcome state
       case welcome_st:

           //If the display is touched
           if(display_isTouched()){

               //sets the current state
               simonControlCurrentState = flash_st;

               // Initialize the flashSequence state machine
               flashSequence_init();

               // Enable the flashSequence state machine.
               flashSequence_enable();

               //Erases the message
               simonControl_welcomeDisplay(false, ERASE);

               //Draws the buttons
               simonDisplay_drawAllButtons();

               //sets the iteration length
               globals_setSequenceIterationLength(INIT_VALUE_ONE);

               //Generates the first sequence
               simonControl_generateSequence();
           }


         break;

         //flash state
       case flash_st:

           //Ticks the flashSequence state machine
           flashSequence_tick();

           //Checks to see if the flash sequence is done
           if(flashSequence_isComplete()){

               // Interlock by first disabling the state machine.
               flashSequence_disable();

               // tick is necessary to advance the state.
               flashSequence_tick();

               // Finish the interlock by enabling the state machine.
               flashSequence_enable();

              //Sets the current state
               simonControlCurrentState = verify_st;

               // Initialize the flashSequence state machine
               verifySequence_init();

               // Enable the flashSequence state machine.
               verifySequence_enable();

           }


         break;

         //verify state
       case verify_st:

           // Advance the verifySequence state machine.
           verifySequence_tick();

           // Advance the buttonHandler state machine.
           buttonHandler_tick();

           // If the verifySequence state machine has finished, check the result,
           // otherwise just keep ticking both machines.
           if (verifySequence_isComplete()) {

               // Was the user too slow?
               if (verifySequence_isTimeOutError()) {

                   // Yes, set the error flag to true
                   timeOutError = true;

                   //Transition to error
                   simonControlCurrentState = error_st;

                   //Erases all the buttons
                   simonDisplay_eraseAllButtons();

                   //Copies the message to the string
                   sprintf(message, TIME_OUT_ERROR_MESSAGE);

                   //Prints the message
                   simonControl_printMessage(message, DRAW);

               // Did the user tap the wrong color?
               } else if (verifySequence_isUserInputError()) {

                   // Yes, set the flag.
                   userInputError = true;

                   //Transition to error
                   simonControlCurrentState = error_st;

                   //Erases all the buttons
                   simonDisplay_eraseAllButtons();

                   //Copies the message to the string
                   sprintf(message, USER_INPUT_ERROR_MESSAGE);

                   //Prints the message
                   simonControl_printMessage(message, DRAW);

               // User was correct if you get here.
               } else if(globals_getSequenceIterationLength() >  globals_getSequenceLength()) {

                   /*//Adds one to the sequence iteration length
                   globals_setSequenceIterationLength(iterationLength++);*/

                   //Checks to see if the there should be a new level

                       //Changes the state to end of level
                       simonControlCurrentState = end_level_st;

                       //Erases the buttons
                       simonDisplay_eraseAllButtons();

                       //Copies the message to the string
                       sprintf(message, "End of Level %d", level);

                       //Prints the message
                       simonControl_printMessage(message, DRAW);

               } else {

               // Interlock: first step of handshake.
               verifySequence_disable();

               // Advance the verifySequence machine.
               verifySequence_tick();

               // Interlock: second step of handshake.
               verifySequence_enable();

               //Adds one to the sequence iteration length
               globals_setSequenceIterationLength(globals_getSequenceIterationLength()+INCREMENT_ITER);

               //Checks to see if it is bigger
               if(globals_getSequenceIterationLength() > longestSequence){

                   //Sets the longest sequence
                   longestSequence = (globals_getSequenceIterationLength()- INCREMENT_ITER);
               }

               printf("Level: %d, Iteration Length: %d\n", level, globals_getSequenceIterationLength());

              //Sets the current state
              simonControlCurrentState = flash_st;

               }
           }



         break;

         //End of level
       case end_level_st:

           //checks to see if the counter is done
           if(endLevelCounter == END_LEVEL_MAX){

               //Copies the message to the string
               sprintf(message, "End of Level %d", level);

               //Erases the message
               simonControl_printMessage(message, ERASE);

               //Increments the level
               level++;

               //Changes the state
               simonControlCurrentState = flash_st;

               //Generates new sequence
               simonControl_generateSequence();

               //Sets the counter to zero
               endLevelCounter = INIT_VALUE;

               //Sets it to 1
               globals_setSequenceIterationLength(INIT_VALUE_ONE);

               //Draws the buttons
               simonDisplay_drawAllButtons();

           }


       break;

       //Error message
       case error_st:

           //If it should change messages
           if(messageCounter == MESSAGE_MAX){

               //Was it a time out error?
               if(timeOutError){

                   //Copies the message to the string
                   sprintf(message, TIME_OUT_ERROR_MESSAGE);

               } else {

                   //Copies the message to the string
                   sprintf(message, USER_INPUT_ERROR_MESSAGE);

               }

               //Prints the message
               simonControl_printMessage(message, ERASE);

               //Generates string
               sprintf(message, "Level: %d\n Longest Length: %d", level, longestSequence);

               //Prints the message
               simonControl_printMessage(message, DRAW);
           }

           //If it should go to the welcome screen
           if(messageCounter == MESSAGE_END){

               //Changes state
               simonControlCurrentState = welcome_st;

               //Time out error flag false
               timeOutError = false;

               //User Input error flag false
               userInputError = false;

               //Generates string
               sprintf(message, "Level: %d\n Longest Length: %d", level, longestSequence);

               //ERASE the message
               simonControl_printMessage(message, ERASE);

               //Sets level to zero
               level = INIT_VALUE;

               //Sets the message counter to zero
               messageCounter = INIT_VALUE;

               //Shows the welcome screen
               simonControl_welcomeDisplay(false, DRAW);
           }

       break;

       default:

         printf("clockControl_tick state update: hit default\n\r");

         break;
     }

     // Perform state action next.
     switch(simonControlCurrentState) {

     //Init state
       case init_st:

           //Nothing

         break;

         //flash st
       case welcome_st:

           //Increments random seeder
           sRandCounter++;

         break;

         //flash st
       case flash_st:



         break;

       case verify_st:


         break;

       case end_level_st:

           //Increments the counter
           endLevelCounter++;

         break;

         //If the player makes a mistake
       case error_st:

           //Increments the counter
           messageCounter++;

           break;

        default:
         printf("clockControl_tick state action: hit default\n\r");
         break;
     }
}


