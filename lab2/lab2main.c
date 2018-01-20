#include "switches.h"
#include "buttons.h"

int main() {

    //Runs the switch test
    switches_runTest();

    //Runs the button test
    buttons_runTest();
}


void isr_function() {
    // Empty for now.
}
