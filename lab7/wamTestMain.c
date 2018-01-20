#include "wamDisplay.h"
#include "wamControl.h"
#include "supportFiles/utils.h"
#include "supportFiles/display.h"
//#include "supportFiles/intervalTimer.h"  // Modify as necessary to point to your intervalTimer.h
#include "supportFiles/leds.h"
#include "supportFiles/interrupts.h"
//#include "supportFiles/switches.h"  // Modify as necessary to point to your switches.h
//#include "supportFiles/buttons.h"   // Modify as necessary to point to your buttons.h
#include <stdio.h>
#include <xparameters.h>


int main(){

    //Runs the test
    wamDisplay_runMilestone1_test();
}

void isr_function() {
    // Empty for now.
}
