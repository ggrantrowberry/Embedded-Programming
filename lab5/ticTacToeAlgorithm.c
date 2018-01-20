//
//  minimax.c
//  Lab5 Algorithm
//
//  Created by Grant Rowberry on 10/16/17.
//  Copyright © 2017 Grant Rowberry. All rights reserved.
//

#include "ticTacToeAlgorithm.h"
#include <stdio.h>

#define R0 0 //Row 0
#define R1 1 //Row 1
#define R2 2 //Row 2
#define C0 0 //Column 0
#define C1 1 //Column 1
#define C2 2 //Column 2
#define MAX_MOVES 9 //How many moves there can be in the table
#define MAX_SCORES 9 //How many scores there can be in the table
#define FIRST_INDEX 0 //The first index in the table is 0
#define PRINT_NEW_LINE 2
#define INIT_VALUE 0
minimax_move_t choice_g;

//-------------------------
//Helper Functions
//-------------------------

//Returns the current player's number in the board
int8_t minimax_getCurrentPlayerNumber(bool player_is_x){

    //Checks to see if the player is x
    if(player_is_x){

        //Returns a 2

        return MINIMAX_X_SQUARE;

    } else {

        //Returns a 1
        return MINIMAX_O_SQUARE;

    }
}

//Returns the current players score
int8_t minimax_getCurrentPlayerWinningScore(bool player_is_x){

    //Checks to see if the player is x
    if(player_is_x){

        //returns x's winning score
        return(MINIMAX_X_WINNING_SCORE);

    } else {

        //returns o's winning score
        return(MINIMAX_O_WINNING_SCORE);

    }

}

//Prints the board
void minimax_printBoard(minimax_board_t* board){

    //Iterates through the board
    for(int i = INIT_VALUE; i < MINIMAX_BOARD_ROWS; i++){
        for(int j = INIT_VALUE; j < MINIMAX_BOARD_COLUMNS; j++){

            //if its an x
            if(board ->squares[i][j] == MINIMAX_X_SQUARE){

                //Prints x
                printf("X");


             //id its an O
            } else if(board ->squares[i][j] == MINIMAX_O_SQUARE) {

                //Prints O
                printf("O");

            } else {

                //Prints _
                printf("_");
            }

            //Puts a new line
            if(j == PRINT_NEW_LINE){
                //prints a new line
                printf("\n\r");
            }

        }
    }
}

//Checks to see if the board is full
bool minimax_isBoardFull(minimax_board_t* board){

    //Iterates through the whole board to see if all the spaces are full
    for(int i = INIT_VALUE; i < MINIMAX_BOARD_ROWS; i++){

        for(int j = INIT_VALUE; j < MINIMAX_BOARD_COLUMNS; j++){

            //Checks to see if the space has been filled or not
            if(board -> squares[i][j] == MINIMAX_EMPTY_SQUARE){

                //Returns false if a space has been filled.
                return false;

            }

        }

    }

    //If it made it out of the iterations without finding an empty space than that means the board must be full.
    return true;
}

//Gets most desirable score index
int8_t minimax_getScoreIndex(minimax_score_t scores[], bool player_is_x, uint8_t tableIndex){

    minimax_score_t score = scores[INIT_VALUE];
    uint8_t index = INIT_VALUE;

    //Iterates through score
    for(int i = INIT_VALUE; i < tableIndex; i++){

        //Checks to see if player is x
        if(player_is_x) {

            //Is it a 10?
            if(scores[i] > score){

                //Sets the score
                score = scores[i];
                index = i;
            }

        } else {

            //Is it a -10?
            if(scores[i] < score){

                //returns i
                score = scores[i];
                index = i;

            }
        }
    }

    return index;

    //If all else fails. It shouldn't
    //return 0;

}


//Recursive function for minimax
minimax_score_t minimax_recursion(minimax_board_t* board, bool player_is_x){

    //Sets the table index to 0 to begin with
    int8_t tableIndex = FIRST_INDEX;

    //Move array
    minimax_move_t moves[MAX_MOVES] = {INIT_VALUE};

    //Scores array
    minimax_score_t scores[MAX_SCORES] = {INIT_VALUE};

    //Gets the score for the board
    minimax_score_t score = minimax_computeBoardScore(board, !player_is_x);

    //checks to see if the game is over
    if(minimax_isGameOver(score)){

        //returns the score
        return score;

    }

    //Iterates over all the possible moves
    for(int i = INIT_VALUE; i < MINIMAX_BOARD_ROWS; i++){

        for(int j = INIT_VALUE; j < MINIMAX_BOARD_COLUMNS; j++){

            //Checks to see if the square is empty
            if(board -> squares[i][j] == MINIMAX_EMPTY_SQUARE){

                //Checks to see if current player is x
                if(player_is_x){

                    //Sets that spot to an X
                    board -> squares[i][j] = MINIMAX_X_SQUARE;

                } else {

                    //Sets that spot to an O
                    board -> squares[i][j] = MINIMAX_O_SQUARE;

                }

                //Runs the recursive algorithm again
                score = minimax_recursion(board, !player_is_x);

                //Puts the row of the move in the table
                moves[tableIndex].row = i;

                //Puts the column of the move in the table
                moves[tableIndex].column = j;

                //Puts the score in the table
                scores[tableIndex] = score;

                //Increments the tableIndex
                tableIndex++;

                //Undo the change to the board
                board -> squares[i][j] = MINIMAX_EMPTY_SQUARE;
            }

        }
    }

    //Gets the correct index for the score
    int8_t index = minimax_getScoreIndex(scores, player_is_x, tableIndex);

    //sets the choice to be the correct one
    choice_g = moves[index];

    //Returns the best score
    return scores[index];
}


//-------------------------
// .h Defined Functions
//-------------------------

//Checks to see if the game is over
bool minimax_isGameOver(minimax_score_t score){

    //Returns if the game is over or not.
    return(score != MINIMAX_NOT_ENDGAME);

}


// Returns the score of the board, based upon the player and the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the player argument makes it possible to speed up this function.
// Assumptions:
// (1) if current_player_is_x == true, the last thing played was an 'X'.
// (2) if current_player_is_x == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.

minimax_score_t minimax_computeBoardScore(minimax_board_t* board, bool player_is_x){

    //gets the current player number
    int8_t currentPlayerNumber = minimax_getCurrentPlayerNumber(player_is_x);

    //Checks all horizontal combinations
    if((board -> squares[R0][C0] == currentPlayerNumber &&
       board -> squares[R0][C1] == currentPlayerNumber &&
       board -> squares[R0][C2] == currentPlayerNumber) ||
       (board -> squares[R1][C0] == currentPlayerNumber &&
       board -> squares[R1][C1] == currentPlayerNumber &&
       board -> squares[R1][C2] == currentPlayerNumber) ||
       (board -> squares[R2][C0] == currentPlayerNumber &&
       board -> squares[R2][C1] == currentPlayerNumber &&
       board -> squares[R2][C2] == currentPlayerNumber)){

       //If the above evaluates to true then the currentPlayer has one the game
           return minimax_getCurrentPlayerWinningScore(player_is_x);

           //Checks all vertical combinations
       } else if((board -> squares[R0][C0] == currentPlayerNumber &&
                  board -> squares[R1][C0] == currentPlayerNumber &&
                  board -> squares[R2][C0] == currentPlayerNumber) ||
                 (board -> squares[R0][C1] == currentPlayerNumber &&
                  board -> squares[R1][C1] == currentPlayerNumber &&
                  board -> squares[R2][C1] == currentPlayerNumber) ||
                 (board -> squares[R0][C2] == currentPlayerNumber &&
                  board -> squares[R1][C2] == currentPlayerNumber &&
                  board -> squares[R2][C2] == currentPlayerNumber)) {

                     //If the above evaluates to true then the currentPlayer has one the game
                     return minimax_getCurrentPlayerWinningScore(player_is_x);

                     //Checks all diagonal combinations
                 } else if((board -> squares[R0][C0] == currentPlayerNumber &&
                            board -> squares[R1][C1] == currentPlayerNumber &&
                            board -> squares[R2][C2] == currentPlayerNumber) ||
                           (board -> squares[R0][C2] == currentPlayerNumber &&
                            board -> squares[R1][C1] == currentPlayerNumber &&
                            board -> squares[R2][C0] == currentPlayerNumber)){

                               //If the above valuates to true then the currentPlayer has one the game
                               return minimax_getCurrentPlayerWinningScore(player_is_x);

                           } else if(minimax_isBoardFull(board)) {

                               //Returns a draw if the board is full.
                               return MINIMAX_DRAW_SCORE;

                           } else {

                               //If none of the above criteria are met than that must mean the game isn't over yet.
                               return MINIMAX_NOT_ENDGAME;

                           }

}

// Init the board to all empty squares.
void minimax_initBoard(minimax_board_t* board){

    //Iterates through the board and makes all squares empty
    for(int i = INIT_VALUE; i < MINIMAX_BOARD_ROWS; i++){

        for(int j = INIT_VALUE; j < MINIMAX_BOARD_COLUMNS; j++){

            //Sets the spot to empty
            board -> squares[i][j] = MINIMAX_EMPTY_SQUARE;
        }

    }

}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will implement in a later milestone.
// It computes the row and column of the next move based upon:
// the current board,
// the player. true means the computer is X. false means the computer is O.
// When called from the controlling state machine, you will call this function as follows:
// 1. If the computer is playing as X, you will call this function with current_player_is_x = true.
// 2. If the computer is playing as O, you will call this function with current_player_is_x = false.
// minimax_computeNextMove directly passes the current_player_is_x argument into the minimax() function.
void minimax_computeNextMove(minimax_board_t* board, bool current_player_is_x, uint8_t* row, uint8_t* column){

    //Prints the board
    //minimax_printBoard(board);

    //Runs the recursive algorithm,
    minimax_recursion(board, current_player_is_x);

    //Sets the row to the right row
    *row = choice_g.row;

    //Sets the column to the right column
    *column = choice_g.column;

}
