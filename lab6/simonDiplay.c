#include "simonDisplay.h"
#include "supportFiles/display.h"
#include "supportFiles/utils.h"
#include <stdio.h>


#define Y_HALF 120
#define X_HALF 160

//Button stuff
#define SIMON_DISPLAY_BUTTON_0_X 50 //Button 0 X
#define SIMON_DISPLAY_BUTTON_0_Y 30 //Button 0 Y
#define SIMON_DISPLAY_BUTTON_1_X 210 // Button 1 X
#define SIMON_DISPLAY_BUTTON_1_Y 30 // Button 1 Y
#define SIMON_DISPLAY_BUTTON_2_X 50 // Button 2 X
#define SIMON_DISPLAY_BUTTON_2_Y 150 // Button 2 Y
#define SIMON_DISPLAY_BUTTON_3_X 210 // Button 3 X
#define SIMON_DISPLAY_BUTTON_3_Y 150 // Button 3 Y
#define SIMON_DISPLAY_BUTTON_0_COLOR DISPLAY_RED //Red color
#define SIMON_DISPLAY_BUTTON_1_COLOR DISPLAY_YELLOW //Yellow
#define SIMON_DISPLAY_BUTTON_2_COLOR DISPLAY_BLUE //Blue
#define SIMON_DISPLAY_BUTTON_3_COLOR DISPLAY_GREEN //Green
#define NUMBER_OF_BUTTONS 4 //4 Buttons

//Square Stuff
#define SIMON_DISPLAY_SQUARE_0_X 20 //X coordinate
#define SIMON_DISPLAY_SQUARE_0_Y 0 //Y coordinate
#define SIMON_DISPLAY_SQUARE_1_X 180 //X coordinate
#define SIMON_DISPLAY_SQUARE_1_Y 0 //Y coordinate
#define SIMON_DISPLAY_SQUARE_2_X 20 //X coordinate
#define SIMON_DISPLAY_SQUARE_2_Y 120 //Y coordinate
#define SIMON_DISPLAY_SQUARE_3_X 180 //X coordinate
#define SIMON_DISPLAY_SQUARE_3_Y 120 //Y coordinate

#define ERASE false
#define DRAW true

////////////////Helper Functions//////////////////
int32_t simonDisplay_getButtonColor(uint8_t regionNumber){

    //Checks if it is region 0
    if(regionNumber == SIMON_DISPLAY_REGION_0){

        //Returns color red
        return SIMON_DISPLAY_BUTTON_0_COLOR;

        //Region 1
    } else if(regionNumber == SIMON_DISPLAY_REGION_1){

        //Returns color yellow
        return SIMON_DISPLAY_BUTTON_1_COLOR;

        //Region 2
    } else if(regionNumber == SIMON_DISPLAY_REGION_2){

        //Returns color blue
        return SIMON_DISPLAY_BUTTON_2_COLOR;

    } else {

        //Returns green
        return SIMON_DISPLAY_BUTTON_3_COLOR;
    }


}


//////////////// .h Functions//////////////////////
int8_t simonDisplay_computeRegionNumber(int16_t x, int16_t y){

    //Checks to see if it is the left half of the screen
    if(x < X_HALF){

        //Checks to see if it is in the top of the screen
        if(y < Y_HALF){

            //Returns region 0
            return SIMON_DISPLAY_REGION_0;

        } else {

            //Returns region 2
            return SIMON_DISPLAY_REGION_2;
        }

    //It is the right half of the screen
    } else {

        //Checks to see if it is in the top of the screen
        if(y < Y_HALF){

            //Returns region 1
            return SIMON_DISPLAY_REGION_1;

        } else {

            //Returns region 3
            return SIMON_DISPLAY_REGION_3;
        }
    }

}


// Draws a colored "button" that the user can touch.
// The colored button is centered in the region but does not fill the region.
// If erase argument is true, draws the button as black background to erase it.
void simonDisplay_drawButton(uint8_t regionNumber, bool erase){

    //The color
    int32_t color;

    //If erase set color to black
    if(erase){

        //Sets the color to black
        color = DISPLAY_BLACK;

    } else {

        //Gets the color
        color = simonDisplay_getButtonColor(regionNumber);

    }

    //Checks to see what region number
    if(regionNumber == SIMON_DISPLAY_REGION_0){

        //Draws rect
        display_fillRect(SIMON_DISPLAY_BUTTON_0_X, SIMON_DISPLAY_BUTTON_0_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, color);

        //Button 1
    } else if(regionNumber == SIMON_DISPLAY_REGION_1){

        //Draws rect
        display_fillRect(SIMON_DISPLAY_BUTTON_1_X, SIMON_DISPLAY_BUTTON_1_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, color);

        //Button 2
    } else if(regionNumber == SIMON_DISPLAY_REGION_2){

        //Draws rect
        display_fillRect(SIMON_DISPLAY_BUTTON_2_X, SIMON_DISPLAY_BUTTON_2_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, color);

        //Button 3
    } else {
        //Draws rect
        display_fillRect(SIMON_DISPLAY_BUTTON_3_X, SIMON_DISPLAY_BUTTON_3_Y, SIMON_DISPLAY_BUTTON_WIDTH, SIMON_DISPLAY_BUTTON_HEIGHT, color);

    }

}

// Convenience function that draws all of the buttons.
void simonDisplay_drawAllButtons(){

    //Loops throught the buttons
    for(int i = 0; i < NUMBER_OF_BUTTONS; i++){

        //Draws the button
        simonDisplay_drawButton(i, ERASE);
    }
}

// Convenience function that draws all of the buttons.
void simonDisplay_eraseAllButtons(){

    //Loops through the buttons
    for(int i = 0; i < NUMBER_OF_BUTTONS; i++){

        //Draws the button
        simonDisplay_drawButton(i, DRAW);
    }
}

// Draws a bigger square that completely fills the region.
// If the erase argument is true, it draws the square as black background to "erase" it.
void simonDisplay_drawSquare(uint8_t regionNo, bool erase){

    int32_t color;
    //If erase set color to black
        if(erase){

            //Sets the color to black
            color = DISPLAY_BLACK;

        } else {

            //Gets the color
            color = simonDisplay_getButtonColor(regionNo);

        }

        //Checks to see what region number
        if(regionNo == SIMON_DISPLAY_REGION_0){

            //Draws rect
            display_fillRect(SIMON_DISPLAY_SQUARE_0_X, SIMON_DISPLAY_SQUARE_0_Y, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);

            //Button 1
        } else if(regionNo == SIMON_DISPLAY_REGION_1){

            //Draws rect
            display_fillRect(SIMON_DISPLAY_SQUARE_1_X, SIMON_DISPLAY_SQUARE_1_Y, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);

            //Button 2
        } else if(regionNo == SIMON_DISPLAY_REGION_2){

            //Draws rect
            display_fillRect(SIMON_DISPLAY_SQUARE_2_X, SIMON_DISPLAY_SQUARE_2_Y, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);

            //Button 3
        } else {
            //Draws rect
            display_fillRect(SIMON_DISPLAY_SQUARE_3_X, SIMON_DISPLAY_SQUARE_3_Y, SIMON_DISPLAY_SQUARE_WIDTH, SIMON_DISPLAY_SQUARE_HEIGHT, color);

        }

}





#define TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS 60 // in ms
#define MAX_STR 255
#define TEXT_SIZE 2
#define TEXT_VERTICAL_POSITION 0
#define TEXT_HORIZONTAL_POSITION (DISPLAY_HEIGHT/2)
#define INSTRUCTION_LINE_1 "Touch and release to start the Simon demo."
#define INSTRUCTION_LINE_2 "Demo will terminate after %d touches."
#define DEMO_OVER_MESSAGE_LINE_1 "Simon demo terminated"
#define DEMO_OVER_MESSAGE_LINE_2 "after %d touches."
#define TEXT_VERTICAL_POSITION 0 // Start at the far left.
#define ERASE_THE_SQUARE true  // drawSquare() erases if this is passed in.
#define DRAW_THE_SQUARE false  // drawSquare() draws the square if this is passed in.
#define ERASE_THE_BUTTON true  // drawSquare() erases if this is passed in.
#define DRAW_THE_BUTTON false  // drawSquare() draws the square if this is passed in.


// Runs a brief demonstration of how buttons can be pressed and squares lit up to implement the user
// interface of the Simon game. The routine will continue to run until the touchCount has been reached, e.g.,
// the user has touched the pad touchCount times.

// I used a busy-wait delay (utils_msDelay) that uses a for-loop and just blocks until the time has passed.
// When you implement the game, you CANNOT use this function as we discussed in class. Implement the delay
// using the non-blocking state-machine approach discussed in class.
void simonDisplay_runTest(uint16_t touchCount) {
  display_init();         // Always initialize the display.
  char str[MAX_STR];      // Enough for some simple printing.
  uint8_t regionNumber = 0;   // Convenience variable.
  uint16_t touches = 0;   // Terminate when you receive so many touches.
  // Write an informational message and wait for the user to touch the LCD.
  display_fillScreen(DISPLAY_BLACK);              // clear the screen.
  display_setCursor(TEXT_VERTICAL_POSITION, TEXT_HORIZONTAL_POSITION); // move to the middle of the screen.
  display_setTextSize(TEXT_SIZE);                 // Set the text size for the instructions.
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);   // Reasonable text color.
  sprintf(str, INSTRUCTION_LINE_1);                   // Copy the line to a buffer.
  display_println(str);                               // Print to the LCD.
  display_println();                                  // new-line.
  sprintf(str, INSTRUCTION_LINE_2, touchCount);       // Copy the line to a buffer.
  display_println(str);                               // Print to the LCD.
  while (!display_isTouched());       // Wait here until the screen is touched.
  while (display_isTouched());        // Now wait until the touch is released.
  display_fillScreen(DISPLAY_BLACK);  // Clear the screen.
  simonDisplay_drawAllButtons();      // Draw all of the buttons.
  bool touched = false;         // Keep track of when the pad is touched.
  int16_t x, y;                     // Use these to keep track of coordinates.
  uint8_t z;                        // This is the relative touch pressure.
  while (touches < touchCount) {  // Run the loop according to the number of touches passed in.
    if (!display_isTouched() && touched) {          // user has stopped touching the pad.
      simonDisplay_drawSquare(regionNumber, ERASE_THE_SQUARE);  // Erase the square.
      simonDisplay_drawButton(regionNumber, DRAW_THE_BUTTON);  //  Draw the button.
      touched = false;                  // Released the touch, set touched to false.
    } else if (display_isTouched() && !touched) {   // User started touching the pad.
      touched = true;                             // Just touched the pad, set touched = true.
      touches++;                                  // Keep track of the number of touches.
      display_clearOldTouchData();                // Get rid of data from previous touches.
      // Must wait this many milliseconds for the chip to do analog processing.
      utils_msDelay(TOUCH_PANEL_ANALOG_PROCESSING_DELAY_IN_MS);
      display_getTouchedPoint(&x, &y, &z);        // After the wait, get the touched point.
      regionNumber = simonDisplay_computeRegionNumber(x, y);// Compute the region number, see above.
      simonDisplay_drawSquare(regionNumber, DRAW_THE_SQUARE);  // Draw the square (erase = false).
    }
  }
  // Done with the demo, write an informational message to the user.
  display_fillScreen(DISPLAY_BLACK);        // clear the screen.
  // Place the cursor in the middle of the screen.
  display_setCursor(TEXT_VERTICAL_POSITION, TEXT_HORIZONTAL_POSITION);
  display_setTextSize(TEXT_SIZE); // Make it readable.
  display_setTextColor(DISPLAY_RED, DISPLAY_BLACK);  // red is foreground color, black is background color.
  sprintf(str, DEMO_OVER_MESSAGE_LINE_1);    // Format a string using sprintf.
  display_println(str);                     // Print it to the LCD.
  sprintf(str, DEMO_OVER_MESSAGE_LINE_2, touchCount);  // Format the rest of the string.
  display_println(str);  // Print it to the LCD.
}

