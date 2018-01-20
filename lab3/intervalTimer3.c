#include "intervalTimer3.h"
#include "xparameters.h"
#include "xil_io.h"
#include <stdio.h>

#define INTERVAL_TIMER_0_BASEADDR XPAR_AXI_TIMER_0_BASEADDR
#define INTERVAL_TIMER_1_BASEADDR XPAR_AXI_TIMER_1_BASEADDR
#define INTERVAL_TIMER_2_BASEADDR XPAR_AXI_TIMER_2_BASEADDR
#define TCSR0_ADDR_OFFSET 0x00
#define TLR0_ADDR_OFFSET 0x04
#define TCR0_ADDR_OFFSET 0x08
#define TCSR1_ADDR_OFFSET 0x10
#define TLR1_ADDR_OFFSET 0x14
#define TCR1_ADDR_OFFSET 0x18
#define CASC_BIT_SET 0x0800
#define ENT0_BIT_SET 0x0080
#define LOAD_BIT_SET 0x0020
#define TIMER_CLOCK_FREQ_HZ XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ
#define LEFT_SHIFT 32
#define RESET_VALUE 0




//Helper function to read GPIO registers
int32_t intervalTimer_readRegister(int32_t baseAddr, int32_t offset) {

    //Uses the low-level Xilinx call
    return Xil_In32(baseAddr + offset);
}

//Helper function to write to GPIO registers
void intervalTimer_writeRegister(int32_t baseAddr, int32_t offset, int32_t value){

    //Uses a xilinx call
    Xil_Out32(baseAddr + offset, value);

}

intervalTimer_status_t initCheckStatus(uint32_t timerBaseAddr){
    //Checks to see if the TCSR0 Register has the CASC bit set to 1 and everything else 0
            if(intervalTimer_readRegister(timerBaseAddr, TCSR0_ADDR_OFFSET) == CASC_BIT_SET){

                //Checks to see if the TCSR1 Register is 0
                if(intervalTimer_readRegister(timerBaseAddr, TCSR1_ADDR_OFFSET) == 0){

                    //If both are good returns ok
                    return INTERVAL_TIMER_STATUS_OK;

                } else {

                    //If the second is bad returns fail
                    return INTERVAL_TIMER_STATUS_FAIL;
                }

            }

            //Returns fail if it makes it this far
            return INTERVAL_TIMER_STATUS_FAIL;

}

//Returns the base address of a timer
uint32_t intervalTimer_getTimerBaseAddr(uint32_t timerNumber){

    //Checks to see which timer to get the base address
    switch(timerNumber){

        //Checks timer 0
        case INTERVAL_TIMER_TIMER_0:

            //Returns timer 0 base addr
            return INTERVAL_TIMER_0_BASEADDR;

            break;

        //Checks timer 1
        case INTERVAL_TIMER_TIMER_1:

            //Returns timer 1 base addr
            return INTERVAL_TIMER_1_BASEADDR;

            break;

        //Checks timer 2
        case INTERVAL_TIMER_TIMER_2:

            //Returns timer 2 base addr
            return INTERVAL_TIMER_2_BASEADDR;

            break;

    }

    //If nothing then return 0
    return 0x0000;
}

//Initializes a given timer.
intervalTimer_status_t intervalTimer_init(uint32_t timerNumber){

    //Gets the baseAddr of timerNumber
    uint32_t timerNumberBaseAddr = intervalTimer_getTimerBaseAddr(timerNumber);

    //Writes zero to the TCSR0 register
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET, RESET_VALUE);

    //Writes zero to the TCSR1 register
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR1_ADDR_OFFSET, RESET_VALUE);

    //Sets the cascading bit to 1
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET, CASC_BIT_SET);

    //Checks to see if init went well
    return initCheckStatus(timerNumberBaseAddr);

    //If it makes it this far then the the init has failed.
    return INTERVAL_TIMER_STATUS_FAIL;
}

//Initializes all the timers
intervalTimer_status_t intervalTimer_initAll(){

    //Returns the status of all the init functions anded together
    return intervalTimer_init(INTERVAL_TIMER_TIMER_0) &&
            intervalTimer_init(INTERVAL_TIMER_TIMER_1) &&
            intervalTimer_init(INTERVAL_TIMER_TIMER_2);

}

//Starts the timer
void intervalTimer_start(uint32_t timerNumber){

    //Gets the baseAddr of timerNumber
    uint32_t timerNumberBaseAddr = intervalTimer_getTimerBaseAddr(timerNumber);

    //Checks to see if the time is already going if it is then it returns
    if(intervalTimer_readRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET) & ENT0_BIT_SET){
        return;
    }

    //Gets the current value of TSCR0
    uint32_t TCSR0_value = intervalTimer_readRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET);

    //Changes the ENT0 bit in the TCSR0 register which starts the timer
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET, ENT0_BIT_SET | TCSR0_value);

}

//Stops the timer
void intervalTimer_stop(uint32_t timerNumber){

    //Gets the baseAddr of timerNumber
    uint32_t timerNumberBaseAddr = intervalTimer_getTimerBaseAddr(timerNumber);

    //Checks to see if the timer is already running. If it is nothing happens
    if(!(intervalTimer_readRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET) & ENT0_BIT_SET)){
            return;
        }

    //Gets the current value of TSCR0
    uint32_t TCSR0_value = intervalTimer_readRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET);

    //Changes the ENT0 bit in the TCSR0 register which starts the timer. Preserves the CASC bit
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET, (~ENT0_BIT_SET) & TCSR0_value);

}

//Resets one timer
void intervalTimer_reset(uint32_t timerNumber){

    //Gets the baseAddr of timerNumber
    uint32_t timerNumberBaseAddr = intervalTimer_getTimerBaseAddr(timerNumber);

    //Writes a 0 to the TLR0 register
    intervalTimer_writeRegister(timerNumberBaseAddr, TLR0_ADDR_OFFSET, RESET_VALUE);

    //Gets the current value of TCSR0
    uint32_t TCSR0_value = intervalTimer_readRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET);

    //Writes a 1 into the LOAD0 bit in the TCSR0
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR0_ADDR_OFFSET, LOAD_BIT_SET | TCSR0_value);

    //Writes a 0 to the TLR1 register
    intervalTimer_writeRegister(timerNumberBaseAddr, TLR1_ADDR_OFFSET, RESET_VALUE);

    //Gets the current value of TCSR1
    uint32_t TCSR1_value = intervalTimer_readRegister(timerNumberBaseAddr, TCSR1_ADDR_OFFSET);

    //Writes a 1 into the LOAD1 bit in the TCSR1
    intervalTimer_writeRegister(timerNumberBaseAddr, TCSR1_ADDR_OFFSET, LOAD_BIT_SET | TCSR1_value);

    //Re-initializes the register
    intervalTimer_init(timerNumber);

}

//Resets all the timers
void intervalTimer_resetAll(){

    //Resets timer 0
    intervalTimer_reset(INTERVAL_TIMER_TIMER_0);

    //Resets timer 1
    intervalTimer_reset(INTERVAL_TIMER_TIMER_1);

    //Resets timer 2
    intervalTimer_reset(INTERVAL_TIMER_TIMER_2);

}

//Gets Duration in Seconds
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber){

    //Gets the baseAddr of timerNumber
    uint32_t timerNumberBaseAddr = intervalTimer_getTimerBaseAddr(timerNumber);

    uint64_t tcr1Value = intervalTimer_readRegister(timerNumberBaseAddr, TCR1_ADDR_OFFSET);
    uint32_t tcr0Value = intervalTimer_readRegister(timerNumberBaseAddr, TCR0_ADDR_OFFSET);

    //Gets the value from TCR1 and shifts it 32 bits to the left the ors the TCR0
    uint64_t currentTime = ( tcr1Value << LEFT_SHIFT) | tcr0Value;

    double doubleTime = currentTime;

    //Converts the time to seconds
    double seconds = doubleTime / TIMER_CLOCK_FREQ_HZ;

    //returns seconds.
    return seconds;
}

//Tests one intervalTimer
intervalTimer_status_t intervalTimer_test(uint32_t timerNumber){

  //Initializes Timer
  intervalTimer_init(timerNumber);

  //Starts the Timer
  intervalTimer_start(timerNumber);

  //Gets the duration in seconds
  printf("%f\n\r",intervalTimer_getTotalDurationInSeconds(timerNumber));

  //Gets the duration in seconds. Should be slightly different then the one before it
  printf("%f\n\r",intervalTimer_getTotalDurationInSeconds(timerNumber));

  //Stops the timer
  intervalTimer_stop(0);

  //Gets duration
  printf("%f\n\r",intervalTimer_getTotalDurationInSeconds(timerNumber));

  //Gets duration
  printf("%f\n\r",intervalTimer_getTotalDurationInSeconds(timerNumber));

  //Resets timer
  intervalTimer_reset(timerNumber);

  //Looks at duration. Should be zero
  printf("%f\n\r",intervalTimer_getTotalDurationInSeconds(timerNumber));

  //Checks to see if the timer was reset
  if(intervalTimer_getTotalDurationInSeconds(timerNumber) == 0){

      //returns good
      return INTERVAL_TIMER_STATUS_OK;
  }

  //returns bad
  return INTERVAL_TIMER_STATUS_FAIL;
}

intervalTimer_status_t intervalTimer_testAll(){

    //Initializes timer0
    intervalTimer_init(INTERVAL_TIMER_TIMER_0);

    //Starts timer0
    intervalTimer_start(INTERVAL_TIMER_TIMER_0);

    //Stops timer 0
        intervalTimer_stop(INTERVAL_TIMER_TIMER_0);

    //Gets duration of timer0
    printf("Interval Timer0:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));

    //Initializes timer1
    intervalTimer_init(INTERVAL_TIMER_TIMER_1);

    //Starts timer1
    intervalTimer_start(INTERVAL_TIMER_TIMER_1);

    //Gets duration of timer1
    printf("Interval Timer1:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));

    //Initializes timer2
    intervalTimer_init(INTERVAL_TIMER_TIMER_2);

    //Starts timer2
    intervalTimer_start(INTERVAL_TIMER_TIMER_2);

    //Gets duration of timer2
    printf("Interval Timer02:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));

    //Gets duration of timer0
    printf("Interval Timer0:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));

    //Gets duration of timer1
    printf("Interval Timer1:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));

    //Gets duration of timer2
    printf("Interval Timer02:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));

    //Stops timer 0
    intervalTimer_stop(INTERVAL_TIMER_TIMER_0);

    //Stops timer 1
    intervalTimer_stop(INTERVAL_TIMER_TIMER_1);

    //Stops timer 2
    intervalTimer_stop(INTERVAL_TIMER_TIMER_2);

    //Gets duration of timer2
    printf("Interval Timer02:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));

    //Gets duration of timer0
    printf("Interval Timer0:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));

    //Gets duration of timer1
    printf("Interval Timer1:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));

    //Gets duration of timer2
    printf("Interval Timer02:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));

    //Resets timer 0
    intervalTimer_reset(INTERVAL_TIMER_TIMER_0);

    //Resets timer 1
    intervalTimer_reset(INTERVAL_TIMER_TIMER_1);

    //Resets timer 2
    intervalTimer_reset(INTERVAL_TIMER_TIMER_2);

    //Gets duration of timer0
    printf("Interval Timer0:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0));

    //Gets duration of timer1
    printf("Interval Timer1:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1));

    //Gets duration of timer2
    printf("Interval Timer02:%f\n\r",intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2));

    if(intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_0) && intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_1)
        && intervalTimer_getTotalDurationInSeconds(INTERVAL_TIMER_TIMER_2)){

        //returns good
          return INTERVAL_TIMER_STATUS_OK;

    }

    //returns bad
    return INTERVAL_TIMER_STATUS_FAIL;

}


