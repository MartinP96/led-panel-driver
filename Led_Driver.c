#include "Led_Driver.h"


/*
 * Variable declarations
 * ----------------------------
 */

uint8_t RGB[8][3] = {OFF,WHITE,RED,GREEN,BLUE,YELLOW,PURPLE,TURQUOISE};
int num_of_colors = 8;

struct Driver_sequence_struct Driver_sequence = {.seqStep = 0, .dim_value = 0, .red_color_value = 0, 
												 .green_color_value = 0, .blue_color_value = 0,
												.red_color_done = 0, .green_color_done = 0, .blue_color_done = 0, 
												.soft_transition_counter = 0};



/*
 * Function: PWM_ini
 * ----------------------------
 *   Initializes PWM registers of atmega8 MCU
 *
 */
void PWM_ini(void)
{	
	//PWM Inicialization 
	DDRB |= (1 << 1) | (1 << 2) | (1 << 3);
	//16 bit timer, 8 bit pwm
	TCCR1A |= (1 << COM1A1) | (1 << COM1B1) | (1 << WGM10);
	TCCR1B |= (1 << WGM12);
	TCCR1B |= (1 << CS11);
	
	//8 bit timer, 8 bit pwm
	TCCR2 |= (1 << WGM20) | (1 << WGM21) | (1 << COM21) | (1 << CS20);
}


/*
 * Function: write_R
 * ----------------------------
 *   Write red color value to the PWM output
 *
 */
void write_R(uint8_t R_val)
{
	OCR1A = R_val;
	
	if(OCR1A == 0)
	{
		TCCR1A &= ~(1 << COM1A1);
		PORTB &=  ~(1 << 1);
	}
	else
	{
		TCCR1A |= (1 << COM1A1);
	}
	
}


/*
 * Function: write_G
 * ----------------------------
 *   Write green color value to the PWM output
 *
 */
void write_G(uint8_t G_val)
{
	OCR2 = G_val;
	
	if(OCR2 == 0)
	{
		TCCR2 &= ~(1 << COM21);
		TCCR2 &= ~(1 << COM20);
		PORTB &= ~(1 << 3);
	}
	else
	{
		TCCR2 |= (1 << COM21) | (1 << CS20);
	}
}


/*
 * Function: write_B
 * ----------------------------
 *   Write blue color value to the PWM output
 *
 */

void write_B(uint8_t B_val)
{
	OCR1B = B_val;
	
	if(OCR1B == 0)
	{
		TCCR1A &= ~(1 << COM1B1);
		PORTB &=  ~(1 << 2);
	}
	else
	{
		TCCR1A |= (1 << COM1B1);
	}
}
