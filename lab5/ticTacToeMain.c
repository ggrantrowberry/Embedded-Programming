/***********************************
************ Flag Method ***********
************************************/
#include <stdio.h>
#include "supportFiles/leds.h"
#include "supportFiles/globalTimer.h"
#include "supportFiles/interrupts.h"
#include <stdbool.h>
#include <stdint.h>
#include "supportFiles/display.h"
#include "ticTacToeDisplay.h"
#include "ticTacToeControl.h"
#include "xparameters.h"

#define TOTAL_SECONDS 120
// The formula for computing the load value is based upon the formula from 4.1.1 (calculating timer intervals)
// in the Cortex-A9 MPCore Technical Reference Manual 4-2.
// Assuming that the prescaler = 0, the formula for computing the load value based upon the desired period is:
// load-value = (period * timer-clock) - 1
#define TIMER_PERIOD 50.0E-3 // You can change this value to a value that you select.
#define TIMER_CLOCK_FREQUENCY (XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ / 2)
#define TIMER_LOAD_VALUE ((TIMER_PERIOD * TIMER_CLOCK_FREQUENCY) - 1.0)
#define INIT_VALUE 0
int main()
{
    // Initialize the GPIO LED driver and print out an error message if it fails (argument = true).
	   // You need to init the LEDs so that LD4 can function as a heartbeat.
    leds_init(true);
    // Init all interrupts (but does not enable the interrupts at the devices).
    // Prints an error message if an internal failure occurs because the argument = true.
    interrupts_initAll(true);
    interrupts_setPrivateTimerLoadValue(TIMER_LOAD_VALUE);
    u32 privateTimerTicksPerSecond = interrupts_getPrivateTimerTicksPerSecond();
    printf("private timer ticks per second: %ld\n\r", privateTimerTicksPerSecond);
    // Allow the timer to generate interrupts.
    interrupts_enableTimerGlobalInts();
    // Initialization of the display is not time-dependent, do it outside of the state machine.
    ticTacToeDisplay_init();
    // Keep track of your personal interrupt count. Want to make sure that you don't miss any interrupts.
     int32_t personalInterruptCount = INIT_VALUE;
    // Start the private ARM timer running.
    interrupts_startArmPrivateTimer();
    // Enable interrupts at the ARM.
    interrupts_enableArmInts();
    // interrupts_isrInvocationCount() returns the number of times that the timer ISR was invoked.
    // This value is maintained by the timer ISR. Compare this number with your own local
    // interrupt count to determine if you have missed any interrupts.
     //while (interrupts_isrInvocationCount() < (TOTAL_SECONDS * privateTimerTicksPerSecond)) {
     while (interrupts_isrInvocationCount() < (TOTAL_SECONDS * privateTimerTicksPerSecond)) {
      if (interrupts_isrFlagGlobal) {  // This is a global flag that is set by the timer interrupt handler.
    	  // Count ticks.
      	personalInterruptCount++;
      	ticTacToeControl_tick();
    	  interrupts_isrFlagGlobal = INIT_VALUE;
      }
   }
   interrupts_disableArmInts();
   printf("isr invocation count: %ld\n\r", interrupts_isrInvocationCount());
   printf("internal interrupt count: %ld\n\r", personalInterruptCount);

   return INIT_VALUE;




   /*Test function
    *     minimax_board_t board1;  // Board 1 is the main example in the web-tutorial that I use on the web-site.
    *
    *
    *     //Test Boards
    *
        #define R0 0 //Row 0
        #define R1 1 //Row 1
        #define R2 2 //Row 2
        #define C0 0 //Column 0
        #define C1 1 //Column 1
        #define C2 2 //Column 2
    board1.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot //Puts an O in that spot
    board1.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board1.squares[R0][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board1.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board1.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board1.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board1.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board1.squares[R2][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board1.squares[R2][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot

    minimax_board_t board2;
    board2.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board2.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board2.squares[R0][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board2.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board2.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board2.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board2.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board2.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board2.squares[R2][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot

    minimax_board_t board3;
    board3.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board3.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board3.squares[R0][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board3.squares[R1][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board3.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board3.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board3.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board3.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board3.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    minimax_board_t board4;
    board4.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board4.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board4.squares[R0][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board4.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board4.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board4.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board4.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board4.squares[R2][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board4.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    minimax_board_t board5;
    board5.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board5.squares[R0][C1] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board5.squares[r0][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board5.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board5.squares[R1][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board5.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board5.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board5.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board5.squares[R2][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot

    minimax_board_t board6;
    board6.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board6.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board6.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board6.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board6.squares[R1][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board6.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board6.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board6.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board6.squares[R2][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot

    minimax_board_t board7;
    board7.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board7.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board7.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board7.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board7.squares[R1][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board7.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board7.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board7.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board7.squares[R2][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot

    minimax_board_t board8;
    board8.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board8.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board8.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board8.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board8.squares[R1][C1] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board8.squares[R1][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board8.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board8.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board8.squares[R2][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot

    minimax_board_t board9;
    board9.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board9.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board9.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board9.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board9.squares[R1][C1] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board9.squares[R1][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board9.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board9.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board9.squares[R2][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot

    minimax_board_t board10;
    board10.squares[R0][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board10.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board10.squares[R0][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board10.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board10.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board10.squares[R1][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board10.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board10.squares[R2][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board10.squares[R2][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot

    minimax_board_t board11;
    board11.squares[R0][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board11.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board11.squares[R0][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board11.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board11.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board11.squares[R1][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board11.squares[R2][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board11.squares[R2][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board11.squares[R2][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot

    minimax_board_t board12;
    board12.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board12.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board12.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board12.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board12.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board12.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board12.squares[R2][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board12.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board12.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    minimax_board_t board13;
    board13.squares[R0][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board13.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board13.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board13.squares[R1][C0] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board13.squares[R1][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board13.squares[R1][C2] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board13.squares[R2][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board13.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board13.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    minimax_board_t board14;
    board14.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board14.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board14.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board14.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board14.squares[R1][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board14.squares[R1][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board14.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board14.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board14.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    minimax_board_t board15;
    board15.squares[R0][C0] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board15.squares[R0][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board15.squares[R0][C2] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board15.squares[R1][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board15.squares[R1][C1] = MINIMAX_O_SQUARE; //Puts an O in that spot
    board15.squares[R1][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board15.squares[R2][C0] = MINIMAX_X_SQUARE; //Puts an X in that spot
    board15.squares[R2][C1] = MINIMAX_EMPTY_SQUARE; //Puts an empty in that spot
    board15.squares[R2][C2] = MINIMAX_X_SQUARE; //Puts an X in that spot

    uint8_t row, column;

    minimax_computeNextMove(&board1, true, &row, &column);      // true means X is current player.
    printf("next move for board1: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board2, true, &row, &column);      // true means X is current player.
    printf("next move for board2: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board3, false, &row, &column);      // true means X is current player.
    printf("next move for board3: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board4, true, &row, &column);     // false means O is current player.
    printf("next move for board4: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board5, false, &row, &column);     // false means O is current player.
    printf("next move for board5: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board6, true, &row, &column);     // true means 1 is current player.
    printf("next move for board6: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board7, false, &row, &column);     // false means O is current player.
    printf("next move for board7: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board8, true, &row, &column);      //true means X
    printf("next move for board8: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board9, false, &row, &column);     //false means O
    printf("next move for board9: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board10, true, &row, &column); // True means x
    printf("next move for board10: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board11, false, &row, &column);    //False means O
    printf("next move for board11: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board12, true, &row, &column); //True means X
    printf("next move for board12: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board13, false, &row, &column);    //False means O
    printf("next move for board13: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board14, false, &row, &column);    //False means O
    printf("next move for board14: (%d, %d)\n\r", row, column);

    minimax_computeNextMove(&board15, false, &row, &column);    //False means O
    printf("next move for board15: (%d, %d)\n\r", row, column);


    *
    */
}

void isr_function() {

  // Leave blank for the flag method.
  // I already set the flag for you in another routine.
}
