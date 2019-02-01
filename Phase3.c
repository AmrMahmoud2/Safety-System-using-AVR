/*
 * phase3.1.c
 *
 * Created: 14-Apr-18 10:34:43 PM
 * Author : Amr & Ahmed
 */ 


#include <avr/io.h>       //header to enable data flow control over pins
#define F_CPU 1000000    //telling controller crystal frequency attached
#include <util/delay.h> //header to enable delay function in program
#include <stdio.h>

//THE KEYPAD FUNCTION
#define KeyDR DDRA //Keypad DirectionRegister(determines whether port pins will be used for input or output)
#define KeyCtrl PORTA //KeypadControl to output data on to port pins
#define KeypadVal PINA //KeypadVal to read data from input pins (port in)
#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD			/* Define LCD data port */
#define RS PD0				/* Define Register Select pin */
#define EN PD1 				/* Define Enable signal pin */

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


void LCD_Command( unsigned char cmnd )
{
	LCD_Port = (LCD_Port & 0x0F) | (cmnd & 0xF0); /* sending upper nibble */
	LCD_Port &= ~ (1<<RS);		/* RS=0, command reg. */
	LCD_Port |= (1<<EN);		/* Enable pulse */
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (cmnd << 4);  /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}


void LCD_Char( unsigned char data )
{
	LCD_Port = (LCD_Port & 0x0F) | (data & 0xF0); /* sending upper nibble */
	LCD_Port |= (1<<RS);		/* RS=1, data reg. */
	LCD_Port|= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);

	_delay_us(200);

	LCD_Port = (LCD_Port & 0x0F) | (data << 4); /* sending lower nibble */
	LCD_Port |= (1<<EN);
	_delay_us(1);
	LCD_Port &= ~ (1<<EN);
	_delay_ms(2);
}

void LCD_Init (void)			/* LCD Initialize function */
{
	LCD_Dir = 0xFF;			/* Make LCD port direction as o/p */
	_delay_ms(20);			/* LCD Power ON delay always >15ms */
	
	LCD_Command(0x02);		/* send for 4 bit initialization of LCD   */
	LCD_Command(0x28);              /* 4bits,2 line, 5*7 matrix in 4-bit mode *///function set in datasheet
	LCD_Command(0x0c);              /* Display on cursor off*/// display on/ off control in data sheet
	LCD_Command(0x06);              /* Increment cursor (shift cursor to right)*///
	LCD_Command(0x01);              /* Clear display screen*///clear display in data sheet
	_delay_ms(2);
}


void LCD_String (char *str)		/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);
	}
}

void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD with xy position */
{
	if (row == 0 && pos<16)
	LCD_Command((pos & 0x0F)|0x80);	/* Command of first row and required position<16 */
	else if (row == 1 && pos<16)
	LCD_Command((pos & 0x0F)|0xC0);	/* Command of first row and required position<16 */
	LCD_String(str);		/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command (0x01);		/* Clear display */
	_delay_ms(2);
	LCD_Command (0x80);		/* Cursor at home position */
}

//THE MAIN

int main (void)
{
	KeyDR=0x0f;    //taking column pins as output and rows pins as input
	KeyCtrl=0xf0; //powering the column //pull up for 'f' in 0xf0

	const char password[4]= {'1','2','3','4'}; //the password  we will set
	char data[4]; //put the data that comes from keypad in this array
	char test='0';
	int times=0;
	LCD_Init();	
	

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
			LCD_Char(x);
		}

		//////////////////////// compare with password /////////////////////
		for (int i=0 ; i<=3 ; i=i+1)
		{
			if (data[i]==password[i])
			{
				test='1';
				_delay_ms(500);
				LCD_Clear();
			}
			else
			{
				test='0';
				_delay_ms(500);
				LCD_Clear();
			}
		}
		
		/////////////////////// ACTION /////////////////////
		if (test=='1')
		{
			LCD_String("Right Password");
			_delay_ms(2000);
			LCD_Clear();
			times=0;
		}
		else
		{
			LCD_String("Wrong Password");
			_delay_ms(2000);
			LCD_Clear();
			times=times+1;
		}
		
		if (times==5)
		{
			
			for (int i=1 ; i<=30 ; i++)
			{ 
				char buffer[3];
				snprintf(buffer, 3 , "%d", i);
				LCD_String("Please wait");
				LCD_Command(0xc3);
				LCD_String(buffer);
				_delay_ms(1000);
				LCD_Clear();
			}
			LCD_Clear();
		}
		
	}
	return 0;
}

