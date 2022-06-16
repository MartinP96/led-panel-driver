/*
 * Led Driver 
 *
 * Author : Martin
 * Date: 04.12.2021
 *
 */ 
 
#include <avr/io.h>
#define F_CPU 8000000UL
#include <util/delay.h>
#include <avr/interrupt.h>

// Custom Files
#include "buttonDebounce.h"
#include "Rtc_modul.h"
#include "Led_driver.h"

#define status_led 			3
#define btn_select 			1
#define btn_dim_down		2
#define btn_dim_up			3
#define btn_custom_color 	4

// RTC variables
struct Real_time_data_struct
{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
	uint8_t turn_on_seconds;
	uint8_t turn_on_minutes;
	uint8_t turn_on_hours;
};
struct Real_time_data_struct real_time_data = {.seconds = 0, .minutes = 0, .hours = 0,
								.turn_on_seconds = 0, .turn_on_minutes = 0, .turn_on_hours = 6};

unsigned long rtc_refresh_time = 0;

// Tmp: Saved morning color
int color_index = 1;
int color_index_saved = 0;
int timer_done = 0;

int main(void)
{
	//Enable interrupts
	sei();
	
	//inicialize PWM timers
	PWM_ini();
	
	//RTC modul comm inicializacion
	Rtc_ini();
	
//  **********************************	
	// INITIALIZATION: Set time
	//RtcSendTime(0,18,21);
//  **********************************	
	
	TCCR0 |= (1 << CS02) | (1 << CS00);
	
	// Declare button structures
	struct button_variables Button_select = {.buttonPin = btn_select, INI_BUTTON_STRUCT};
	struct button_variables Button_dim_up = {.buttonPin = btn_dim_up,INI_BUTTON_STRUCT};
	struct button_variables Button_dim_down = {.buttonPin = btn_dim_down,INI_BUTTON_STRUCT};
	struct button_variables Button_custom_color = {.buttonPin = btn_custom_color,INI_BUTTON_STRUCT};
	
	// Set Input/Output pins
	DDRC |= (1 << status_led);
	DDRD &= ~(1 << Button_select.buttonPin);
	DDRD &= ~(1 << Button_dim_up.buttonPin);
	DDRD &= ~(1 << Button_dim_down.buttonPin);
	DDRD &= ~(1 << Button_custom_color.buttonPin);
	
	// Turn ON LEDs
	write_R(RGB[1][0]);
	write_G(RGB[1][1]);
	write_B(RGB[1][2]);
	
	int seqStep = 0;
	uint8_t dim_value = 0;
	uint8_t red_color_value = 255;
	uint8_t green_color_value = 255; 
	uint8_t blue_color_value = 255;
	uint8_t	red_color_done = 0;
	uint8_t	green_color_done = 0;
	uint8_t	blue_color_done = 0;
	
	// Counter Soft Transition Delay
	unsigned long soft_transition_counter = 0;
	
	while(1)
	{
		// Read buttons
		readButton(&Button_select);
		readButton(&Button_dim_up);
		readButton(&Button_dim_down);
		readButton(&Button_custom_color);
		
		// Control Sequence	
		switch(seqStep)
		{
			case 0: // Idle Wait for Button press
				
				// Tmp: Save morning turn on color
				if(Button_dim_down.buttonFallingEdge == 1)
				{
					color_index_saved = color_index;
					timer_done = 0;
				}
				
				// Transition: Select new color
				if (Button_select.buttonFallingEdge == 1)
				{
					color_index++;
					if (color_index >= num_of_colors)
					{
						color_index = 0;
					}
					
					// Reset LEDs states
					dim_value = 0;
					red_color_value = 0;
					green_color_value = 0;
					blue_color_value = 0;
					red_color_done = 0;
					green_color_done = 0;
					blue_color_done = 0;
					
					seqStep = 10;
				}
				
				// Transition: Turn On LEDS in the morning
				if (real_time_data.hours == real_time_data.turn_on_hours && real_time_data.minutes == real_time_data.turn_on_minutes && color_index_saved > 0 && timer_done == 0)
				{
					timer_done = 1;
					
					color_index = color_index_saved;
					
					// Reset LEDs states
					dim_value = 0;
					red_color_value = 0;
					green_color_value = 0;
					blue_color_value = 0;
					red_color_done = 0;
					green_color_done = 0;
					blue_color_done = 0;
						
					seqStep = 10;
				}
				
				// Transition: Turn Off LEDS in the morning
				if ()
				
				
				// Transition Dim LEDs down
				/*
				if(Button_dim_down.buttonFallingEdge == 1)
				{
					if (dim_value + 10 <= 255)
					{
						dim_value = dim_value + 10;
					} 
					seqStep = 20;
				}
				*/
				
				// Transition: Dim LEDs up
				/*
				if(Button_dim_up.buttonFallingEdge == 1)
				{
					
					if (dim_value - 10 > 0)
					{
						dim_value = dim_value - 10;
					}
					seqStep = 20;
				}
				*/		
			break;
						
			case 10: // Turn ON Leds - Soft Transition
			
				if (soft_transition_counter >= 10)
				{
				
					if(red_color_value < RGB[color_index][0])
					{
						red_color_value++;	
					}
					else
					{
						red_color_done = 1;
					}
					if(green_color_value < RGB[color_index][1])
					{
						green_color_value++;	
					}
					else
					{
						green_color_done = 1;
					}
					if(blue_color_value < RGB[color_index][2])
					{
						blue_color_value++;
						
					}
					else
					{
						blue_color_done = 1;
					}
		
					write_R(red_color_value);
					write_G(green_color_value);
					write_B(blue_color_value);
					
					soft_transition_counter = 0;
				}
				soft_transition_counter++;
				
				// Transition
				if((red_color_done == 1 && green_color_done == 1 && blue_color_done == 1) || Button_select.buttonFallingEdge == 1)
				{
					soft_transition_counter = 0;
					seqStep = 0;
				}
			break;
			
			case 20: // Dim LEDs Down
				
				if(red_color_value - dim_value > 0)
				{
					red_color_value = RGB[color_index][0] - dim_value;
				}
				if(green_color_value - dim_value > 0)
				{
					green_color_value = RGB[color_index][1] - dim_value;
				}
				if(blue_color_value - dim_value > 0)
				{
					blue_color_value = RGB[color_index][2] - dim_value;
				}
				
				write_R(red_color_value);
				write_G(green_color_value);
				write_B(blue_color_value);
				
				seqStep = 0;
			break;
		}
		
		// TMP:Read real clock time
		if (rtc_refresh_time >= 20000)
		{
			RtcReadTime(&real_time_data.seconds, &real_time_data.minutes, &real_time_data.hours);
			rtc_refresh_time = 0;
			
			// TEST
			//PORTC ^= (1 << status_led);
		}
		rtc_refresh_time++;
		
		// Morning LEDs turn on enable
		if (color_index_saved > 0)
		{
			PORTC |= (1 << status_led);

		}
		else 
		{
			PORTC &= ~(1 << status_led);
		}
	}
}
 