#ifndef _BUTTON_DEBOUNCE_H_
#define _BUTTON_DEBOUNCE_H_

#include <avr/io.h>

#define INI_BUTTON_STRUCT  .releaseCounter = 0, .pressCounter = 0, .buttonPressedState = 0, .buttonFallingEdge = 0,.buttonPreviousState = 0, .buttonState = 0, .new_state  = 0,.buttonRisingEdge = 0

// Button Structure

struct button_variables 
{
	int buttonPin;
	uint8_t new_state;
	
	double pressCounter;
	double releaseCounter;
	
	uint8_t buttonPressedState;
	uint8_t buttonPreviousState;
	
	
	uint8_t buttonRisingEdge;
	uint8_t buttonFallingEdge;
	uint8_t buttonState;
};

// Debounce Functions

void readButton(struct button_variables *btn);
void debounceButton(struct button_variables *btn);


#endif