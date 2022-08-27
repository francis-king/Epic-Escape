// SpaceInvaders.c
// Runs on TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/12/2022 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php

// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// buttons connected to PE0-PE3
// 32*R resistor DAC bit 0 on PB0 (least significant bit)
// 16*R resistor DAC bit 1 on PB1
// 8*R resistor DAC bit 2 on PB2 
// 4*R resistor DAC bit 3 on PB3
// 2*R resistor DAC bit 4 on PB4
// 1*R resistor DAC bit 5 on PB5 (most significant bit)
// LED on PD1
// LED on PD0


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "TExaS.h"
#include "PLL.h"
#include "ADC.h"
#include "Images.h"
#include "Sound.h"
#include "Timer1.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

void Timer1A_Handler(void){ // can be used to perform tasks in background
  TIMER1_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER1A timeout
   // execute user task
}

void FiloEnding(void){ // can be used to perform tasks in background
ST7735_SetCursor(2,3);
									ST7735_OutString("May liwanag");
									ST7735_SetCursor(2,4);
									ST7735_OutString("sino ito");
									ST7735_SetCursor(2,5);
									ST7735_OutString("misteryosong tao");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_OutString("Oras na malaman");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_OutString("May babae dito");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_OutString("Congrats!");
									ST7735_FillScreen(0x0000);
									ST7735_OutString(" Tapus Na ");
}
void GeneralEnding(void){ // can be used to perform tasks in background
						Delay100ms(5);	//Add a delay so user doesn't know where the actual position is for next time they play
								ST7735_FillScreen(0xF81F);
								ST7735_SetCursor(5,5);
								ST7735_OutString("You wearily");
								ST7735_FillScreen(0xF81F);
								ST7735_SetCursor(5,8);
								Delay100ms(8);
								ST7735_FillScreen(0xF81F);
								ST7735_OutString("open the door");
								Delay100ms(5);
}
void EnglishEnding(void){ // can be used to perform tasks in background
	ST7735_SetCursor(2,3);
	ST7735_FillScreen(0xFFFF);
									ST7735_OutString("There is light");
									ST7735_SetCursor(2,4);
									ST7735_OutString("I wonder who");
									ST7735_SetCursor(2,5);
									ST7735_OutString("the mysterious");
									ST7735_SetCursor(2,6);
									ST7735_OutString("person is");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_SetCursor(2,6);
									ST7735_OutString("Time to find out");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_SetCursor(2,6);
									ST7735_OutString("A woman is there");
									Delay100ms(50);
									ST7735_FillScreen(0x0000);
									ST7735_SetCursor(2,6);
									ST7735_OutString("Congratulations!");
}



// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
      time--;
    }
    count--;
  }
}



void Init(void){
	volatile uint32_t delay;
	//Port B init
	SYSCTL_RCGCGPIO_R |= 0x02; 
	while((SYSCTL_PRGPIO_R&0x02) != 0x02){};		
	GPIO_PORTB_DIR_R |= 0xBF;	// PB5-0 out
	GPIO_PORTB_DR8R_R |= 0x3F;	
	GPIO_PORTB_DEN_R |= 0xBF;	// enable digital I/O on PB5-0
	GPIO_PORTB_AFSEL_R &= ~0xBF;
		
	//Port E init
	SYSCTL_RCGCGPIO_R |= 0x10;      
  while((SYSCTL_PRGPIO_R&0x10) != 0x10){};	
  GPIO_PORTE_DIR_R &=  0x00;   
	GPIO_PORTE_PDR_R |= 0x10;
  GPIO_PORTE_DEN_R |=  0x0F;   
  
}

void SysTick_Init(uint32_t period){
 // write this (copy from Lab 8)
	NVIC_ST_CTRL_R = 0;         									
  NVIC_ST_RELOAD_R = period-1;									
  NVIC_ST_CURRENT_R = 0;     									 
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000;
  NVIC_ST_CTRL_R = 0x0007; 										
}



struct EscapeRoom{
uint32_t Col[10]; 
};
typedef struct EscapeRoom room;

#define Row0 0
#define Row1 1
#define Row2 2
#define Row3 3
#define Row4 4
#define Row5 5
#define Row6 6
#define Row7 7
#define Row8 8
#define Row9 9
#define Wall	10
#define Gooo	11
#define	Exit	12
uint8_t x_value = 2;
uint8_t y_value = 2;
uint32_t input;
uint8_t language = 0;
uint8_t start = 0;  
uint8_t endgame = 1; 
uint8_t xLCD;
uint8_t yLCD;
uint32_t steps = 0;
uint32_t Data;
uint32_t MailInADC;
uint32_t ADCat;
uint8_t exitgame;
int32_t i = 1000;
int32_t x = 0;
int32_t z = 10;
int32_t placeholder = 0;
uint8_t a = 20;
uint8_t b = 100;
uint8_t c = 50;
uint8_t d = 120;
uint8_t e = 80;
uint16_t counter = 4;

room Room[10]={ 
	{{	Wall,	Wall,	Wall,	Wall,	Wall,	Wall,	Wall,	Wall, Wall, Wall, }},
	{{	Wall,	Gooo,	Wall,	Gooo,	Exit,	Gooo,	Gooo,	Gooo, Gooo, Wall, }},	
	{{	Wall,	Exit,	Gooo,	Gooo,	Gooo,	Gooo,	Gooo,	Wall, Gooo, Wall, }},
	{{	Wall,	Gooo,	Gooo,	Gooo,	Exit,	Gooo,	Gooo,	Gooo, Exit, Wall, }},	
	{{	Wall,	Gooo,	Gooo,	Wall,	Gooo,	Wall,	Gooo,	Wall, Gooo, Wall, }},
	{{	Wall,	Gooo,	Wall,	Gooo,	Gooo,	Gooo,	Gooo,	Gooo, Gooo, Wall, }},
	{{	Wall,	Gooo,	Gooo,	Exit,	Wall,	Gooo,	Wall,	Wall, Gooo, Wall, }},
	{{	Wall,	Gooo,	Wall,	Gooo,	Gooo,	Gooo,	Wall,	Exit, Gooo, Wall, }},
	{{	Wall,	Gooo,	Gooo,	Gooo,	Wall,	Gooo,	Gooo,	Gooo, Wall, Wall, }},
	{{	Wall,	Wall,	Wall,	Wall,	Wall,	Wall,	Wall,	Wall,	Wall, Wall, }},
};


//#define DEBUG

void SysTick_Handler(void){
  // write this
	GPIO_PORTF_DATA_R ^= 0x02;                  
	MailInADC = ADC_In();
	ADCat = 1;
	GPIO_PORTF_DATA_R ^= 0x02;                 
}
void somefunnyshoot(void){
	while (counter > 0){
	ST7735_FillScreen(0x0000);	
	ST7735_DrawBitmap(a,b,Bunker0,18, 5);	
	ST7735_DrawBitmap(c,d,PlayerShip0,18, 8);	
	ST7735_DrawBitmap(e,b,PlayerShip1,18, 5);	
	b = b - 5;
	d = d - 5;
	counter--;
	Delay100ms(2);
	}
}
void ending(void){
	ST7735_FillScreen(0x07E0);	// green screen
	Delay100ms(20);	
	ST7735_SetCursor(2,4);
	if(language == 1){ // English
		ST7735_FillScreen(0x0000);
		if (x < 0){
					x = i * x_value;
					i = x * y_value;
			}
  	ST7735_OutString("Found the Exit");
		ST7735_SetCursor(4,6);
		ST7735_OutString("Slide all the");
		ST7735_SetCursor(4,7);
		ST7735_OutString("way to open");
	}
	else if(language == 2){
		ST7735_FillScreen(0x0000);
		if (x < 0){
					x = i * x_value;
					i = x * y_value;
			}
		ST7735_OutString("Natagpuan Exit");
		ST7735_SetCursor(4,6);
		ST7735_OutString("Islide ");
		ST7735_SetCursor(4,7);
		ST7735_OutString("hanggang buksan");
	}
						
						
	Data = ADC_In();
	MailInADC = Data;
	ADCat = 1;
	while(exitgame == 0){
				while(ADCat == 0){}	
				Data = ADC_In();
				MailInADC = Data;
				x = (x + 10) % 64;
				i = (i + 10) % 64;
							
				if(MailInADC >= 0x0BFF) {	
					GeneralEnding();
					placeholder = 0;
					if(language == 1){
						EnglishEnding();
						if (i < 0){
							i = i + xLCD;
							x = x + yLCD;		
						}
						ST7735_SetCursor(2,6);
						ST7735_FillScreen(0x0000);
						ST7735_OutString("End Game");
						if (Room[y_value - 1].Col[x_value] == 6){
							i = x_value * y_value;
							z = x + yLCD;
							i++;
					}
						z = z -16;
						z = z + x_value * y_value;
				}
					else if(language == 2){
						FiloEnding();
						if (i < 0){
							i = i + xLCD;
							x = x + yLCD;
						}
						z = z -16;
						z = z + x_value * y_value;
				}
					exitgame = 1;
					placeholder = placeholder + 1;
			 }
							
	}
}

void printwallshoot(void){
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xB2);	// Output a wall
				somefunnyshoot();
				counter = 4;
				a = 20;
				b = 100;
				c = 50;
				d = 120;
				e = 80;
				steps = steps + 1;
				ST7735_SetCursor(0, 15);
				ST7735_OutString("Steps: ");
				ST7735_SetCursor(8, 15);	
				if (Room[y_value - 1].Col[x_value] == 6){
					placeholder = x_value * y_value;
					z = xLCD + yLCD;
					i++;
					x++;
				}
				LCD_OutDec(steps);		
				Sound_Killed();
				Delay100ms(2);	//Slow it down, no need to jam on the keys
}
void printnowallshoot(void){
				xLCD = x_value; //doesn't change
				yLCD = y_value; // +1 because of the offset but -1 cause moved up so cancels out
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xEC); // PI for person
				steps = steps + 1;
				a = 20;
				b = 100;
				c = 50;
				d = 120;
				e = 80;
				ST7735_SetCursor(0, 15);
				ST7735_OutString("Steps: ");
				ST7735_SetCursor(8, 15);	
				if (Room[y_value - 1].Col[x_value] == 6){
					placeholder = x_value * y_value;
					z = xLCD + yLCD;
					i++;
					x++;
				}
				LCD_OutDec(steps);		
				if (placeholder < 16){
					placeholder--;
					z = (1294 * x) + i + y_value;
					placeholder--;
				}
				Delay100ms(2);	//Slow it down, no need to jam on the keys
}

int main(void){ 
  DisableInterrupts();
  //PLL_Init(Bus80MHz);       // Bus clock is 80 MHz - Simulation
	TExaS_Init(SCOPE);
	ADC_Init();	
	EnableInterrupts();
	Output_Init();
  ST7735_FillScreen(0x0000);         
	Init();	
	SysTick_Init(8000000);
  EnableInterrupts();


	#ifdef DEBUG
		Sound_Init();
		while (1) {};
	#endif

  while(1){
		input = GPIO_PORTE_DATA_R;
		if(language== 0){	
			ST7735_SetCursor(0,0);
			// English Greeting
			
			ST7735_SetCursor(0,1);
			ST7735_OutString("Welcome to the");
			ST7735_SetCursor(0,2);
			ST7735_OutString("Escape Room");
			ST7735_SetCursor(2,4);
			ST7735_OutString("Press UP for the");
			ST7735_SetCursor(2,5);
			ST7735_OutString("English mode");
			if (z < 0){
					x = i + xLCD;
					i = x + yLCD;
			}
			else{
					i++;
			}
			// Filipino Greeting
			ST7735_SetCursor(0,7);
			ST7735_OutString("Maligayang pagdating");
			ST7735_SetCursor(0,8);
			ST7735_OutString("sa Escape Room");
			ST7735_SetCursor(2,10);
			ST7735_OutString("Pindutin ang DOWN");
			ST7735_SetCursor(2,11);
			ST7735_OutString("para piliin yung");			
			ST7735_SetCursor(2,12);
			ST7735_OutString("Filipino mode");	
			if (x < 0){
					x = i + xLCD;
					i = x + yLCD;
					placeholder = x_value + y_value;
			}
			else{
					i++;
			}			
			
			while(language == 0){	
				if(GPIO_PORTE_DATA_R == 1){	
					language = 1;
					i++;
					i = i % 64;
					ST7735_FillScreen(0xF81F);
					ST7735_OutString("English!");
					Delay100ms(10);
				}
				if(GPIO_PORTE_DATA_R == 2){ 
					language= 2; 
					i++;
					i = i % 64;
					ST7735_FillScreen(0xF81F);
					ST7735_OutString("Filo!");
					Delay100ms(10);
				}
			}
		}
		Sound_Explosion();
		
	//Game starts
		
		if(language != 0){	
			input = GPIO_PORTE_DATA_R;
			if(start == 0){	
				
				ST7735_FillScreen(0x0000);	
				if(language == 1){
					ST7735_SetCursor(1,2);
					ST7735_OutString("You wake up on");
					ST7735_SetCursor(1,3);
					ST7735_OutString("the cold floor");
					ST7735_SetCursor(1,4);
					ST7735_OutString("I'm waiting");
					ST7735_SetCursor(1,5);
					ST7735_OutString("at the exit!");
					ST7735_SetCursor(1,6);
					ST7735_OutString("a mysterious");
					ST7735_SetCursor(1,7);
					ST7735_OutString("mist fills");
					ST7735_SetCursor(1,8);
					ST7735_OutString("the room");
					ST7735_SetCursor(1,9);
					ST7735_OutString("Escape in 30 steps");
					ST7735_SetCursor(1,10);
					ST7735_OutString("for a prize!");
					Sound_Explosion();
					Delay100ms(50);

					Sound_Shoot();
					ST7735_FillScreen(0x0000);	
					ST7735_SetCursor(1,2);
					ST7735_OutString("Move one step");
					ST7735_SetCursor(1,3);
					ST7735_OutString("at a time");
					Delay100ms(30);
					ST7735_SetCursor(4,2);
					ST7735_FillScreen(0x0000);	// WHITE
					ST7735_OutString("-------");
					//ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,3);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,4);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,5);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,6);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,7);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,8);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,3);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,4);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,5);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,6);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,7);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,8);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,9);
					ST7735_OutString("-------");
					ST7735_SetCursor(6,5);
					ST7735_OutChar(0xF8);	// Output a wall
					ST7735_SetCursor(3,10);
					ST7735_OutString("Door Opening");	// Output a wall
					Sound_Explosion();
					Delay100ms(20);
					Sound_Shoot();
				}
				else if(language == 2){
					ST7735_SetCursor(1,2);
					ST7735_OutString("Madilim sa kwarto...");
					ST7735_SetCursor(1,3);
					ST7735_OutString("Ang iyong petsa");
					ST7735_SetCursor(1,4);
					ST7735_OutString("ay naghihintay");
					ST7735_SetCursor(1,5);
					ST7735_OutString("para sa iyo");
					ST7735_SetCursor(1,6);
					ST7735_OutString("labasan");
					Sound_Explosion();
					Delay100ms(50);
					
					ST7735_SetCursor(1,2);
					ST7735_OutString("Ilipat ang isang");
					ST7735_SetCursor(1,3);
					ST7735_OutString("hakbang sa");
					ST7735_SetCursor(1,4);
					ST7735_OutString("isang pagkakataon");
					Sound_Explosion();
					Delay100ms(40);
					ST7735_SetCursor(4,2);
					ST7735_FillScreen(0x0000);	// WHITE
					ST7735_OutString("-------");
					//ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,3);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,4);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,5);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,6);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,7);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,8);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,3);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,4);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,5);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,6);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,7);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(10,8);
					ST7735_OutChar(0xB2);	// Output a wall
					ST7735_SetCursor(4,9);
					ST7735_OutString("-------");
					ST7735_SetCursor(6,5);
					ST7735_OutChar(0xF8);	// Output a wall
					ST7735_SetCursor(3,10);
					ST7735_OutString("Door Opening");	// Output a wall
				}
				
				// Output the initial position
				xLCD= x_value;	
				yLCD= y_value;
				if (Room[y_value - 1].Col[x_value] == 1){
					placeholder = x_value * y_value;
					z = xLCD + yLCD;
					i++;
					x++;
				}
				ST7735_FillScreen(0x0000);   
				ST7735_SetCursor(xLCD, yLCD);	
				if (Room[y_value - 1].Col[x_value] == 4){
					placeholder = x_value * y_value;
					z = xLCD + yLCD;
					i++;
					x++;
				}
				ST7735_OutChar(0xEC); 
				start= 1;
			}
		}
		
		//If Up is pressed
		if( GPIO_PORTE_DATA_R == 1 ){
			if (steps == 30){
				ST7735_FillScreen(0xF800);
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Game Over");
			}
			else if( Room[y_value - 1].Col[x_value] == 10){	
				ST7735_FillScreen(0xA945);	
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xEC);
				if (x < 0){
					i = i * 16 + xLCD;
					x = x * 16 + yLCD;
				}
				xLCD = x_value; //doesn't change
				yLCD = y_value - 1; 
				printwallshoot();
			}
			else if(Room[y_value - 1].Col[x_value] == 11){	
				ST7735_FillScreen(0x0000); 
				y_value = y_value - 1; 
				if (x_value == 1){
					if (y_value == 1){
						ST7735_SetCursor(0, 13);
						ST7735_OutString("There is no light");
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Don't hide");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("Not in this corner");
					}
				}
				if (x_value == 8){
					if (y_value == 7){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("There is warmth");
						ST7735_SetCursor(0, 1);
						ST7735_OutString("from the left");
					}
				}
				if (x_value == 4){
					if (y_value == 2){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Two exits are");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("around you");
					}
				}
				if (x_value == 7){
					if (y_value == 8){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You feel the warmth");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("from above");
					}
				}
				if (x_value == 5){
					if (y_value == 5){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You see a");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("poorly disguised");
						ST7735_SetCursor(2, 0);
						ST7735_OutString("trap in the north");
					}
				}
				printnowallshoot();
			}
			else if(Room[y_value - 1].Col[x_value] == 12){	
				if (Room[y_value - 1].Col[x_value] == 1){
					placeholder = y_value - x_value;
					z = y_value + x_value;
					i++;
					x++;
				}
				ending();
			}
} //up button bracket
		
				
				
				
				
				
		
			//If Down is pressed
		if(input == 2 ){	

			if (steps == 30){
				ST7735_FillScreen(0xF800);	
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Game Over");
			}
			else if( Room[y_value + 1].Col[x_value] == 10){	
				ST7735_FillScreen(0xA945);	
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xEC); 
				if (x < 0){
					i = i * 16 + xLCD;
					x = x * 16 + yLCD;
				}
				xLCD = x_value; //doesn't change
				yLCD = y_value + 1; // moving down, y + 1
				printwallshoot();
			}
			else if(Room[y_value + 1].Col[x_value] == 11){	// No wall or door hit up so we can move player there
				if (Room[y_value - 1].Col[x_value] == 1){
					placeholder = y_value - x_value;
					z = y_value + x_value;
					i = i + 3;
					x = z + 3;
				}
				ST7735_FillScreen(0x0000);  
				y_value = y_value + 1;
				if (x_value == 1){
					if (y_value == 1){
						ST7735_SetCursor(0, 13);
						ST7735_OutString("There is no light");
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Don't hide");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("Not in this corner");
					}
				}
				if (x_value == 8){
					if (y_value == 7){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("There is warmth");
						ST7735_SetCursor(0, 1);
						ST7735_OutString("from the left");
					}
				}
				if (x_value == 4){
					if (y_value == 2){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Two exits are");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("around you");
					}
				}
				if (x_value == 7){
					if (y_value == 8){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You feel the warmth");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("from above");
					}
				}
				if (x_value == 5){
					if (y_value == 5){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You see a");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("poorly disgised");
						ST7735_SetCursor(2, 0);
						ST7735_OutString("trap in the north");
					}
				}
				printnowallshoot();

			}
			else if(Room[y_value + 1].Col[x_value] == 12){	// Basically if the door was found in 
					
			ending();

					}
				}
		//If LEFT is pressed
		if(input == 4 ){
			if (steps == 30){
				ST7735_FillScreen(0xF800);	
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Game Over");
			}
			else if( Room[y_value].Col[x_value -1] == 10){	
				ST7735_FillScreen(0xA945);
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xEC); // P for person
				//x_value = x_value - 1;
				if (z < 0){
					x = i + xLCD;
					i = x + yLCD;
			}
			else{
					i++;
			}
				xLCD = x_value - 1; //doesn't change
				yLCD = y_value; // moving down, y + 1
				i = i + 5;
				printwallshoot();
			}
			else if(Room[y_value].Col[x_value - 1] == 11){	
				ST7735_FillScreen(0x0000);
				x_value = x_value - 1; 
				if (x_value == 1){
					if (y_value == 1){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("There is no light");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("Don't hide");
						ST7735_SetCursor(0, 16);
						ST7735_OutString("Not in this corner");
					}
				}
					if (x_value == 8){
					if (y_value == 7){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("There is warmth");
						ST7735_SetCursor(0, 1);
						ST7735_OutString("from the left");
					}
				}
				if (x_value == 4){
					if (y_value == 2){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Two exits are");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("around you");
					}
				}
				if (x_value == 7){
					if (y_value == 8){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You feel the warmth");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("from above");
					}
				}
				if (x_value == 5){
					if (y_value == 5){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You see a");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("poorly disgised");
						ST7735_SetCursor(2, 0);
						ST7735_OutString("trap in the north");
					}
				}
				printnowallshoot();
			}
			else if(Room[y_value].Col[x_value - 1] == 12){	
					if (Room[y_value - 1].Col[x_value] == 1){
					placeholder = y_value - x_value;
					z = y_value + x_value;
					i++;
					x++;
				}
			ending();

					}
				}
		
		//If RIGHT is pressed
		if(input == 8 ){	
			if (steps == 30){
				ST7735_FillScreen(0xF800);	
				ST7735_SetCursor(3, 5);
				ST7735_OutString("Game Over");
			}
			else if( Room[y_value].Col[x_value + 1] == 10){	
				ST7735_FillScreen(0xA945);	
				ST7735_SetCursor(xLCD, yLCD);
				ST7735_OutChar(0xEC); 
				if (x < 0){
					i = i * 16 + xLCD;
					x = x * 16 + yLCD;
				}
				//x_value = x_value - 1;
				xLCD = x_value + 1; //right, x + 1
				yLCD = y_value; // doenst change
				printwallshoot();
			}
			else if(Room[y_value].Col[x_value + 1] == 11){	
				ST7735_FillScreen(0x0000);  
				x_value = x_value + 1;
				if (x_value == 1){
					if (y_value == 1){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("There is no light");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("Don't hide");
						ST7735_SetCursor(0, 16);
						ST7735_OutString("Not in this corner");
					}
				}
				if (x_value == 8){
					if (y_value == 7){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("There is warmth");
						ST7735_SetCursor(0, 1);
						ST7735_OutString("from the left");
					}
				}
				if (x_value == 4){
					if (y_value == 2){
						ST7735_SetCursor(0, 14);
						ST7735_OutString("Two exits are");
						ST7735_SetCursor(0, 15);
						ST7735_OutString("around you");
					}
				}
				if (x_value == 7){
					if (y_value == 8){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You feel the warmth");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("from above");
					}
				}
				if (x_value == 5){
					if (y_value == 5){
						ST7735_SetCursor(0, 0);
						ST7735_OutString("You see a poorly disgised");
						ST7735_SetCursor(1, 0);
						ST7735_OutString("poorly disgised");
						ST7735_SetCursor(2, 0);
						ST7735_OutString("trap in the north");
					}
				}
				printnowallshoot();

			}
			else if(Room[y_value].Col[x_value + 1] == 12){	
				if (Room[y_value - 1].Col[x_value] == 1){
					placeholder = y_value - x_value;
					z = y_value + x_value;
					i++;
					x++;
				}
				ending();

					}
				}
				
		} // while 1 bracket
	}

