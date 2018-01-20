#include "ticTacToeControl.h"
#include "supportFiles/display.h"
#include "ticTacToeAlgorithm.h"
#include "ticTacToeDisplay.h"
#include <stdbool.h>
#include "../lab2/buttons.h"
#include <stdio.h>
#include <stdlib.h>

#define ADC_COUNTER_MAX_VALUE 1 //Max value of the adc Counter.
#define START_GAME_MAX_VALUE 40 //Max value of star game Counter
#define WELCOME_MAX_VALUE 40 //Max value of welcome screen
#define R0 0 //Row 0
#define R1 1 //Row 1
#define R2 2 //Row 2
#define C0 0 //Column 0
#define C1 1 //Column 1
#define C2 2 //Column 2
#define TURN_COMPUTER 0
#define TURN_PLAYER 1
#define COMPUTER_VALUE_X 0
#define COMPUTER_VALUE_O 1
#define SQUARE_FULL 1 //Full Square on board
#define SQUARE_EMPTY 0 //Empty Square on board
#define BUTTONS_BTN0_MASK 0x1 //The button 0 mask
#define MAX_ROW 3
#define MAX_COLUMN 3
#define WELCOME_X 0
#define WELCOME_Y 0
#define DEFAULT_TEXT_SIZE 3
#define INIT_VALUE 0

// States for the controller state machine.
enum ticTacToeControl_st_t {
    init_st,                 // Start here, transition out of this state on the first tick.
    welcome_screen_st,      //For the welcome screen
    start_game_wait_st,        // Wait here until the first touch or until the computer goes
    comp_plays_st,    // Minimax algorithm is run
    adc_timer_running_st,     // waiting for the touch-controller ADC to settle.
    player_plays_st,   // Computes and draws where the player played
    game_wait_st,   // Waits for the player to play or transitions for the computer to play
    end_game_st,   // The end

} currentState;

minimax_board_t gameBoard;

//Period 50ms

int16_t adcCounter = INIT_VALUE; //Debounce counter
int16_t startGameCounter = INIT_VALUE; //Start game counter
int16_t welcomeScreenCounter = INIT_VALUE;
uint8_t computerValue = INIT_VALUE;
uint8_t turnValue = INIT_VALUE;

bool isGameOver = false;


//////////Helper Functions////////////

void ticTacToeControl_initBoard(){
   gameBoard.squares[R0][C0] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R0][C1] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R0][C2] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R1][C0] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R1][C1] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R1][C2] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R2][C0] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R2][C1] = MINIMAX_EMPTY_SQUARE;
   gameBoard.squares[R2][C2] = MINIMAX_EMPTY_SQUARE;
}

//Gets if the square that the user has tapped on is already full
int ticTacToeControl_squareIsFull(uint8_t row, uint8_t column){

    //Is the selected spot an empty square
    if(gameBoard.squares[row][column] == MINIMAX_EMPTY_SQUARE){

        //Returns that it is not full
        return SQUARE_EMPTY;

    } else {

        //Returns that it is full
        return SQUARE_FULL;
    }

}


void ticTacToeControl_resetScreen(){
    for(int i = INIT_VALUE; i < MAX_ROW; i++){
        for(int j = INIT_VALUE; j < MAX_COLUMN; j++){
            if(gameBoard.squares[i][j] == MINIMAX_X_SQUARE){
                //Draws an X
                ticTacToeDisplay_drawX(i, j, true);
            } else {
                //Draws an O
                ticTacToeDisplay_drawO(i, j, true);
            }
        }
    }
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static ticTacToeControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    //printf("msCounter:%d\n\r", msCounter);
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf("init_st\n\r");
        break;
      case start_game_wait_st:
        printf("start_game_wait_st\n\r");
        break;
      case comp_plays_st:
        printf("comp_plays_st\n\r");
        break;
      case adc_timer_running_st:
        printf("adc_timer_running_st\n\r");
        break;
      case player_plays_st:
        printf("player_plays_st\n\r");
        break;
      case game_wait_st:
        printf("game_wait_st\n\r");
        break;
      case end_game_st:
        printf("end_game_st\n\r");
        break;
     }
  }
}

//////////State Machine //////////////
void ticTacToeControl_init(){
    currentState = init_st;

    //Inits the game board
    ticTacToeControl_initBoard();
}

void ticTacToeControl_tick() {

  //debugStatePrint();

  // Perform state update first.
  switch(currentState) {

  //The Initial state
    case init_st:
    {
        //Changes the state to start_game_wait_st
          currentState = welcome_screen_st;

          //Sets the text size
          display_setTextSize(DEFAULT_TEXT_SIZE);

          //Prints
          display_println("Welcome To Tic\n\r Tac Toe\n\r");

    }
      break;

    case welcome_screen_st:
    {
        //Checks to see if its at the right value
        if(welcomeScreenCounter == WELCOME_MAX_VALUE){

            //Sets the current state
           currentState = start_game_wait_st;

           //sets the cursor
           display_setCursor(WELCOME_X,WELCOME_Y);

           //Sets the color to black
           display_setTextColor(DISPLAY_BLACK);

           //Prints the welcome
           display_println("Welcome To Tic\n\r Tac Toe\n\r");

           //Draws the board lines
           ticTacToeDisplay_drawBoardLines();

        }
    }

    break;

  //In this state it waits for the player to play
    case start_game_wait_st:
    {
        //Checks to see if display is touched
        if(display_isTouched()){

            //Clears touch data
            display_clearOldTouchData();

            //Sets the current state the adc state
            currentState = adc_timer_running_st;

            //Sets the computer value to O
            computerValue = COMPUTER_VALUE_O;

        }

        //If the player has waited for the computer to go.
        if(!display_isTouched() && startGameCounter == START_GAME_MAX_VALUE){

            //Row and column
            uint8_t row, column;

            //Seeds a random generator
            srand(rand());

            //Generates random row
            row = rand() % MAX_ROW;

            //Generates random column
            column = rand() % MAX_COLUMN;

            //Draws the x
            ticTacToeDisplay_drawX(row, column, false);

            //sets the spot to an x on the board
            gameBoard.squares[row][column] = MINIMAX_X_SQUARE;

            //Sets the current state to the computer playing
            currentState = game_wait_st;

            turnValue = TURN_PLAYER;

            //Sets the computer value to x
            computerValue = COMPUTER_VALUE_X;

        }
    }
      break;

    case comp_plays_st:
    {

        //Sets the currentState to the game wait state
        currentState = game_wait_st;

    }
      break;

    case adc_timer_running_st:
    {
        //Checks to see if it is touched and the counter has reached the max value
        if(display_isTouched() && adcCounter == ADC_COUNTER_MAX_VALUE){

            //Changes the state
            currentState = player_plays_st;

            //Sets the counter back to 0
            adcCounter = INIT_VALUE;
        }

        //Checks to see if somehow the player didn't touch
        if(!display_isTouched()){

            //Changes the state back to start game wait
            currentState = game_wait_st;
        }

    }
      break;

    case player_plays_st:
    {
        //Sets the currentState to the game wait state
        currentState = game_wait_st;
    }
      break;

    case game_wait_st:
    {
        if(isGameOver){

            //Sets the current state to the end game
            currentState = end_game_st;

        } else {

            //Is it the players turn and have they touched the screen?
            if(turnValue == TURN_PLAYER && display_isTouched()){

                //Clears old touch data
                display_clearOldTouchData();

                //Sets the current state
                currentState = adc_timer_running_st;

            } else if(turnValue == TURN_COMPUTER){

                //Sets the current state
                currentState = comp_plays_st;

            }

        }
    }

      break;

    case end_game_st:
    {
        //Reads the buttons
        int32_t buttons = buttons_read();

        //Checks to see if the button 1 is pressed
        if(buttons & BUTTONS_BTN0_MASK){

            //Resets the screen
            ticTacToeControl_resetScreen();

            //Sets the start game counter to 0
             startGameCounter = INIT_VALUE;

             //Re inits the board
             ticTacToeControl_initBoard();

            //Sets the current State back to the beginning
            currentState = start_game_wait_st;

            //Sets the isGameOver back to false;
            isGameOver = false;

        }
    }

      break;

    default:

        //Prints
      printf("clockControl_tick state update: hit default\n\r");

      break;
  }

  // Perform state action next.
  switch(currentState) {
      case init_st:

        break;

      case welcome_screen_st:
         {
             //Counts up for the welcome screen
             welcomeScreenCounter++;
         }

         break;

        //The beginning of the game
      case start_game_wait_st:

          //Sets the adcCounter
          adcCounter = INIT_VALUE;

          //Increments the start game counter
          startGameCounter++;

        break;

        //The meat of the program when the computer plays
      case comp_plays_st:

          //The row and column for the next move
          uint8_t row, column;

          //Checks to see if the computer is x or o
          if(computerValue == COMPUTER_VALUE_X){

              //Computes the next move for the computer to make as X
              minimax_computeNextMove(&gameBoard, true, &row, &column);

              //Draws an x
              ticTacToeDisplay_drawX(row, column, false);

              //Sets the spot on the board to what the computer chose
              gameBoard.squares[row][column] = MINIMAX_X_SQUARE;

              //Checks to see if the game is over
              if(minimax_isGameOver(minimax_computeBoardScore(&gameBoard, true))){

                  //The game is over
                  isGameOver = true;
              }

          } else {

              //Computes the next move for the computer as O
              minimax_computeNextMove(&gameBoard, false, &row, &column);

              //Draws an O
              ticTacToeDisplay_drawO(row, column, false);

              //Sets the chosen spot on the board
              gameBoard.squares[row][column] = MINIMAX_O_SQUARE;

              //Checks to see if the game is over
              if(minimax_isGameOver(minimax_computeBoardScore(&gameBoard, false))){

                  //The game is over
                  isGameOver = true;
              }

          }

          //Sets the current turn to players
          turnValue = TURN_PLAYER;


        break;

      case adc_timer_running_st:

          //Increments the counter
          adcCounter++;

        break;

      case player_plays_st:

      {
          //The row and column for the next move
          uint8_t row, column;

          //Gets the touch screen info
          ticTacToeDisplay_touchScreenComputeBoardRowColumn(&row, &column);

          //Checks to see if the space is available for play
          if(!ticTacToeControl_squareIsFull(row, column)){

              //Checks to see if it should be an O or an X
              if(computerValue != COMPUTER_VALUE_X){

                //Draws an X
                ticTacToeDisplay_drawX(row, column, false);

                //Sets the space to an x on the board
                gameBoard.squares[row][column] = MINIMAX_X_SQUARE;

                //Checks to see if the game is over
                if(minimax_isGameOver(minimax_computeBoardScore(&gameBoard, true))){

                    //The game is over
                    isGameOver = true;
                }

              } else {

                  //Draws an O
                  ticTacToeDisplay_drawO(row, column, false);

                  //Sets the chosen spot on the board
                  gameBoard.squares[row][column] = MINIMAX_O_SQUARE;

                  //Checks to see if the game is over
                  if(minimax_isGameOver(minimax_computeBoardScore(&gameBoard, false))){

                      //The game is over
                      isGameOver = true;
                  }

              }

              //Sets the current turn to computers
              turnValue = TURN_COMPUTER;
          }
      }

        break;

      case game_wait_st:

          //Nothing happens here


        break;

      case end_game_st:

          //Nothing happens



        break;

      default:
        printf("clockControl_tick state update: hit default\n\r");
        break;
  }
}
