#include "ticTacToeDisplay.h"
#include "supportFiles/display.h"
#include "../lab2/buttons.h"
#include "../lab2/switches.h"
#include "supportFiles/utils.h"

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#define DEFAULT_COLOR DISPLAY_BLUE //The default color for the game

//Board lines
#define LEFT_VERT_LINE_TOP_X SCREEN_WIDTH / 3
#define LEFT_VERT_LINE_TOP_Y 0
#define LEFT_VERT_LINE_BOTTOM_X SCREEN_WIDTH /3
#define LEFT_VERT_LINE_BOTTOM_Y SCREEN_HEIGHT
#define RIGHT_VERT_LINE_TOP_X 2 * (SCREEN_WIDTH / 3)
#define RIGHT_VERT_LINE_TOP_Y 0
#define RIGHT_VERT_LINE_BOTTOM_X 2 * (SCREEN_WIDTH / 3)
#define RIGHT_VERT_LINE_BOTTOM_Y SCREEN_HEIGHT
#define TOP_HORIZ_LINE_LEFT_X 0
#define TOP_HORIZ_LINE_LEFT_Y SCREEN_HEIGHT / 3
#define TOP_HORIZ_LINE_RIGHT_X SCREEN_WIDTH
#define TOP_HORIZ_LINE_RIGHT_Y SCREEN_HEIGHT / 3
#define BOTTOM_HORIZ_LINE_LEFT_X 0
#define BOTTOM_HORIZ_LINE_LEFT_Y 2 * (SCREEN_HEIGHT / 3)
#define BOTTOM_HORIZ_LINE_RIGHT_X SCREEN_WIDTH
#define BOTTOM_HORIZ_LINE_RIGHT_Y 2 * (SCREEN_HEIGHT / 3)

//Center coordinates
#define R0_CENTER SCREEN_HEIGHT / 6 //Row 0 center
#define R1_CENTER 3 * (SCREEN_HEIGHT / 6) //Row 1 center
#define R2_CENTER 5 * (SCREEN_HEIGHT / 6) //Row 2 center
#define C0_CENTER SCREEN_WIDTH / 6 //Column 0 center
#define C1_CENTER 3 * (SCREEN_WIDTH / 6) //Column 1 center
#define C2_CENTER 5 * (SCREEN_WIDTH / 6) //Column 2 center

#define R0 0
#define R1 1
#define R2 2
#define C0 0
#define C1 1
#define C2 2

#define BUFFER_DIVISOR 18

#define RADIUS 2 * (SCREEN_HEIGHT / BUFFER_DIVISOR)

#define TOUCHDELAY 50

//Gets the center of the row
uint16_t ticTacToeDisplay_getRowCenter(uint8_t row){

    //Switches on the row
    switch(row){

    //Is it Row 0
    case R0:

        //Returns R0 Center
        return R0_CENTER;

    break;

    //Row 1?
    case R1:

        //Returns R1 Center
        return R1_CENTER;

    break;

    //Row 2?
    case R2:

        //Returns R2 Center
        return R2_CENTER;

    break;

    //It shouldn't make it to here
    default:

        //Defaults to R0
        return R0_CENTER;
    }
}

//Gets the center of the column
uint16_t ticTacToeDisplay_getColCenter(uint8_t column){

    //Switches on the column
    switch(column){

    //Is it column 0
    case C0:

        //Returns R0 Center
        return C0_CENTER;

    break;

    //column 1?
    case C1:

        //Returns R1 Center
        return C1_CENTER;

    break;

    //column 2?
    case C2:

        //Returns C2 Center
        return C2_CENTER;

    break;

    //It shouldn't make it to here
    default:

        //Defaults to C0
        return C0_CENTER;
    }
}

//Gets the row of the touch
uint16_t ticTacToeDisplay_getTouchRow(uint16_t y){

    //Checks to see if in row 0
    if(y < TOP_HORIZ_LINE_LEFT_Y){

        //Returns row 0
        return R0;

    } else if(y > BOTTOM_HORIZ_LINE_LEFT_Y){

        //Returns row 2
        return R2;
    } else {

        //Returns row 1
        return R1;
    }
}

//Gets the column of the touch
uint16_t ticTacToeDisplay_getTouchCol(uint16_t x){

    //Checks to see if in col 0
    if(x < LEFT_VERT_LINE_TOP_X){

        //Returns col 0
        return C0;

    } else if(x > RIGHT_VERT_LINE_TOP_X){

        //Returns col 2
        return C2;

    } else {

        //Returns col 1
        return C1;

    }
}


// This will draw the four board lines.
void ticTacToeDisplay_drawBoardLines(){

    //Draws left vertical line
    display_drawLine(LEFT_VERT_LINE_TOP_X, LEFT_VERT_LINE_TOP_Y, LEFT_VERT_LINE_BOTTOM_X, LEFT_VERT_LINE_BOTTOM_Y, DEFAULT_COLOR);

    //Draws right vertical line
    display_drawLine(RIGHT_VERT_LINE_TOP_X, RIGHT_VERT_LINE_TOP_Y, RIGHT_VERT_LINE_BOTTOM_X, RIGHT_VERT_LINE_BOTTOM_Y, DEFAULT_COLOR);

    //Draws top horizontal line
    display_drawLine(TOP_HORIZ_LINE_LEFT_X, TOP_HORIZ_LINE_LEFT_Y, TOP_HORIZ_LINE_RIGHT_X, TOP_HORIZ_LINE_RIGHT_Y, DEFAULT_COLOR);

    //Draws bottom horizontal line
    display_drawLine(BOTTOM_HORIZ_LINE_LEFT_X, BOTTOM_HORIZ_LINE_LEFT_Y, BOTTOM_HORIZ_LINE_RIGHT_X, BOTTOM_HORIZ_LINE_RIGHT_Y, DEFAULT_COLOR);

}

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init(){

    // Must init all of the software and underlying hardware for LCD.
    display_init();

    //Black out the screen
    display_fillScreen(DISPLAY_BLACK);

    //Draws the lines
    //ticTacToeDisplay_drawBoardLines();
}

// Draws an X at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase == false, draw the X as foreground.
void ticTacToeDisplay_drawX(uint8_t row, uint8_t column, bool erase){

    //Calculates the x offset
    uint16_t xOffset = SCREEN_WIDTH / BUFFER_DIVISOR;

    //Calculates the y offset
    uint16_t yOffset = SCREEN_HEIGHT / BUFFER_DIVISOR;

    //Gets the row y coordinate
    uint16_t rowY = ticTacToeDisplay_getRowCenter(row);

    //Gets the column X
    uint16_t colX = ticTacToeDisplay_getColCenter(column);

    //Computes the left x coordinate
    uint16_t leftX = colX - xOffset;

    //Computes the right x coordinate
    uint16_t rightX = colX + yOffset;

    //Computes the top y coordinate
    uint16_t topY = rowY - yOffset;

    //Computes the bottom y coordinate
    uint16_t bottomY = rowY + yOffset;

    //The color it's going to draw
    uint16_t color;

    //Checks to see if it's supposed to erase
    if(erase){

        //Sets the color to black
        color = DISPLAY_BLACK;

    } else {

        //Sets the color to default color
        color = DEFAULT_COLOR;

    }

    //Draws the first line
    display_drawLine(leftX, topY, rightX, bottomY, color);

    //Draws the second line
    display_drawLine(leftX, bottomY, rightX, topY, color);

}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background. erase == false, draw the X as foreground.
void ticTacToeDisplay_drawO(uint8_t row, uint8_t column, bool erase){

    //Gets the row y coordinate
    uint16_t rowY = ticTacToeDisplay_getRowCenter(row);

    //Gets the column X
    uint16_t colX = ticTacToeDisplay_getColCenter(column);

    //The color for the circle
    uint16_t color;

    //Is it supposed to erase?
    if(erase){

        //Sets the color to black
        color = DISPLAY_BLACK;
    } else {

        //Sets the color to the default
        color = DEFAULT_COLOR;
    }

    //Draws the circle
    display_drawCircle(colX, rowY, RADIUS, color);

}

// After a touch has been detected and after the proper delay, this sets the row and column arguments
// according to where the user touched the board.
void ticTacToeDisplay_touchScreenComputeBoardRowColumn(uint8_t* row, uint8_t* column){

    //X coordinate of touch
    int16_t x;

    //Y coordinate of touch
    int16_t y;

    //Z coordinate of touch
    uint8_t z;

    //Gets the touchedPoint and puts them into x, y, and z
    display_getTouchedPoint(&x, &y, &z);

    //Sets the row to the touched row
    *row = ticTacToeDisplay_getTouchRow(y);

    //Sets the column to the touched column
    *column = ticTacToeDisplay_getTouchCol(x);

}

//Clears the board
void ticTacToeDisplay_clearBoard(){

   //Black out the screen
   display_fillScreen(DISPLAY_BLACK);

   //Draws the lines
   ticTacToeDisplay_drawBoardLines();

}

#define BUTTONS_BTN0_MASK 0x1
#define BUTTONS_BTN1_MASK 0x2
#define SWITCHES_SW0_MASK 0x1

// Runs a test of the display. Does the following.
// Draws the board. Each time you touch one of the screen areas, the screen will paint
// an X or an O, depending on whether switch 0 (SW0) is slid up (O) or down (X).
// When BTN0 is pushed, the screen is cleared. The test terminates when BTN1 is pushed.
#define TRUE 1
#define DELAY 50
void ticTacToeDisplay_runTest(){

    //Inits the board
    ticTacToeDisplay_init();

    //Runs the test
    while(TRUE){

        //Reads the buttons
        int32_t buttons = buttons_read();

        //Reads the switches
        int32_t switches = switches_read();

        //Checks to see if the button 1 is pressed
        if(buttons & BUTTONS_BTN1_MASK){

            //Clear the screen
            display_fillScreen(DISPLAY_BLACK);

            //Test is over
            display_println("Test is over");

            //terminate
            break;
        }

        //If button one is pressed
        if(buttons & BUTTONS_BTN0_MASK){

            //Clear board
            ticTacToeDisplay_clearBoard();
        }
        //Clears that ##&^#$%@ touch data
        display_clearOldTouchData();

        //Checks to see if the display was touched
        if(display_isTouched()){

            //Waits
            utils_msDelay(DELAY);

            //Row
            uint8_t row;

            //Column
            uint8_t column;

            //Get the row and column
            ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);

            //Checks to see if switch is up is pressed
            if(switches & SWITCHES_SW0_MASK){

               //Draws an 0
               ticTacToeDisplay_drawO(row, column, false);

            } else {

                //Draws an x
                ticTacToeDisplay_drawX(row, column, false);

            }

            display_clearOldTouchData();

        }
    }
}
