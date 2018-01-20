#include "wamControl.h"
#include "../lab2/buttons.h"
#include <stdio.h>
#include <stdlib.h>
#include "supportFiles/display.h"


#define MAX_WAIT 20 //Maximum waiting time
#define MIN_WAIT 10 //Minimum waiting time
#define ADC_MAX 1 //Adc counter max
#define MAX_MISS_SCORE 25 //How many moles can be missed
#define INIT_VALUE 0 //Initilizes some things to zero
#define MAX_ACTIVE_MOLES 1 //The max active moles is 1 to start out
#define INIT_LEVEL 1 //For init level
#define BINARY_MOD 2 //For making a random binary decision

uint16_t adcCounter = INIT_VALUE; //Counts for the adc

uint16_t wamControl_msPerTick = INIT_VALUE; //Keeps track of msPerTick

uint16_t maxActiveMoles = INIT_VALUE; //Max active moles at a time

uint16_t maxMissCount = INIT_VALUE; //Max miss count possible

uint16_t maxWait = MAX_WAIT; //Max wait count for a mole

uint16_t minWait = MIN_WAIT; //Min wait count for a mole

uint16_t lastLevel = INIT_LEVEL;

// States for the controller state machine.
enum wam_st_t {
    init_st,                 // Start here, transition out of this state on the first tick.
    wait_for_touch_st,      //Waits for touch
    adc_st        // ADC


} wamCurrentState;


//***************Helper Function*****************//

//Checks active moles
void wamControl_checkActive(){

    //Checks to see if there are enough active moles
    if(wamDisplay_getActiveMoleCount() < maxActiveMoles){

        //Activate a random mole
        wamDisplay_activateRandomMole();
    }
}


// Call this before using any wamControl_ functions.
void wamControl_init(){

    //Sets the current state to init
    wamCurrentState = init_st;
}

// Call this to set how much time is consumed by each tick of the controlling state machine.
// This information makes it possible to set the awake and sleep time of moles in ms, not ticks.
void wamControl_setMsPerTick(uint16_t msPerTick){

    //sets mspertick
    wamControl_msPerTick = msPerTick;

}

// This returns the time consumed by each tick of the controlling state machine.
uint16_t wamControl_getMsPerTick(){

    //Returns
    return wamControl_msPerTick;

}

//For debug
void debugStatePrintWAM() {
  static wam_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != wamCurrentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = wamCurrentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(wamCurrentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case wait_for_touch_st:
        printf("wait_for_touch_st\n\r");
        break;
      case adc_st:
        printf("adc_st\n\r");
        break;
     }
  }
}
// Standard tick function.
void wamControl_tick(){

    //Prints
    //debugStatePrintWAM();

    // Perform state update first.
      switch(wamCurrentState) {

      //The Initial state
        case init_st:

            //Resets active moles
            maxActiveMoles = MAX_ACTIVE_MOLES;

            //Sets the current state
            wamCurrentState = wait_for_touch_st;

          break;

        //Waits for a touch
      case wait_for_touch_st:

          //Resets
          adcCounter = INIT_VALUE;

          //Checks the display
          if(display_isTouched()){

              //Changes state
              wamCurrentState = adc_st;

          }

      break;

      //ADC
        case adc_st:

            //Checks to see if the adc is done
            if(adcCounter == ADC_MAX){

                //X coordinate of touch
                      int16_t x;

                      //Y coordinate of touch
                      int16_t y;

                      //Z coordinate of touch
                      uint8_t z;

                  //Gets the touchedPoint and puts them into x, y, and z
                  display_getTouchedPoint(&x, &y, &z);

                  //Makes a point
                  wamDisplay_point_t point;

                  //Sets the x
                  point.x = x;

                  //Sets the y
                  point.y = y;

                  //Calls whack mole
                  wamDisplay_whackMole(&point);

                //Changes state
                wamCurrentState = wait_for_touch_st;

                //clears old touch data
                display_clearOldTouchData();
            }

        break;

        default:

            //Prints
          printf("clockControl_tick state update: hit default\n\r");

          break;
      }

      // Perform state action next.
      switch(wamCurrentState) {
          case init_st:


            break;

          case wait_for_touch_st:

              //Updates all the mole tick counts
              wamDisplay_updateAllMoleTickCounts();

              //Checks active moles
              wamControl_checkActive();

              //Checks the level
              if(lastLevel != wamDisplay_getLevel()){

                  //Sets the last level
                  lastLevel = wamDisplay_getLevel();

                  //Random changes one
                  if(rand() % BINARY_MOD){

                     //Decrements maxWait
                      maxWait--;

                      //Decrements
                      minWait--;


                  } else {

                      //Increments
                      maxActiveMoles++;
                  }

              }

             break;

            //The beginning of the game
          case adc_st:

              //Updates the tick counts
              wamDisplay_updateAllMoleTickCounts();

              //Checks active moles
              wamControl_checkActive();

              //Increments counter
              adcCounter++;

            break;

          default:
            printf("clockControl_tick state update: hit default\n\r");
            break;
      }
}

// Returns a random value that indicates how long the mole should sleep before awaking.
wamDisplay_moleTickCount_t wamControl_getRandomMoleAsleepInterval(){

    //Generates a random interval
    wamDisplay_moleTickCount_t tickCount = (rand() % MAX_WAIT) + MIN_WAIT;

    //Returns
    return tickCount;

}

// Returns a random value that indicates how long the mole should stay awake before going dormant.
wamDisplay_moleTickCount_t wamControl_getRandomMoleAwakeInterval(){

    //Generate a random interval
    wamDisplay_moleTickCount_t tickCount = (rand() % MAX_WAIT) + MIN_WAIT;

    //Returns
    return tickCount;
}

// Set the maximum number of active moles.
void wamControl_setMaxActiveMoles(uint16_t count){

    //Sets the max active moles
    maxActiveMoles = count;

}

// Get the current allowable count of active moles.
uint16_t wamControl_getMaxActiveMoles(){

    //Returns
    return maxActiveMoles;
}

// Set the seed for the random-number generator.
void wamControl_setRandomSeed(uint32_t seed){

    //Seeds the random
    srand(seed);
}

// Set the maximum number of misses until the game is over.
void wamControl_setMaxMissCount(uint16_t missCount){

    //Sets max miss count
    maxMissCount = missCount;

}

// Use this predicate to see if the game is finished.
bool wamControl_isGameOver(){

    //Checks to see if the miss score is at the max
    if(wamDisplay_getMissScore() >= MAX_MISS_SCORE){

        //Returns true
        return true;

    } else {

        //Returns false
        return false;
    }

}
