/*
* phase1 final.c
*
* Created: 24-Feb-18 6:46:37 PM
* Author : Amr & Ahmed
*/

#include <avr/io.h>       //header to enable data flow control over pins
#define F_CPU 1000000    //telling controller crystal frequency attached
#include <util/delay.h> //header to enable delay function in program

//THE KEYPAD FUNCTION
#define KeyDR DDRA //Keypad DirectionRegister(determines whether port pins will be used for input or output)
#define KeyCtrl PORTA //KeypadControl to output data on to port pins
#define KeypadVal PINA //KeypadVal to read data from input pins (port in)
char keypadScan()
{
	if(KeypadVal == 0xf0)
	{
		return 'z';
	}
	
	_delay_ms(50);
	
	uint8_t keypressedCode=KeypadVal; //uint8_t means unsigned char //max value 255
	KeyDR ^= 0xff;
	KeyCtrl ^= 0xff;
	asm volatile("nop"); //do nothing
	asm volatile("nop");
	keypressedCode |=KeypadVal;
	_delay_ms(50);
	
	char x;
	if(keypressedCode == 0b11101110)
	x='1';
	else if(keypressedCode == 0b11011110)
	x='2';
	else if(keypressedCode == 0b10111110)
	x='3';
	else if(keypressedCode == 0b01111110)
	x='A';
	else if(keypressedCode == 0b11101101)
	x='4';
	else if(keypressedCode == 0b11011101)
	x='5';
	else if(keypressedCode == 0b10111101)
	x='6';
	else if(keypressedCode == 0b01111101)
	x='B';
	else if(keypressedCode == 0b11101011)
	x='7';
	else if(keypressedCode == 0b11011011)
	x='8';
	else if(keypressedCode == 0b10111011)
	x='9';
	else if(keypressedCode == 0b01111011)
	x='c';
	else if(keypressedCode == 0b11100111)
	x='*';
	else if(keypressedCode == 0b11010111)
	x='0';
	else if(keypressedCode == 0b10110111)
	x='#';
	else if(keypressedCode == 0b01110111)
	x='D';
	else
	{
		x='z';
	}
	if (x !='z')
	{
		DDRB = 0b00000010;
		PORTB |=0b00000010;
		_delay_ms(500);
		PORTB &=0b11111101;
	}
	return x;
}

//BLINKING FUNCTION

void blink ()
{
	PORTB |= 0b00000001;
	_delay_ms(500);
	PORTB &= 0b11111110;
	_delay_ms(500);
}

void blink_2()
{
	for (int i=0 ; i<=3 ; i=i+1)
	{
		PORTB |= 0b00000001;
		_delay_ms(500);
		PORTB &= 0b11111110;
		_delay_ms(500);
	}
}

//THE MAIN

int main (void)
{
	KeyDR=0x0f;    //taking column pins as output and rows pins as input
	KeyCtrl=0xf0; //powering the column //pull up for 'f' in 0xf0

	DDRB = 0b00000001; //put the led in the first pin in port B
	const char password[4]= {'1','2','3','4'}; //the password  we will set
	char data[4]; //put the data that comes from keypad in this array
	char test='0';
	int times=0;
	

	///////////////////////////////////////////////////////////////////

	//THE LOOP

	while (1)
	{char x;
		//////////////////////// initializing the DATA ARRAY /////////////////////
		for (int i=0 ; i<4 ;i=i+1)
		{ x=keypadScan();
			while (x=='z')
			{
				_delay_ms(50);
				x=keypadScan();
			}
			data[i]=x;
		}

		//////////////////////// compare with password /////////////////////
		for (int i=0 ; i<=3 ; i=i+1)
		{
			if (data[i]==password[i])
			{
				test='1';
			}
			else
			{
				test='0';
			}
		}
		
		/////////////////////// ACTION /////////////////////
		if (test=='1')
		{
			blink_2();
			times=0;
		}
		else
		{
			blink();
			times=times+1;
		}
		
		if (times==5)
		{
			_delay_ms(30000);
		}
		
	}
	return 0;
}

