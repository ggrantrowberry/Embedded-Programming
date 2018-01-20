#include "globals.h"
#include <string.h>

#define INIT_VALUE 0
// The length of the sequence.
// The static keyword means that globals_sequenceLength can only be accessed
// by functions contained in this file.
static uint16_t globals_sequenceLength = INIT_VALUE;  // The length of the sequence.

static  uint8_t globals_sequence[GLOBALS_MAX_FLASH_SEQUENCE] = {INIT_VALUE};

static uint16_t globals_sequenceIterationLength = INIT_VALUE;

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the user works through the pattern one color at a time.
void globals_setSequenceIterationLength(uint16_t length) {globals_sequenceIterationLength = length;}

// You will need to implement the other functions.

// This is the length of the sequence that you are currently working on,
// not the maximum length but the interim length as
// the use works through the pattern one color at a time.
uint16_t globals_getSequenceIterationLength() {return globals_sequenceIterationLength;}

// This is the length of the complete sequence at maximum length.
// You must copy the contents of the sequence[] array into the global variable that you maintain.
// Do not just grab the pointer as this will fail.
void globals_setSequence(const uint8_t sequence[], uint16_t length){

    //loops through the sequence and copys
    for(int i = 0; i < length; i++){

        //Copys one value
        globals_sequence[i] = sequence[i];

    }

    //Sets the length
    globals_sequenceLength = length;


}

// This returns the value of the sequence at the index.
uint8_t globals_getSequenceValue(uint16_t index){return globals_sequence[index];}

// Retrieve the sequence length.
uint16_t globals_getSequenceLength() {return globals_sequenceLength;}
