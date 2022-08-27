// DAC.c
// This software configures DAC output
// Lab 6 requires 6 bits for the DAC
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17
// Last Modified: 1/11/22
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Sound.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data

//#define DEBUG

// **************DAC_Init*********************
// Initialize 6-bit DAC, called once
// Input: none
// Output: none
void DAC_Init(void){
	volatile uint32_t delay;
	SYSCTL_RCGCGPIO_R |= 0x02; 

	delay = SYSCTL_PRGPIO_R; 
	GPIO_PORTB_DIR_R |= 0xBF; 
	GPIO_PORTB_DR8R_R |= 0x3F; 
	GPIO_PORTB_DEN_R |= 0xBF; 
	GPIO_PORTB_AFSEL_R &= ~0xBF;
}

// **************DAC_Out*********************
// output to DAC
// Input: 6-bit data, 0 to 63
// Input=n is converted to n*3.3V/63
// Output: none

#ifdef DEBUG
uint32_t wave_index = 0;
uint8_t wave[3] = {0, 128, 255};
#endif

void DAC_Out(uint32_t data){
	#ifdef DEBUG
		GPIO_PORTB_DATA_R = wave[wave_index]; // Data is written into Port B and resistors gets output;
		wave_index = (wave_index + 1) % 3;
	#endif
	#ifndef DEBUG
		GPIO_PORTB_DATA_R = data; // Data is written into Port B and resistors gets output;
	#endif
}

