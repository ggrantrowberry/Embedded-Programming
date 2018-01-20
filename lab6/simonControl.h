#include <stdbool.h>

// Initialize the state machine
void simonControl_init();

// Standard tick function.
void simonControl_tick();

//Displays welcome
void simonControl_welcomeDisplay(bool firstTime, bool erase);
