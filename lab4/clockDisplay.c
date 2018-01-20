#include "supportFiles/display.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "supportFiles/utils.h"

#define COLOR DISPLAY_GREEN //Makes the default color green
#define CLOCK_TEXT_SIZE 6 //Size of the characters
#define CENTER_POINT_X  160 //X coordinate of the center of the screen
#define CENTER_POINT_Y  120 //Y coordinate of the center of the screen
#define CHARS_LEFT_OF_CENTER 4 //How many characters are to the left of the center point
#define CHARS_RIGHT_OF_CENTER 4 //How many characters are to the right of the center point
#define CHAR_WIDTH DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE
#define HALF_CHAR_HEIGHT ((DISPLAY_CHAR_HEIGHT)/ 2) * CLOCK_TEXT_SIZE
#define TEXT_CURSOR_X CENTER_POINT_X - (CHARS_LEFT_OF_CENTER * DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE) //The X Coordinate for the text cursor
#define TEXT_CURSOR_Y CENTER_POINT_Y - (HALF_CHAR_HEIGHT) //The Y Coordinate for the text cursor
//#define COLON_OFFSET_X DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE)
#define BLANK_SPACE_Y ((DISPLAY_CHAR_HEIGHT /6) * CLOCK_TEXT_SIZE)
#define TRIANGLE_HEIGHT DISPLAY_CHAR_HEIGHT * CLOCK_TEXT_SIZE
#define TRIANGLE_HALF_WIDTH DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE
#define LEFT_BOUND CENTER_POINT_X - (CHARS_LEFT_OF_CENTER * DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE)
#define RIGHT_BOUND CENTER_POINT_X + (CHARS_LEFT_OF_CENTER * DISPLAY_CHAR_WIDTH * CLOCK_TEXT_SIZE)
#define TOP_BOUND CENTER_POINT_Y - (HALF_CHAR_HEIGHT + BLANK_SPACE_Y + TRIANGLE_HEIGHT)
#define BOTTOM_BOUND CENTER_POINT_Y + (HALF_CHAR_HEIGHT + BLANK_SPACE_Y + TRIANGLE_HEIGHT)
#define UP_TRIANGLE_BOTTOM TOP_BOUND + TRIANGLE_HEIGHT - BLANK_SPACE_Y
#define DOWN_TRIANGLE_TOP BOTTOM_BOUND - TRIANGLE_HEIGHT + BLANK_SPACE_Y
#define LEFT_TRIANGLE_LEFT LEFT_BOUND
#define LEFT_TRIANGLE_MIDDLE LEFT_BOUND + TRIANGLE_HALF_WIDTH
#define LEFT_TRIANGLE_RIGHT LEFT_BOUND + 2 * TRIANGLE_HALF_WIDTH
#define MIDDLE_TRIANGLE_LEFT CENTER_POINT_X - TRIANGLE_HALF_WIDTH
#define MIDDLE_TRIANGLE_MIDDLE CENTER_POINT_X
#define MIDDLE_TRIANGLE_RIGHT CENTER_POINT_X + TRIANGLE_HALF_WIDTH
#define RIGHT_TRIANGLE_LEFT RIGHT_BOUND - 2 * TRIANGLE_HALF_WIDTH
#define RIGHT_TRIANGLE_MIDDLE RIGHT_BOUND - TRIANGLE_HALF_WIDTH
#define RIGHT_TRIANGLE_RIGHT RIGHT_BOUND

#define CHAR0_CURSOR_X TEXT_CURSOR_X //X cursor position for character 0 in the time string
#define CHAR1_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH  //X cursor position for character 1 in the time string
#define CHAR2_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 2 //X cursor position for character 2 in the time string
#define CHAR3_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 3//X cursor position for character 3 in the time string
#define CHAR4_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 4//X cursor position for character 4 in the time string
#define CHAR5_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 5//X cursor position for character 5 in the time string
#define CHAR6_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 6//X cursor position for character 6 in the time string
#define CHAR7_CURSOR_X TEXT_CURSOR_X + CHAR_WIDTH * 7//X cursor position for character 7 in the time string

#define CHAR0_INDEX 0 //Index of char 0
#define CHAR1_INDEX 1 //Index of char 1
#define CHAR2_INDEX 2 //Index of char 2
#define CHAR3_INDEX 3 //Index of char 3
#define CHAR4_INDEX 4 //Index of char 4
#define CHAR5_INDEX 5 //Index of char 5
#define CHAR6_INDEX 6 //Index of char 6
#define CHAR7_INDEX 7 //Index of char 7

//Triangle Points start with left then top then right and are labeled 1,2,3 respectively
//Vertex definitions for the hour increment triangle
#define LEFT_TRIANGLE_UP_1_X LEFT_TRIANGLE_LEFT //X Coordinate for vertex 1
#define LEFT_TRIANGLE_UP_1_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 1
#define LEFT_TRIANGLE_UP_2_X LEFT_TRIANGLE_MIDDLE//X Coordinate for vertex 2
#define LEFT_TRIANGLE_UP_2_Y TOP_BOUND //Y Coordinate for vertex 2
#define LEFT_TRIANGLE_UP_3_X LEFT_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define LEFT_TRIANGLE_UP_3_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 3

//Vertex definitions for the hour decrement triangle
#define LEFT_TRIANGLE_DOWN_1_X LEFT_TRIANGLE_LEFT //X Coordinate for vertex 1
#define LEFT_TRIANGLE_DOWN_1_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 1
#define LEFT_TRIANGLE_DOWN_2_X LEFT_TRIANGLE_MIDDLE //X Coordinate for vertex 2
#define LEFT_TRIANGLE_DOWN_2_Y BOTTOM_BOUND //Y Coordinate for vertex 2
#define LEFT_TRIANGLE_DOWN_3_X LEFT_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define LEFT_TRIANGLE_DOWN_3_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 3

//Vertex definitions for the minute increment triangle
#define MIDDLE_TRIANGLE_UP_1_X MIDDLE_TRIANGLE_LEFT //X Coordinate for vertex 1
#define MIDDLE_TRIANGLE_UP_1_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 1
#define MIDDLE_TRIANGLE_UP_2_X MIDDLE_TRIANGLE_MIDDLE//X Coordinate for vertex 2
#define MIDDLE_TRIANGLE_UP_2_Y TOP_BOUND //Y Coordinate for vertex 2
#define MIDDLE_TRIANGLE_UP_3_X MIDDLE_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define MIDDLE_TRIANGLE_UP_3_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 3

//Vertex definitions for the minute decrement triangle
#define MIDDLE_TRIANGLE_DOWN_1_X MIDDLE_TRIANGLE_LEFT //X Coordinate for vertex 1
#define MIDDLE_TRIANGLE_DOWN_1_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 1
#define MIDDLE_TRIANGLE_DOWN_2_X MIDDLE_TRIANGLE_MIDDLE //X Coordinate for vertex 2
#define MIDDLE_TRIANGLE_DOWN_2_Y BOTTOM_BOUND //Y Coordinate for vertex 2
#define MIDDLE_TRIANGLE_DOWN_3_X MIDDLE_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define MIDDLE_TRIANGLE_DOWN_3_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 3

//Vertex definitions for the second increment triangle
#define RIGHT_TRIANGLE_UP_1_X RIGHT_TRIANGLE_LEFT //X Coordinate for vertex 1
#define RIGHT_TRIANGLE_UP_1_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 1
#define RIGHT_TRIANGLE_UP_2_X RIGHT_TRIANGLE_MIDDLE//X Coordinate for vertex 2
#define RIGHT_TRIANGLE_UP_2_Y TOP_BOUND //Y Coordinate for vertex 2
#define RIGHT_TRIANGLE_UP_3_X RIGHT_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define RIGHT_TRIANGLE_UP_3_Y UP_TRIANGLE_BOTTOM //Y Coordinate for vertex 3

//Vertex definitions for the second decrement triangle
#define RIGHT_TRIANGLE_DOWN_1_X RIGHT_TRIANGLE_LEFT //X Coordinate for vertex 1
#define RIGHT_TRIANGLE_DOWN_1_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 1
#define RIGHT_TRIANGLE_DOWN_2_X RIGHT_TRIANGLE_MIDDLE //X Coordinate for vertex 2
#define RIGHT_TRIANGLE_DOWN_2_Y BOTTOM_BOUND //Y Coordinate for vertex 2
#define RIGHT_TRIANGLE_DOWN_3_X RIGHT_TRIANGLE_RIGHT //X Coordinate for vertex 3
#define RIGHT_TRIANGLE_DOWN_3_Y DOWN_TRIANGLE_TOP //Y Coordinate for vertex 3

#define TIME_ARRAY_LENGTH 9 //Length of the time array

#define TOUCH_MIDDLE_LINE_Y 120 //The middle of the screen on the y axis
#define TOUCH_LEFT_LINE_X 106 //Left third bound on x axis
#define TOUCH_RIGHT_LINE_X 212 //Right third bound on x axis
#define TOUCH_DELAY_MS 50

#define HOURS 0 //indicators for hours
#define MINUTES 1 //indicator for minutes
#define SECONDS 2 //indicator for seconds
#define HOURS_MAX 12 //Max amount of hours
#define TIME_MIN 0 // Least amount of time for seconds, and minutes
#define HOUR_MIN 1 //Least amount of time for hours
#define MINUTES_MAX 59 //Max amount of minutes
#define SECONDS_MAX 59 //Max amount of seconds
#define INCREMENT 1 //Flag for incrementing
#define DECREMENT 0 //Flag for decrementing

#define HOUR_INIT_VALUE 10
#define MINUTE_INIT_VALUE 58
#define SECOND_INIT_VALUE 58

#define FORCE_UPDATE_ALL 1 //If all the characters should update
#define NO_FORCE_UPDATE_ALL 0 //If all the characters should ot update

char previousTime[TIME_ARRAY_LENGTH]; //Array for holding the previous time
char currentTime[TIME_ARRAY_LENGTH]; //Array for holding the current time

uint8_t hours = HOUR_INIT_VALUE; //int for holding the hours
uint8_t minutes = MINUTE_INIT_VALUE; //int for holding minutes
uint8_t seconds = SECOND_INIT_VALUE; //int for holding seconds

//Gets the necessary cursor for the char
int clockDisplay_getCharCursor(int index){

        int cursor_x = 0; //Creates a holder for the cursor_x


    switch(index){
        //Checks for Char0
        case CHAR0_INDEX:

            //Sets temp cursor x to char 0 x
            cursor_x = CHAR0_CURSOR_X;

        break;

        //Checks for Char0
        case CHAR1_INDEX:

            //Sets temp cursor x to char 1 x
            cursor_x = CHAR1_CURSOR_X;


        break;

        //Checks for Char2
        case CHAR2_INDEX:

            //Sets temp cursor x to char 2 x
            cursor_x = CHAR2_CURSOR_X;

        break;

        //Checks for Char3
        case CHAR3_INDEX:

            //Sets temp cursor x to char 3 x
            cursor_x = CHAR3_CURSOR_X;

        break;

        //Checks for Char4
        case CHAR4_INDEX:

            //Sets temp cursor x to char 4 x
            cursor_x = CHAR4_CURSOR_X;

        break;

        //Checks for Char5
        case CHAR5_INDEX:

            //Sets temp cursor x to char 5 x
            cursor_x = CHAR5_CURSOR_X;

        break;

        //Checks for Char6
        case CHAR6_INDEX:

            //Sets temp cursor x to char 6 x
            cursor_x = CHAR6_CURSOR_X;

        break;

        //Checks for Char7
        case CHAR7_INDEX:

            //Sets temp cursor x to char 7 x
            cursor_x = CHAR7_CURSOR_X;

        break;

    }


    return cursor_x;
}

//Redraws one character
void clockDisplay_redrawChar(int coordinate, char previousChar, char currentChar){
    //Sets cursor
    display_setCursor(coordinate, TEXT_CURSOR_Y);

    //Sets text color to black
    display_setTextColor(DISPLAY_BLACK);

    //Draws the previous character in black
    display_println(previousChar);

    //Changes the text color to the default color
    display_setTextColor(COLOR);

    //Sets cursor
    display_setCursor(coordinate, TEXT_CURSOR_Y);

    //Draws currentChar
    display_println(currentChar);
}

//Makes sure we don't have impossible times
void clockDisplay_incDecCheck(int partOfTime, int incDec){

    if(incDec == INCREMENT){

        //Looks at which part of time it should deal with
        switch(partOfTime){

        //If hours
            case HOURS:

                //If hours is ready to roll over
                if(hours == HOURS_MAX){

                    //Sets hours to zero
                    hours = HOUR_MIN;

                } else {

                    //Increments the hours
                    hours++;
                }

                break;

            case MINUTES:

                //If minutes is ready to roll over
                if(minutes == MINUTES_MAX){

                    //Sets minutes to zero
                    minutes = TIME_MIN;

                } else {

                    //Increments the minutes
                    minutes++;

                }

                break;


            case SECONDS:
                //If seconds is ready to roll over
                if(seconds == SECONDS_MAX){

                    //Sets seconds to zero
                    seconds = TIME_MIN;

                } else {

                    //Increments the seconds
                    seconds++;

                }

                break;

            }

        //For decrement
    } else {

        //Looks at which part of time it should deal with
                switch(partOfTime){

                //If hours
                    case HOURS:

                        //If hours is ready to roll over
                        if(hours == HOUR_MIN){

                            //Sets hours to max
                            hours = HOURS_MAX;

                        } else {

                            //Decrements the hours
                            hours--;
                        }

                        break;

                    case MINUTES:

                        //If hours is ready to roll over
                        if(minutes == TIME_MIN){

                            //Sets hours to zero
                            minutes = MINUTES_MAX;

                        } else {

                            //Increments the hours
                            minutes--;

                        }

                        break;


                    case SECONDS:
                        //If hours is ready to roll over
                        if(seconds == TIME_MIN){

                            //Sets hours to zero
                            seconds = SECONDS_MAX;

                        } else {

                            //Increments the hours
                            seconds--;

                        }

                        break;

                    }
    }

}

//Initializes the display
void clockDisplay_init(){
    //Sets the current time to 0
    sprintf(currentTime, "%2hd:%02hd:%02hd", hours, minutes, seconds);

    //Sets the previous time to 0
    sprintf(previousTime, "%2hd:%02hd:%02hd", hours, minutes, seconds);

    display_init();  // Must init all of the software and underlying hardware for LCD.
    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.
    display_setCursor(TEXT_CURSOR_X, TEXT_CURSOR_Y); // Sets the text cursor to the calculated position
    display_setTextColor(COLOR);  // Make the text colored based on the COLOR defined.
    display_setTextSize(CLOCK_TEXT_SIZE); //Set the text size to CLOCK_TEXT_SIZE
    display_println(currentTime); //Displays the time

    //Draws hour increment triangle
    display_fillTriangle(LEFT_TRIANGLE_UP_1_X, LEFT_TRIANGLE_UP_1_Y, LEFT_TRIANGLE_UP_2_X, LEFT_TRIANGLE_UP_2_Y,
            LEFT_TRIANGLE_UP_3_X, LEFT_TRIANGLE_UP_3_Y, COLOR);

    //Draws hour decrement triangle
    display_fillTriangle(LEFT_TRIANGLE_DOWN_1_X, LEFT_TRIANGLE_DOWN_1_Y, LEFT_TRIANGLE_DOWN_2_X, LEFT_TRIANGLE_DOWN_2_Y,
                LEFT_TRIANGLE_DOWN_3_X, LEFT_TRIANGLE_DOWN_3_Y, COLOR);

    //Draws minute increment triangle
    display_fillTriangle(MIDDLE_TRIANGLE_UP_1_X, MIDDLE_TRIANGLE_UP_1_Y, MIDDLE_TRIANGLE_UP_2_X, MIDDLE_TRIANGLE_UP_2_Y,
            MIDDLE_TRIANGLE_UP_3_X, MIDDLE_TRIANGLE_UP_3_Y, COLOR);

    //Draws minute decrement triangle
    display_fillTriangle(MIDDLE_TRIANGLE_DOWN_1_X, MIDDLE_TRIANGLE_DOWN_1_Y, MIDDLE_TRIANGLE_DOWN_2_X, MIDDLE_TRIANGLE_DOWN_2_Y,
                MIDDLE_TRIANGLE_DOWN_3_X, MIDDLE_TRIANGLE_DOWN_3_Y, COLOR);

    //Draws second increment triangle
    display_fillTriangle(RIGHT_TRIANGLE_UP_1_X, RIGHT_TRIANGLE_UP_1_Y, RIGHT_TRIANGLE_UP_2_X, RIGHT_TRIANGLE_UP_2_Y,
            RIGHT_TRIANGLE_UP_3_X, RIGHT_TRIANGLE_UP_3_Y, COLOR);

    //Draws seconds decrement triangle
    display_fillTriangle(RIGHT_TRIANGLE_DOWN_1_X, RIGHT_TRIANGLE_DOWN_1_Y, RIGHT_TRIANGLE_DOWN_2_X, RIGHT_TRIANGLE_DOWN_2_Y,
                RIGHT_TRIANGLE_DOWN_3_X, RIGHT_TRIANGLE_DOWN_3_Y, COLOR);

}

//Updates the Display
void clockDisplay_updateTimeDisplay(bool forceUpdateAll){
    //Updates the currentTime
    sprintf(currentTime, "%2hd:%02hd:%02hd", hours, minutes, seconds);

    //Checks to see if forceUpdateAll is true
    if(forceUpdateAll){

        //Sets the cursor
        display_setCursor(TEXT_CURSOR_X, TEXT_CURSOR_Y);

        //Sets the color to black
        display_setTextColor(DISPLAY_BLACK);

        //Prints the previous time
        display_println(previousTime);

        //Sets the cursor
        display_setCursor(TEXT_CURSOR_X, TEXT_CURSOR_Y);

        //Sets the color to default color
        display_setTextColor(COLOR);

        //Prints the previous time
        display_println(currentTime);


    } else {

        //Checks to see if all indices of updateTime are the same as previousTime
        for(int i = 0; i < TIME_ARRAY_LENGTH; i++){

            //Checks to see if the character at index i in currentTime is equal to previousTime
            if(currentTime[i] != previousTime[i]){

                //Gets the cursor for the character and then redraws it
                clockDisplay_redrawChar(clockDisplay_getCharCursor(i), previousTime[i], currentTime[i]);

            }
        }
    }

   //Set previous time to current time
    memcpy(previousTime, currentTime, strlen(currentTime)+1);

}

//Increments or decrements a given value
void clockDisplay_performIncDec(){

    int16_t x,y; //The  x and y coordinates for the touch
    uint8_t z; //The pressure of the touch

    //Gets the touchedPoint and puts them into x, y, and z
    display_getTouchedPoint(&x, &y, &z);

    //Clears any previous touch data
    display_clearOldTouchData();

    //Check to see which half of the screen the touch is on by y axis
    if(y < TOUCH_MIDDLE_LINE_Y){

        //Is the x position on the right third?
        if(x > TOUCH_RIGHT_LINE_X){

            //Increments seconds
            clockDisplay_incDecCheck(SECONDS, INCREMENT);

            //Redraws the time
            clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);

        //Is the x position in the middle?
        } else if(x > TOUCH_LEFT_LINE_X){

            //Increments minutes
            clockDisplay_incDecCheck(MINUTES, INCREMENT);

            //Redraws the time
            clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);

        //The x position is on the left third
        } else {

            //Increments hours
            clockDisplay_incDecCheck(HOURS, INCREMENT);

            //Redraws the time
            clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
        }

    //The touch is on the bottom half of the screen
    } else {

        //Is the x position on the right third?
                if(x > TOUCH_RIGHT_LINE_X){

                    //Decrements seconds
                    clockDisplay_incDecCheck(SECONDS, DECREMENT);

                    //Redraws the time
                    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);

                //Is the x position in the middle?
                } else if(x > TOUCH_LEFT_LINE_X){

                    //Decrements minutes
                    clockDisplay_incDecCheck(MINUTES, DECREMENT);

                    //Redraws the time
                    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);

                //The x position is on the left third
                } else {

                    //Decrements hours
                    clockDisplay_incDecCheck(HOURS, DECREMENT);

                    //Redraws the time
                    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
                }

    }


}

//Advances time by one Second
void clockDisplay_advanceTimeOneSecond(){



    //Checks to see if hours should roll over
    if(minutes == MINUTES_MAX && seconds == SECONDS_MAX){

            //Increment Hours
            clockDisplay_incDecCheck(HOURS, INCREMENT);

        }

    //Checks to see if it needs to roll over
    if(seconds == SECONDS_MAX){

        //Increment Minutes
        clockDisplay_incDecCheck(MINUTES, INCREMENT);

    }

    //Increments seconds by one
     clockDisplay_incDecCheck(SECONDS, INCREMENT);

    //Redraws the time
    clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);


}


#define TEST_LOOP_ITER 5
#define TEST_LOOP_ITER_10_SEC 100
#define TEST_DELAY_LONG 500
#define TEST_DELAY_SHORT 100


// Run a test of clock-display functions
void clockDisplay_runTest(){

    //Increments Hours
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Increments hours
        clockDisplay_incDecCheck(HOURS, INCREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Decrements Hours
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Decrements hours
        clockDisplay_incDecCheck(HOURS, DECREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Increments Minutes
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Increments Minutes
        clockDisplay_incDecCheck(MINUTES, INCREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Decrements Minutes
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Decrements MINUTES
        clockDisplay_incDecCheck(MINUTES, DECREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Increments seconds
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Increments seconds
        clockDisplay_incDecCheck(SECONDS, INCREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Decrements seconds
    for(int i = 0; i < TEST_LOOP_ITER; i++){
        //Add a delay of TEST_DELAY_LONG
        utils_msDelay(TEST_DELAY_LONG);

        //Decrements seconds
        clockDisplay_incDecCheck(SECONDS, DECREMENT);

        //Updates the display
        clockDisplay_updateTimeDisplay(NO_FORCE_UPDATE_ALL);
    }

    //Goes for 10 seconds
        for(int i = 0; i < TEST_LOOP_ITER_10_SEC; i++){
            //Add a delay of TEST_DELAY_SHORT
            utils_msDelay(TEST_DELAY_SHORT);

            //Adds one second
            clockDisplay_advanceTimeOneSecond();

        }

}
