#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

#include <avr/io.h>

// Colors
#define OFF		{0,0,0}
#define WHITE {255,255,255}
#define RED	 	{255,0,0}
#define GREEN	{0,255,0}
#define BLUE	{0,0,255}
#define YELLOW 	{255,100,0}
#define PURPLE 	{255,0,255}
#define TURQUOISE {0,255,255}

uint8_t RGB[8][3];
int num_of_colors;

// Led driver sequence variables
struct Driver_sequence_struct
{
	int seqStep;
	uint8_t dim_value;
	uint8_t red_color_value;
	uint8_t green_color_value; 
	uint8_t blue_color_value;
	uint8_t	red_color_done;
	uint8_t	green_color_done;
	uint8_t	blue_color_done;
	unsigned long soft_transition_counter;

};


// Led Driver Functions
void blink(void);
void PWM_ini(void);
void write_R(uint8_t R_val);
void write_G(uint8_t G_val);
void write_B(uint8_t B_val);

#endif