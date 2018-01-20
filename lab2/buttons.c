#include "buttons.h"
#include "xil_io.h"
#include "supportFiles/display.h"
#include "stdio.h"
#include "supportFiles/leds.h"

#define BUTTONS_GPIO_DEVICE_BASEADDR XPAR_PUSH_BUTTONS_BASEADDR
#define GPIO_DATA_OFFSET 0
#define GPIO_TRI_OFFSET 4
#define TRI_INIT_SET_INPUT 0xF
#define BUTTON_MASK 0x000F
#define BUTTON_END_TEST 0x000F
#define BUTTON_3_MASK 0x0008
#define BUTTON_2_MASK 0x0004
#define BUTTON_1_MASK 0x0002
#define BUTTON_0_MASK 0x0001
#define TOP_OF_SCREEN_HORIZONTAL 0
#define TOP_OF_SCREEN_VERTICAL 0
#define BUTTON_WIDTH  80
#define BUTTON_HEIGHT 120
#define BUTTON_3_X TOP_OF_SCREEN_HORIZONTAL
#define BUTTON_3_Y TOP_OF_SCREEN_VERTICAL
#define BUTTON_2_X 80
#define BUTTON_2_Y TOP_OF_SCREEN_VERTICAL
#define BUTTON_1_X 160
#define BUTTON_1_Y TOP_OF_SCREEN_VERTICAL
#define BUTTON_0_X 240
#define BUTTON_0_Y TOP_OF_SCREEN_VERTICAL
#define TEXT_SIZE 2
#define BUTTON_3_TEXT_CURSOR_X 17
#define BUTTON_3_TEXT_CURSOR_Y 60
#define BUTTON_2_TEXT_CURSOR_X 97
#define BUTTON_2_TEXT_CURSOR_Y 60
#define BUTTON_1_TEXT_CURSOR_X 177
#define BUTTON_1_TEXT_CURSOR_Y 60
#define BUTTON_0_TEXT_CURSOR_X 257
#define BUTTON_0_TEXT_CURSOR_Y 60


int32_t previous_screen_state = 0x0000;

//Helper function to read GPIO registers
int32_t buttons_readGpioRegister(int32_t offset) {

    //Uses the low-level Xilinx call
    return Xil_In32(BUTTONS_GPIO_DEVICE_BASEADDR + offset);
}

//Helper function to write to GPIO registers
void buttons_writeGpioRegister(int32_t offset, int32_t value){

    //Uses a xilinx call
    Xil_Out32(BUTTONS_GPIO_DEVICE_BASEADDR + offset, value);

}

//Initializes the buttons
int32_t buttons_init(){

       //Tells the Tri state driver to accept input
       buttons_writeGpioRegister(GPIO_TRI_OFFSET, TRI_INIT_SET_INPUT);

       //Checks to make sure that the Tri is set for input
       if(buttons_readGpioRegister(GPIO_TRI_OFFSET) == TRI_INIT_SET_INPUT){

           return 1; //Returns 1 if they are the same

       } else {

           return 0; //Returns 0 if they aren't the same
       }

}

//Initializes the screen
void screen_init(){

    display_init();  // Must init all of the software and underlying hardware for LCD.

    display_fillScreen(DISPLAY_BLACK);  // Blank the screen.

}

//Draws the squares for each button pressed.
void draw_buttons(int32_t button_value){

    //Sets textColor as white.
    display_setTextColor(DISPLAY_WHITE);

    //Sets the text size
    display_setTextSize(TEXT_SIZE);

    //Checks to see if the buttons pressed are the same as just were pressed so it doesn't have to redraw.
    if(button_value != previous_screen_state) {

        //Checks to see if button 3 is pressed
        if((button_value & BUTTON_3_MASK) == BUTTON_3_MASK){

            //Draw a green rectangle signifying button 3
            display_fillRect(BUTTON_3_X, BUTTON_3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_GREEN);

            //Sets the cursor for the text
            display_setCursor(BUTTON_3_TEXT_CURSOR_X, BUTTON_3_TEXT_CURSOR_Y);

            //Prints the text
            display_println("BTN3");

        } else {

            //Fills in the square with black if the button is not pressed anymore.
            display_fillRect(BUTTON_3_X, BUTTON_3_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);

        }

        //Checks to see if button 2 is pressed
        if((button_value & BUTTON_2_MASK) == BUTTON_2_MASK){

            //Draw a blue rectangle signifying button 2
            display_fillRect(BUTTON_2_X, BUTTON_2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLUE);

            //Sets the cursor for the text
            display_setCursor(BUTTON_2_TEXT_CURSOR_X, BUTTON_2_TEXT_CURSOR_Y);

            //Prints the text
            display_println("BTN2");

        } else {

            //Fills in the square with black if the button is not pressed anymore.
            display_fillRect(BUTTON_2_X, BUTTON_2_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);

        }

        //Checks to see if button 1 is pressed
        if((button_value & BUTTON_1_MASK) == BUTTON_1_MASK){

            //Draw a blue rectangle signifying button 2
            display_fillRect(BUTTON_1_X, BUTTON_1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_YELLOW);

            //Sets the cursor for the text
            display_setCursor(BUTTON_1_TEXT_CURSOR_X, BUTTON_1_TEXT_CURSOR_Y);

            //Prints the text
            display_println("BTN1");

        } else {

            //Fills in the square with black if the button is not pressed anymore.
            display_fillRect(BUTTON_1_X, BUTTON_1_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);

        }

        //Checks to see if button 0 is pressed
        if((button_value & BUTTON_0_MASK) == BUTTON_0_MASK){

            display_fillRect(BUTTON_0_X, BUTTON_0_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_RED);

            //Sets the cursor for the text
            display_setCursor(BUTTON_0_TEXT_CURSOR_X, BUTTON_0_TEXT_CURSOR_Y);

            //Prints the text
            display_println("BTN0");

        } else {

            //Fills in the square with black if the button is not pressed anymore.
            display_fillRect(BUTTON_0_X, BUTTON_0_Y, BUTTON_WIDTH, BUTTON_HEIGHT, DISPLAY_BLACK);

        }

        //Sets the previous screen state to the current button value.
        previous_screen_state = button_value;

    }

}

//Returns the current value of all 4 buttons.
int32_t buttons_read(){

    //Returns the value of the buttons
    return buttons_readGpioRegister(GPIO_DATA_OFFSET);

}

void buttons_runTest(){

    //Initializes the buttons
        if(!buttons_init()){

            //Prints out a failed message
            printf("Button initialization failed\n\r");

            return;
        }

        //Initializes the screen
        screen_init();

        // Flag for if the while loop should continue
        char cont = 1;

        //Runs until all 4 switches have been switched.
        while(cont){

            //Calls the draw button function
            draw_buttons(buttons_read());

            //Checks to see if all the switches are on.
           if((buttons_read() & BUTTON_MASK) == BUTTON_END_TEST){

               //Draws all the buttons
               draw_buttons(BUTTON_END_TEST);

               //Sets the cont flag to 0 so the while loop exits
               cont = 0;
           }
        }

}
