/*
 * phase4 final.c
 *
 * Created: 20-Apr-18 6:50:54 PM
 * Author : Amr & Ahmed
 */ 

#include <avr/io.h>       //header to enable data flow control over pins
#define F_CPU 1000000    //telling controller crystal frequency attached
#include <util/delay.h> //header to enable delay function in program
#include <stdio.h>

//THE KEYPAD FUNCTION
#define KeyDR DDRC //Keypad DirectionRegister(determines whether port pins will be used for input or output)
#define KeyCtrl PORTC //KeypadControl to output data on to port pins
#define KeypadVal PINC //KeypadVal to read data from input pins (port in)
#define LCD_Dir  DDRD			/* Define LCD data port direction */
#define LCD_Port PORTD		/* Define LCD data port */
#define RS PD0				/* Define Register Select pin */
#define EN PD1 				/* Define Enable signal pin */

volatile uint8_t adcValue1;
volatile uint8_t adcValue2;



void ADC_Init(){
	DDRA = 0x00;	        /* Make ADC port as input */
	ADCSRA = 0x87;          /* Enable ADC, with freq/128  */
	ADMUX = 0x40;           /* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)
{
	ADMUX = 0x40 | (channel & 0x07);   /* set input channel to read */
	ADCSRA |= (1<<ADSC);               /* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));     /* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);               /* Clear interrupt flag */
	_delay_ms(1);                      /* Wait a little bit */
	return ADCW;                       /* Return ADC word */
}

char passwordA[4]= {'4','5','6','B'}; //the password  we will set
char passwordB[4]={'1','1','1','1'};
char passwordC[4]={'2','2','2','2'};
char passwordD[4]={'3','3','3','3'};

char data[4];
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

void change_pass()
{
	LCD_String("EnterSystemPass");
	LCD_Command(0xc3);
	for (int i=0; i<4; i++)
	{
		char x=keypadScan();
		while (x=='z')
		{
			_delay_ms(50);
			x=keypadScan();
		}
		passwordA[i]=x;
		LCD_Char(x);
	}
	_delay_ms(100);
	LCD_Clear();
	LCD_String("Enter App1 Pass");
	LCD_Command(0xc3);
	for (int i=0; i<4; i++)
	{
		char x=keypadScan();
		while (x=='z')
		{
			_delay_ms(50);
			x=keypadScan();
		}
		passwordB[i]=x;
		LCD_Char(x);
	}
	_delay_ms(100);
	LCD_Clear();
	LCD_String("Enter App2 Pass");
	LCD_Command(0xc3);
	for (int i=0; i<4; i++)
	{
		char x=keypadScan();
		while (x=='z')
		{
			_delay_ms(50);
			x=keypadScan();
		}
		passwordC[i]=x;
		LCD_Char(x);
	}
	_delay_ms(100);
	LCD_Clear();
	LCD_String("Enter App3 Pass");
	LCD_Command(0xc3);
	for (int i=0; i<4; i++)
	{
		char x=keypadScan();
		while (x=='z')
		{
			_delay_ms(50);
			x=keypadScan();
		}
		passwordD[i]=x;
		LCD_Char(x);
	}
	_delay_ms(100);
	LCD_Clear();
}

void readPass()
{
	char x;
	for (int i=0 ; i<4 ;i=i+1)
	{
		
		x=keypadScan();
		while (x=='z')
		{
			_delay_ms(50);
			x=keypadScan();
		}
		data[i]=x;
		LCD_Char(x);
		_delay_ms(50);
		if(data[0]=='*')
		break;
		
	}
	LCD_Clear();
}


void app1(char *tempval)
{
	//char tempval[3];
	char x;
	int tempi;
	while(1)
	{
		float temp = (ADC_Read('0')*4.88);
		temp = (temp/10.00);
		
		 tempi =(int) temp; //int temperature value
		
		snprintf(tempval, 3, "%d", tempi);
		LCD_Clear();
		LCD_String("Temperature");
		LCD_Command(0xc3);
		LCD_String(&tempval[0]);
		_delay_ms(100);
		DDRB=0x0f;
		if(tempi>25)
		{
			
			PORTB = 0b00000001;
		}
		else
		PORTB = 0b00000000;
		x='p';
		x=keypadScan();
		if (x=='*')
		{
			LCD_Clear();
			PORTB = 0b00000000;
			break ;
		}
	}

}

int main (void)
{
	KeyDR=0x0f;    //taking column pins as output and rows pins as input
	KeyCtrl=0xf0; //powering the column //pull up for 'f' in 0xf0
	MCUCSR=(1<<JTD);
	MCUCSR=(1<<JTD);
	ADC_Init();
	
	//put the data that comes from keypad in this array
	int testA= 0;
	int testB= 0;
	int testC= 0;
	int testD= 0;
	
	int times=0;
	LCD_Init();
	

	
char  SavedTemp[3];
	///////////////////////////////////////////////////////////////////

	//THE LOOP

	while (1)
	{char x;
		//////////////////////// initializing the DATA ARRAY /////////////////////
		LCD_String("EnterSystemPass");
		LCD_Command(0xc3);
		for (int i=0 ; i<4 ;i=i+1)
		{
			x=keypadScan();
			while (x=='z')
			{
				_delay_ms(50);
				x=keypadScan();
			}
			data[i]=x;
			LCD_Char(x);
			_delay_ms(50);
			
		}
		LCD_Clear();

		//////////////////////// compare with sys password /////////////////////
		for (int i=0 ; i<=3 ; i=i+1)
		{
			if (data[i]==passwordA[i])
			{
				testA++;
			}
		}
		
		/////////////////////// ACTION 1 /////////////////////
		if (testA==4)
		{
			LCD_String("Right Password");
			_delay_ms(2000);
			LCD_Clear();
			times=0;
			testA=0;
			int q=1;
			while (q)
			{
				LCD_String("EnterAppPassword");
				LCD_Command(0xc3);
				readPass();
				
				
				//////////////////////// compare with app password /////////////////////
				
				for (int i=0 ; i<=3 ; i=i+1)
				{
					if(data [0]=='*')
					{
						q=0;
						break;
					}
					if (data[i]==passwordB[i])
					{
						testB++;
					}
					if (data[i]==passwordC[i])
					{
						testC++;
					}
					if (data[i]==passwordD[i])
					{
						testD++;
					}
				}
				if (testB==4)
				{
					testB=0;
					testC=0;
					testD=0;
					times=0;
					LCD_Clear();
					LCD_String("App1 Welcome");
					_delay_ms(1000);
					app1(&SavedTemp[0]);
					LCD_Clear();
					LCD_String("SavedTemp");
					LCD_String(SavedTemp);
					_delay_ms(2000);
					LCD_Clear();
					
				}
				else if (testC==4)
				{
					testB=0;
					testC=0;
					testD=0;
					times=0;
					LCD_Clear();
					LCD_String("App2 Working");
					while(1)
					{
						
						x=keypadScan();
						_delay_ms(50);
						if (x=='*')
						{
							LCD_Clear();
							break ;
						}
					}
				}
				else if (testD==4)
				{
					testB=0;
					testC=0;
					testD=0;
					times=0;
					LCD_Clear();
					change_pass();
					
					
				}
				else
				{
					testB=0;
					testC=0;
					testD=0;
					LCD_Clear();
					if(data[0]!='*')
					LCD_String("Wrong Password");
					else
					LCD_String("system closed");
					_delay_ms(2000);
					LCD_Clear();
					if(data[0]!='*')
					times=times+1;
				}
				if (times==5)
				{
					times=0;
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

		}
		/////////////////////// ACTION /////////////////////
		else
		{
			LCD_String("Wrong Password");
			_delay_ms(2000);
			LCD_Clear();
			times=times+1;
			testA=0;
		}

		if (times==5)
		{
			times=0;
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
