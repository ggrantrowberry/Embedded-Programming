#include "switches.h"
#include "supportFiles/leds.h"
#include "xparameters.h"
#include "xil_io.h"
#include "stdio.h"

#define SWITCHES_GPIO_DEVICE_BASEADDR XPAR_SLIDE_SWITCHES_BASEADDR
#define PRINT_FAILED_STATUS_FLAG 1
#define GPIO_DATA_OFFSET 0
#define GPIO_TRI_OFFSET 4
#define TRI_INIT_SET_INPUT 0xF
#define SWITCH_MASK 0x000F
#define SWITCH_END_TEST 0x000F
#define LEDS_OFF 0x0000

//Helper function for reading from GPIO register
int32_t switches_readGpioRegister(int32_t offset){

    //Uses the xilinx call
    return Xil_In32(SWITCHES_GPIO_DEVICE_BASEADDR + offset);

}

//Helper function for writing to GPIO register
void switches_writeGpioRegister(int32_t offset, int32_t value){

    //Uses a xilinx call
    Xil_Out32(SWITCHES_GPIO_DEVICE_BASEADDR + offset, value);

}

//Initializes the switches.
int32_t switches_init(){

    //Initializes the LEDS
    leds_init(PRINT_FAILED_STATUS_FLAG);

    //Tells the Tri state driver to accept input
    switches_writeGpioRegister(GPIO_TRI_OFFSET, TRI_INIT_SET_INPUT);

    //Checks to make sure that the Tri is set for input
    if(switches_readGpioRegister(GPIO_TRI_OFFSET) == TRI_INIT_SET_INPUT){

        return 1; //Returns 1 if they are the same

    } else {

        return 0; //Returns 0 if they aren't the same

    }
}

//Returns the current value of all 4 switches.
int32_t switches_read(){

    //Returns the value of the switches
    return switches_readGpioRegister(GPIO_DATA_OFFSET);

}

void switches_runTest(){

    //Initializes the switches
    if(!switches_init()){

        //Prints out a failed message
        printf("Switch initialization failed\n\r");

        return;
    }

    // Flag for if the while loop should continue
    char cont = 1;

    //Runs until all 4 switches have been switched.
    while(cont){

        //Writes the switch value to the leds.
        leds_write(switches_read());

        //Checks to see if all the switches are on.
       if((switches_read() & SWITCH_MASK) == SWITCH_END_TEST){

           //Turns all the leds on
           leds_write(SWITCH_END_TEST);

           //Sets the cont flag to 0 so the while loop exits
           cont = 0;

           //Turns off all the leds.
           leds_write(LEDS_OFF);

       }

    }
}
