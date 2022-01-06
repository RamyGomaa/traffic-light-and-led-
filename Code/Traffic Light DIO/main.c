#include "avr/io.h"
#include "avr/interrupt.h"
#include <util/delay.h>
#include "Mcal/DIO/DIO_Prog.h"
#include "Mcal/DIO/DIO_reg.h"
#include "LIB/BIT_MATH.h"
#include "Hal/LCD_interface.h"
#define F_CPU 800000
u8 arr_Dec_to_Seg[10]={
	//dot g f e d c b a
	0b00111111,//0
	0b01101111,//9
	0b01111111,//8
	0b00000111,//7
	0b01111101,//6
	0b01101101,//5		
	0b01100110,//4
	0b01001111,//3
	0b01011011,//2
	0b00000110,//1
	0b00111111,//0

};

u8 leds[3]={0b11100001,0b11100010,0b11100100}; // holding the led pins. first 3 ones are for the LCD
	char lcdString[3][5] = { // holding the strings that will be shown in the LCD
		{'s', 't', 'o', 'p','\0'},
		{'w', 'a', 'i','t','\0'},
		{'m', 'o', 'v', 'e','\0'}
	};	
u8 currentLed = 0; 
 // holding the different wait time for the leds.
u8 interval = 0; // to produce 10 seconds. works as a time interrupt that happens every second.

ISR(INT0_vect){

	
	currentLed = (currentLed+1) %3; // switching to the next led.
	if(currentLed==1)interval = 6; // reseting the interval to zero so it start counting from the start.
	else interval = 0;
	DIO_SetPortValue(PORTC,arr_Dec_to_Seg[interval]); // setting the 7 segment to 0
	DIO_SetPortValue(PORTB,leds[currentLed]); // lighting the led
	writeOnLCD();
	_delay_ms(1000);  // this should be 1000 (1 sec).
}

int main(void)

{	
	//LED init
		DIO_SetPortDirection(PORTB,0b11100000);//RS
		//DIO_SetPinDirection(PORTB,PIN7,HIGH);//RS
	//	DIO_SetPinDirection(PORTB,PIN6,HIGH);//RW
		//DIO_SetPinDirection(PORTB,PIN5,HIGH);//E
		//--------------------------------------------
		DIO_SetPortDirection(PORTA,0xFF);
		LCD_Init();
		LCD_CLR();
		Gotoyx(0,0);	
		
	//interrupt enabling.
	SET_BIT(GICR,6);
	SET_BIT(MCUCR,1);
	sei();
	
	DIO_SetPortDirection(PORTB,0b11100111); // the first three pins as output. for led.
	DIO_SetPortDirection(PORTC,0b11111111); // all pins as output. for the first 7 segment
	 //PortD as input. for the button.


	while (1){
		writeOnLCD();
	
		
		
		for(interval; interval <= 10; interval ++){ 
			 
			DIO_SetPortValue(PORTC,arr_Dec_to_Seg[interval]); // changing the 7 segment every interval.
			if(interval==0){
				DIO_SetPortValue(PORTB,leds[currentLed]);
				DIO_SetPortValue(PORTC,0b00111111);
				
				} // if the 10 intervals has finished and the interval counter was reset.
			
			
			_delay_ms(1000);  // this should be 1000 (1 sec).
			}
			
		// switching the lights
	    currentLed = (currentLed+1) %3; 
		if(currentLed == 1) interval = 6;
		else interval = 0; // resetting the interval so it can produce another 10 or 3 seconds.
		LCD_CLR();
	}
}

void writeOnLCD(){
	LCD_CLR();
	LCD_WriteString(lcdString[currentLed]); // writing in LCD
	if(currentLed == 0) LCD_WriteExtraChar(1,1); 
	else if( currentLed == 2) LCD_WriteSecondExtraChar(1,0); 
	_delay_ms(10);
}