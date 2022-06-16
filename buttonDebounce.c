#include "buttonDebounce.h"


void readButton(struct button_variables *btn)
{
	// Read button pin 
	btn->new_state = (PIND & (1 << btn->buttonPin)) >> btn->buttonPin;
	
	// Deboubnce
	debounceButton(btn);
	
	// Detect rising edge
	if (btn->buttonPressedState == 1 && btn->buttonPreviousState == 0)
	{
		btn->buttonRisingEdge = 1;
		btn->buttonPreviousState = 1;
	}
	else if (btn->buttonPreviousState == 1)
	{
		btn->buttonRisingEdge = 0;
	}
	
	// Detect falling edge
	if (btn->buttonPressedState == 0 && btn->buttonPreviousState == 1)
	{
		btn->buttonFallingEdge = 1;
		btn->buttonPreviousState = 0;
	}
	else if (btn->buttonPreviousState == 0)
	{
		btn->buttonFallingEdge = 0;
	}
	
}
	
void debounceButton(struct button_variables *btn)
{
	// Debounce button press
	if (btn->new_state == 1 && btn->buttonPressedState == 0)
	{
		btn->pressCounter++;	
		if (btn->pressCounter >= 100)
		{
			btn->buttonPressedState = 1;
			btn->pressCounter = 0;
		}	
	} 
	else if (btn->new_state == 0 && btn->buttonPressedState == 0)
	{
		btn->pressCounter = 0;
	}
	// Debounce button release
	if (btn->new_state == 0 && btn->buttonPressedState == 1)
	{
		btn->releaseCounter++;
		if (btn->releaseCounter >= 100)
		{
			btn->buttonPressedState = 0;
			btn->releaseCounter = 0;
		}
	}
}
