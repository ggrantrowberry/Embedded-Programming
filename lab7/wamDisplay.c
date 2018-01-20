#include "supportFiles/display.h"
#include "wamDisplay.h"
#include "wamControl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "supportFiles/leds.h"
#include "../lab2/switches.h"  // Modify as necessary to point to your switches.h
#include "../lab2/buttons.h"   // Modify as necessary to point to your buttons.h


#define NINE_MOLES 9
#define SIX_MOLES 6
#define FOUR_MOLES 4
#define X_COORD_COUNT 3
#define Y_COORD_COUNT 3
#define RESET_VALUE 0
#define RESET_LEVEL 1
#define ERASE true //For erasing
#define DRAW false //For drawing
#define DECREMENT 1 //For decrementing ticks
#define HITS_LEVEL 5 //How many hits per level
#define INIT_VALUE 0 //For zero
#define MAX_ACTIVATE_MOLE_TRIES 20

//For Accessing indices
#define INDEX_0 0
#define INDEX_1 1
#define INDEX_2 2
#define INDEX_3 3
#define INDEX_4 4
#define INDEX_5 5
#define INDEX_6 6
#define INDEX_7 7
#define INDEX_8 8

//Display stuff
#define HEIGHT 200 //Height of screen
#define WIDTH 320 //Width of screen
#define LEFT_X WIDTH / 6 //Left x
#define MID_X WIDTH / 2 //Middle x
#define RIGHT_X (5 * WIDTH / 6) //Right x
#define TOP_Y HEIGHT / 6 //top y
#define MID_Y HEIGHT / 2 //mid y
#define BOTTOM_Y (5 * HEIGHT / 6) // bottom y
#define MOLE_RADIUS 30 //Radius of mole
#define GAME_BACKGROUND_X 0 //X
#define GAME_BACKGROUND_Y 0 //Y
#define GAME_BACKGROUND_HEIGHT HEIGHT //HEIGHT
#define GAME_BACKGROUND_WIDTH WIDTH //WIDTH

//Splash Screen display
#define TITLE_CURSOR_X 43
#define TITLE_CURSOR_Y 90
#define TITLE_TEXT_SIZE 3
#define SUB_TITLE_CURSOR_X 34
#define SUB_TITLE_CURSOR_Y 134
#define SUB_TITLE_TEXT_SIZE 2

//Game Stats Display
#define STATS_Y 224
#define STATS_TEXT_SIZE 2
#define HIT_LABEL_X 6
#define HIT_LABEL_Y STATS_Y
#define HIT_SCORE_X (HIT_LABEL_X + (STATS_TEXT_SIZE * 5 * 6))
#define HIT_SCORE_Y STATS_Y
#define MISS_LABEL_X (HIT_SCORE_X + (4 * 6 * STATS_TEXT_SIZE))
#define MISS_LABEL_Y STATS_Y
#define MISS_SCORE_X (MISS_LABEL_X + (5 * 6 * STATS_TEXT_SIZE))
#define MISS_SCORE_Y STATS_Y
#define LEVEL_LABEL_X (MISS_SCORE_X + (4 * 6 * STATS_TEXT_SIZE))
#define LEVEL_LABEL_Y STATS_Y
#define LEVEL_SCORE_X (LEVEL_LABEL_X + (6 * 6 * STATS_TEXT_SIZE))
#define LEVEL_SCORE_Y STATS_Y

//Touch boundaries
#define LEFT_COL (WIDTH / 3) //Left column inner boundary
#define RIGHT_COL (2 * WIDTH / 3) //Right column inner boundary
#define TOP_ROW (HEIGHT / 3) //Top row inner boundary
#define BOTTOM_ROW (2 * HEIGHT / 3) //Bottom row inner boundary


//Game Over display
#define GAME_OVER_X 79
#define GAME_OVER_Y 54
#define GAME_OVER_SIZE 3
#define GO_STATS_SIZE 2
#define GO_HITS_LABEL_X 112 //Label x
#define GO_HITS_LABEL_Y 90 //Label y
#define GO_HITS_SCORE_X (GO_HITS_LABEL_X + (GO_STATS_SIZE * 5 * 6)) //Score x
#define GO_HITS_SCORE_Y GO_HITS_LABEL_Y //Score y
#define GO_MISS_LABEL_X 100 //Label x
#define GO_MISS_LABEL_Y 112 //Label y
#define GO_MISS_SCORE_X (GO_MISS_LABEL_X + (GO_STATS_SIZE * 7 * 6)) //Score x
#define GO_MISS_SCORE_Y GO_MISS_LABEL_Y //Score y
#define GO_LEVEL_LABEL_X 82 //Label x
#define GO_LEVEL_LABEL_Y 134 //Label y
#define GO_LEVEL_SCORE_X (GO_LEVEL_LABEL_X + (GO_STATS_SIZE * 12 * 6)) //Score x
#define GO_LEVEL_SCORE_Y GO_LEVEL_LABEL_Y //Score y
#define GO_TOUCH_LABEL_X 40 //Label x
#define GO_TOUCH_LABEL_Y 162 //Label y

//static wamDisplay_moleCount_e moleCount;
static uint8_t numOfMoles = RESET_VALUE;

//Array for mole coordinates
static wamDisplay_point_t moleCoordinates[NINE_MOLES];

//Keeps track of misses
static uint16_t missScore = RESET_VALUE;

//Keeps track of hits
static uint16_t hitScore = RESET_VALUE;

//Keeps track of levels
static uint16_t level = RESET_VALUE;


/********************** typedefs **********************/
// This keeps track of all mole information.
typedef struct {
        wamDisplay_point_t origin;  // This is the origin of the hole for this mole.
        // A mole is active if either of the tick counts are non-zero. The mole is dormant otherwise.
        // During operation, non-zero tick counts are decremented at a regular rate by the control state machine.
        // The mole remains in his hole until ticksUntilAwake decrements to zero and then he pops out.
        // The mole remains popped out of his hole until ticksUntilDormant decrements to zero.
        // Once ticksUntilDomant goes to zero, the mole hides in his hole and remains dormant until activated again.
        wamDisplay_moleTickCount_t ticksUntilAwake;  // Mole will wake up (pop out of hole) when this goes from 1 -> 0.
        wamDisplay_moleTickCount_t ticksUntilDormant; // Mole will go dormant (back in hole) this goes 1 -> 0.
} wamDisplay_moleInfo_t;

// This will contain pointers to all of the mole info records.
// This will ultimately be treated as an array of pointers.
static wamDisplay_moleInfo_t** wamDisplay_moleInfo;



/**********************Helper Functions ***************/



//Fills the lookup table
void fillMoleCoordinates(){

    //Temp array
    wamDisplay_point_t moleCoordinatesTEMP[NINE_MOLES];

    //Sets up the x coordinate possibilities
    wamDisplay_coord_t xCoord[X_COORD_COUNT];

    //Sets up the y coordinate possibilities
    wamDisplay_coord_t yCoord[Y_COORD_COUNT];

    //Index 0
    xCoord[INDEX_0] = LEFT_X;

    //Index 1
    xCoord[INDEX_1] = MID_X;

    //Index 2
    xCoord[INDEX_2] = RIGHT_X;

    //Index 0
    yCoord[INDEX_0] = TOP_Y;

    //Index 1
    yCoord[INDEX_1] = MID_Y;

    //Index 2
    yCoord[INDEX_2] = BOTTOM_Y;

    //Index for points
    uint8_t pointIndex = INIT_VALUE;

    //Loops through x's
    for(int i = 0; i < X_COORD_COUNT; i++){

        //Loops through y's
        for(int j = 0; j < Y_COORD_COUNT; j++){

            //Makes a point
            wamDisplay_point_t point;

            //Sets the x
            point.x = xCoord[i];

            //Sets the y
            point.y = yCoord[j];

            //Puts the point in the array
            moleCoordinatesTEMP[pointIndex] = point;

            //Increments point index
            pointIndex++;
        }
    }


    /*
     * -------------
     * | 0 | 4 | 1 |
     * -------------
     * | 6 | 7 | 8 |
     * -------------
     * | 2 | 5 | 3 |
     * -------------
     */

    //Mole 0
    moleCoordinates[INDEX_0] = moleCoordinatesTEMP[INDEX_0];

    //Mole 1
    moleCoordinates[INDEX_1] = moleCoordinatesTEMP[INDEX_2];

    //Mole 2
    moleCoordinates[INDEX_2] = moleCoordinatesTEMP[INDEX_6];

    //Mole 3
    moleCoordinates[INDEX_3] = moleCoordinatesTEMP[INDEX_8];

    //Mole 4
    moleCoordinates[INDEX_4] = moleCoordinatesTEMP[INDEX_3];

    //Mole 5
    moleCoordinates[INDEX_5] = moleCoordinatesTEMP[INDEX_5];

    //Mole 6
    moleCoordinates[INDEX_6] = moleCoordinatesTEMP[INDEX_1];

    //Mole 7
    moleCoordinates[INDEX_7] = moleCoordinatesTEMP[INDEX_4];

    //Mole 8
    moleCoordinates[INDEX_8] = moleCoordinatesTEMP[INDEX_7];

}

//Checks to see if the place touched has an active mole
wamDisplay_moleIndex_t checkMole(wamDisplay_point_t* whackOrigin){

    //Determines what the mole is
    wamDisplay_moleIndex_t moleLocation = INIT_VALUE;

    //Is it in the left column
    if(whackOrigin->x < LEFT_COL){

        //Is it on the top row
        if(whackOrigin->y < TOP_ROW){

            //It is the top left mole
            moleLocation = INDEX_0;

            //Is it in the bottom row
        } else if(whackOrigin->y > BOTTOM_ROW){

            //It is the bottom left mole
            moleLocation = INDEX_2;

        } else {

            //It is the middle left mole
            moleLocation = INDEX_6;
        }

        //Is it in the right column
    } else if(whackOrigin->x > RIGHT_COL){

        //Is it on the top row
               if(whackOrigin->y < TOP_ROW){

                   //It is the top right mole
                   moleLocation = INDEX_1;

                   //Is it in the bottom row
               } else if(whackOrigin->y > BOTTOM_ROW){

                   //It is the bottom right mole
                   moleLocation = INDEX_3;

               } else {

                   //It is the middle left mole
                   moleLocation = INDEX_8;
               }


        //Is it in the middle column
    } else {

        //Is it on the top row
        if(whackOrigin->y < TOP_ROW){

            //It is the top middle mole
            moleLocation = INDEX_4;

            //Is it in the bottom row
        } else if(whackOrigin->y > BOTTOM_ROW){

            //It is the bottom middle mole
            moleLocation = INDEX_5;

        } else {

            //It is the middle middle mole
            moleLocation = INDEX_7;
        }
    }

    //Returns mole location
    return moleLocation;

}

void drawMole(wamDisplay_moleIndex_t moleLocation, bool erase){

    //The color to set the mole
    uint16_t color = DISPLAY_RED;

    //If it's supposed to erase
    if(erase){

        //Sets the color to black
        color = DISPLAY_BLACK;
    }

    //Draws a mole
    display_fillCircle(wamDisplay_moleInfo[moleLocation]->origin.x, wamDisplay_moleInfo[moleLocation]->origin.y, MOLE_RADIUS, color);

}

//Frees up the memory for the array
void deallocate(){

    // First deallocate all of the structs.
        for (uint16_t l=0; l<numOfMoles; l++) {

            // This deallocates the memory.
            free(wamDisplay_moleInfo[l]);

            // This step is not necessary but will keep you from reusing deallocated memory.
            wamDisplay_moleInfo[l] = NULL;

        }

        // Next, deallocate the array that contains the pointers to the structs.
        // Deallocates the container of arrays.
        free(wamDisplay_moleInfo);

        // Also keeps you from reusing the deallocated memory.
        wamDisplay_moleInfo = NULL;
}



/***********************.h Functions*******************/
// Allocates the memory for wamDisplay_moleInfo_t records.
// Computes the origin for each mole assuming a simple row-column layout:
// 9 moles: 3 rows, 3 columns, 6 moles: 2 rows, 3 columns, 4 moles: 2 rows, 2 columns
// Also inits the tick counts for awake and dormant.
void wamDisplay_computeMoleInfo() {

    // Setup all of the moles, creates and inits mole info records.
    // Create the container array. It contains pointers to each of the mole-hole info records.
    wamDisplay_moleInfo =  (wamDisplay_moleInfo_t**) malloc(numOfMoles * sizeof(wamDisplay_moleInfo_t*)); // Need to uncomment this line and initialize this variable via malloc().

    //Loops through and allocates moles
    for (uint16_t i=0; i< numOfMoles; i++) {

        // Second, allocate an instance of myStruct and point to it.
        wamDisplay_moleInfo[i] = (wamDisplay_moleInfo_t*) malloc(sizeof(wamDisplay_moleInfo_t));

    }

    //Assigns stuff to moles
    for (uint16_t j=0; j< numOfMoles; j++) {

        //Fills the origins of all the moles
        wamDisplay_moleInfo[j]->origin = moleCoordinates[j];

        //Sets the ticks until awake
        wamDisplay_moleInfo[j]->ticksUntilAwake = 0;

        //Sets the ticks until dormant
        wamDisplay_moleInfo[j]->ticksUntilDormant = 0;

       }

}


// Provide support to set games with varying numbers of moles. This function
// would be called prior to calling wamDisplay_init();
void wamDisplay_selectMoleCount(wamDisplay_moleCount_e moleCount){

    //Switches on moleCount
    switch(moleCount){

    //9 Moles
    case wamDisplay_moleCount_9:

        //Sets the number of moles to 9
        numOfMoles = NINE_MOLES;

    break;

    //6 Moles
    case wamDisplay_moleCount_6:

        //Sets the number of moles to 6
        numOfMoles = SIX_MOLES;

    break;

    //4 Moles
    case wamDisplay_moleCount_4:

        //Sets the number of moles to 4
        numOfMoles = FOUR_MOLES;

    break;

    default:

        //prints
        printf("You've done messed up!\n\r");

    break;

    }

}

// Call this before using any wamDisplay_ functions.
void wamDisplay_init(){

    //Initializes look up table
    fillMoleCoordinates();

    //Computes stuff
    wamDisplay_computeMoleInfo();

}

// Draw the game display with a background and mole holes.
void wamDisplay_drawMoleBoard(){

    //Fills the background
    display_fillRect(GAME_BACKGROUND_X, GAME_BACKGROUND_Y, GAME_BACKGROUND_WIDTH, GAME_BACKGROUND_HEIGHT, DISPLAY_BLUE);

    //Fills all the moles in with black
    for(int i = 0; i < numOfMoles; i++){

        //Draws all the mole holes
        display_fillCircle(moleCoordinates[i].x, moleCoordinates[i].y, MOLE_RADIUS, DISPLAY_BLACK);

    }

    //Sets the cursor for hit label
    display_setCursor(HIT_LABEL_X, HIT_LABEL_Y);

    //Draws hit label
    display_println("Hits:");

    //Sets the cursor for hit score
    display_setCursor(HIT_SCORE_X, HIT_SCORE_Y);

    //Draws hit score
    display_println(hitScore);

    //Sets the cursor for miss label
    display_setCursor(MISS_LABEL_X, MISS_LABEL_Y);

    //Draws miss label
    display_println("Miss:");

    //Sets the cursor for miss score
    display_setCursor(MISS_SCORE_X, MISS_SCORE_Y);

    //Draws miss score
    display_println(missScore);

    //Sets the cursor for level label
    display_setCursor(LEVEL_LABEL_X, LEVEL_LABEL_Y);

    //Draws level label
    display_println("Level:");

    //Sets the cursor for level score
    display_setCursor(LEVEL_SCORE_X, LEVEL_SCORE_Y);

    //Draws hit label
    display_println(level);

}

// Draw the initial splash (instruction) screen.
void wamDisplay_drawSplashScreen(){

    //Fills the screen with black
    display_fillScreen(DISPLAY_BLACK);

    display_setTextSize(TITLE_TEXT_SIZE);

    //Sets cursor
    display_setCursor(TITLE_CURSOR_X, TITLE_CURSOR_Y);

    //Prints
    display_println("Whack A Mole!");

    display_setTextSize(SUB_TITLE_TEXT_SIZE);

    //Sets cursor
    display_setCursor(SUB_TITLE_CURSOR_X, SUB_TITLE_CURSOR_Y);

    //Prints
    display_println("Touch Screen To Start");

}

// Draw the game-over screen.
void wamDisplay_drawGameOverScreen(){

    //Deallocate the memory
    deallocate();

    //Fills the screen black
    display_fillScreen(DISPLAY_BLACK);

    //Sets text size
    display_setTextSize(GAME_OVER_SIZE);

    //Sets cursor
    display_setCursor(GAME_OVER_X, GAME_OVER_Y);

    //Prints
    display_println("Game Over");

    //Sets text size
    display_setTextSize(GO_STATS_SIZE);

    //Sets cursor
    display_setCursor(GO_HITS_LABEL_X, GO_HITS_LABEL_Y);

    //Prints
    display_println("Hits:");

    //Sets cursor
    display_setCursor(GO_HITS_SCORE_X, GO_HITS_SCORE_Y);

    //Prints
    display_println(hitScore);

    //Sets cursor
    display_setCursor(GO_MISS_LABEL_X, GO_MISS_LABEL_Y);

    //Prints
    display_println("Misses:");

    //Sets cursor
    display_setCursor(GO_MISS_SCORE_X, GO_MISS_SCORE_Y);

    //Prints
    display_println(missScore);

    //Sets cursor
    display_setCursor(GO_LEVEL_LABEL_X, GO_LEVEL_LABEL_Y);

    //Prints
    display_println("Final Level:");

    //Sets cursor
    display_setCursor(GO_LEVEL_SCORE_X, GO_LEVEL_SCORE_Y);

    //Prints
    display_println(level);

    //Sets cursor
    display_setCursor(GO_TOUCH_LABEL_X, GO_TOUCH_LABEL_Y);

    //Prints
    display_println("(Touch To Try Again)");

}

// Selects a random mole and activates it.
// Activating a mole means that the ticksUntilAwake and ticksUntilDormant counts are initialized.
// See the comments for wamDisplay_moleInfo_t for details.
// Returns true if a mole was successfully activated. False otherwise. You can
// use the return value for error checking as this function should always be successful
// unless you have a bug somewhere.
bool wamDisplay_activateRandomMole(){

    for(int i = 0; i < MAX_ACTIVATE_MOLE_TRIES; i++){

        //gets a random index to activate
        wamDisplay_moleIndex_t index = (rand() % numOfMoles);

        //Checks to see if its not active
        if(wamDisplay_moleInfo[index]->ticksUntilAwake == INIT_VALUE || wamDisplay_moleInfo[index]->ticksUntilDormant == INIT_VALUE){

            //Gets a random interval
            wamDisplay_moleInfo[index]->ticksUntilAwake = wamControl_getRandomMoleAsleepInterval();

            //Gets a random interval
            wamDisplay_moleInfo[index]->ticksUntilDormant = wamControl_getRandomMoleAwakeInterval();

            break;
        }

    }
}

// This takes the provided coordinates and attempts to whack a mole. If a
// mole is successfully whacked, all internal data structures are updated and
// the display and score is updated. You can only whack a mole if the mole is awake (visible).
// The return value can be used during testing (you could just print which mole is
// whacked without having to implement the entire game).
wamDisplay_moleIndex_t wamDisplay_whackMole(wamDisplay_point_t* whackOrigin){

    //
    wamDisplay_moleIndex_t moleLocation = checkMole(whackOrigin);


    //Checks to see if the mole is active
    if(wamDisplay_moleInfo[moleLocation]->ticksUntilDormant != 0 && wamDisplay_moleInfo[moleLocation]->ticksUntilAwake == 0){

        //Sets it to zero
        wamDisplay_moleInfo[moleLocation]->ticksUntilDormant = INIT_VALUE;

        //Sets it to zero
        wamDisplay_moleInfo[moleLocation]->ticksUntilAwake = INIT_VALUE;

        //Updates hits
         wamDisplay_setHitScore(++hitScore);

         //Draws the mole
         drawMole(moleLocation, ERASE);

    }

    //Returns the mole
    return moleLocation;

}

// This updates the ticksUntilAwake/ticksUntilDormant clocks for all of the moles.
void wamDisplay_updateAllMoleTickCounts(){


    for(int i = 0; i < numOfMoles; i++){

        //Checks to see if it is 0
        if(wamDisplay_moleInfo[i]->ticksUntilAwake != 0){

            //Decrements
            wamDisplay_moleInfo[i]->ticksUntilAwake -= DECREMENT;

            //If it zero
            if(wamDisplay_moleInfo[i]->ticksUntilAwake == 0){

                //Draw
                drawMole(i, DRAW);
            }
        }

        //Checks to see if it is 0
        if(wamDisplay_moleInfo[i]->ticksUntilDormant != 0 && wamDisplay_moleInfo[i]->ticksUntilAwake == 0){

            //Decrements
            wamDisplay_moleInfo[i]->ticksUntilDormant -= DECREMENT;

            //If it zero
            if(wamDisplay_moleInfo[i]->ticksUntilDormant == 0){

                //draws
                drawMole(i, ERASE);

                //Increments missScore
                wamDisplay_setMissScore(++missScore);
            }

        }

    }

}

// Returns the count of currently active moles.
// A mole is active if it is not dormant, if:
// ticksUntilAwake or ticksUntilDormant are non-zero (in the moleInfo_t struct).
uint16_t wamDisplay_getActiveMoleCount(){

    uint16_t activeMoleCount = INIT_VALUE;

    //Loops through the moles
    for(int i = 0; i < numOfMoles; i++){

        //Checks to see if the mole is active
        if(wamDisplay_moleInfo[i]->ticksUntilAwake != 0 || wamDisplay_moleInfo[i]->ticksUntilDormant != 0){

            //Increases the count
            activeMoleCount++;

        }

    }

    //Returns the count
    return activeMoleCount;

}

// Sets the hit value in the score window.
void wamDisplay_setHitScore(uint16_t hits){

    //Sets the text color
    display_setTextColor(DISPLAY_BLACK);

    //Sets the size
    display_setTextSize(STATS_TEXT_SIZE);

    //Sets the cursor for hit score
    display_setCursor(HIT_SCORE_X, HIT_SCORE_Y);

    //Draws hit score
    display_println(--hitScore);

    //Sets the hit score
    hitScore = hits;

    //Sets the text color
    display_setTextColor(DISPLAY_WHITE);

    //Sets the cursor for hit score
    display_setCursor(HIT_SCORE_X, HIT_SCORE_Y);

    //Draws hit score
    display_println(hitScore);

    //Checks to see if levels should increment
    if(hitScore % HITS_LEVEL == INIT_VALUE){

        //Increments the level
        wamDisplay_incrementLevel();
    }

}

// Gets the current hit value.
uint16_t wamDisplay_getHitScore(){

    //returns hit score
    return hitScore;
}

// Sets the miss value in the score window.
void wamDisplay_setMissScore(uint16_t misses){

    //Sets text color
    display_setTextColor(DISPLAY_BLACK);

    //Sets text size
    display_setTextSize(STATS_TEXT_SIZE);

    //Sets the cursor for miss score
    display_setCursor(MISS_SCORE_X, MISS_SCORE_Y);

    //Draws miss score
    display_println(--missScore);

    //Sets missScore to misses
    missScore = misses;

    //Sets text color
    display_setTextColor(DISPLAY_WHITE);

    //Sets the cursor for miss score
    display_setCursor(MISS_SCORE_X, MISS_SCORE_Y);

    //Draws miss score
    display_println(missScore);
}

// Gets the miss value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getMissScore(){

    //Returns missScore
    return missScore;
}

// Sets the level value on the score board.
void wamDisplay_incrementLevel(){

    //Sets the text color
    display_setTextColor(DISPLAY_BLACK);

    //Sets the cursor for level score
    display_setCursor(LEVEL_SCORE_X, LEVEL_SCORE_Y);

    //Draws hit label
    display_println(level);

    //Increments level
    level++;

    //Sets the text color
    display_setTextColor(DISPLAY_WHITE);

    //Sets the cursor for level score
    display_setCursor(LEVEL_SCORE_X, LEVEL_SCORE_Y);

    //Draws hit label
    display_println(level);

}

// Retrieves the current level value.
// Can be used for testing and other functions.
uint16_t wamDisplay_getLevel(){

   //Returns level
   return level;
}

// Completely draws the score screen.
// This function renders all fields, including the text fields for "Hits" and "Misses".
// Usually only called once when you are initializing the game.
void wamDisplay_drawScoreScreen(){

    //Unnecessary
}

// Make this function available for testing purposes.
void wamDisplay_incrementMissScore(){

    //Increments the miss score
    missScore++;
}


// Reset the scores and level to restart the game.
void wamDisplay_resetAllScoresAndLevel(){

    //Resets the hit score
    hitScore = RESET_VALUE;

    //Resets the miss score
    missScore = RESET_VALUE;

    //Resets the level
    level = RESET_VALUE;

}

//*****************Test Code******************//

#define SWITCH_VALUE_9 9  // Binary 9 on the switches indicates 9 moles.
#define SWITCH_VALUE_6 6  // Binary 6 on the switches indicates 6 moles.
#define SWITCH_VALUE_4 4  // Binary 9 on the switches indicates 4 moles.
#define SWITCH_MASK 0xf   // Ignore potentially extraneous bits.

void wamMain_selectMoleCountFromSwitchesTest(uint16_t switchValue) {
    switch(switchValue & SWITCH_MASK) {
    case SWITCH_VALUE_9:    // this switch pattern = 9 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
        break;
    case SWITCH_VALUE_6:    // this switch pattern = 6 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_6);
        break;
    case SWITCH_VALUE_4:   // this switch pattern = 4 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_4);
        break;
    default:    // Any other pattern of switches = 9 moles.
        wamDisplay_selectMoleCount(wamDisplay_moleCount_9);
        break;
    }
}
// Test function that can be called from main() to demonstrate milestone 1.
// Invoking this function should provide the same behavior as shown in the Milestone 1 video.
#define SPLASH 0
#define GAME 1
#define GO 2


void wamDisplay_runMilestone1_test(){
    switches_init();  // Init the slide switches.
    buttons_init();   // Init the push buttons.
    leds_init(true);  // You need to init the LEDs so that LD4 can function as a heartbeat.
    wamDisplay_init();              // Initialize the WAM display.


    display_init();

    //Fills it black
    display_fillScreen(DISPLAY_BLACK);

    uint8_t screen = GAME;

    //Displays splash screen
    wamDisplay_drawSplashScreen();

    while(1){
        wamMain_selectMoleCountFromSwitchesTest(switches_read());  // Mole count selected via slide switches.

        //Checks to see if the display is touched
        if(display_isTouched()){

            //if it's on splash
            if(screen == SPLASH){

                //Displays splash screen
                wamDisplay_drawSplashScreen();

                //Goes to next screen
                screen++;

                //Is it game?
            } else if(screen == GAME){

                //Draws the board
                wamDisplay_drawMoleBoard();

                //Increments the screen
                screen++;
            } else {

                //Draws the game over screen
                wamDisplay_drawGameOverScreen();

                //Sets the screen to 0
                screen = SPLASH;
            }
        }

    }
}
