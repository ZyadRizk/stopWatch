/*
 * project2.c
 *
 *  Created on: Feb 13, 2025
 *      Author: zyad
 */
#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>

unsigned char g_hours1=0 , g_mins1=0 , g_secs1=0 , g_hours2=0 , g_mins2=0 , g_secs2=0 , count=0 ;
unsigned char g_incrment_flag=0 , g_decrment_flag=0 ;
unsigned char g_mode_flag=1 ; //1 inc 0 dec
unsigned char g_mode_pb_flag=0;
unsigned char g_alarm_flag=0;
unsigned char hours_inc_flag=0  ,hours_dec_flag=0;
unsigned char mins_inc_flag=0  ,mins_dec_flag=0;
unsigned char secs_inc_flag=0  ,secs_dec_flag=0;

void INT0_init(void);
void INT1_init(void);
void INT2_init(void);
void TIMER1_COMPA_init(void);
void SEVEN_SEG_init(void);
void DISPLAY_UPDATE(void);
void TIMER_INC(void);
void TIMER_DEC(void);
void TIMER_MODE_TOGGEL(void);
void HOURS_DEC(void);
void HOURS_INC(void);
void MINS_DEC(void);
void MINS_INC(void);
void SECS_INC(void);
void SECS_DEC(void);


ISR(TIMER1_COMPA_vect)
{
	if(g_mode_flag)
	{
		g_incrment_flag=1;
	}
	else
	{
		g_decrment_flag=1;
	}
}

ISR(INT0_vect)
{
	g_secs2=0;
	g_secs1=0;
	g_mins2=0;
	g_mins1=0;
	g_hours2=0;
	g_hours1=0;
	g_alarm_flag=0;
}

ISR(INT1_vect)
{
	TIMSK &= ~(1<<OCIE1A);
}

ISR(INT2_vect)
{
	TIMSK |= (1<<OCIE1A);
}


int main(void)
{
	INT0_init();
	INT1_init();
	INT2_init();
	TIMER1_COMPA_init();
	SEVEN_SEG_init();

	SREG  |= (1<<7);
	DDRD  |= (1<<PD0) | (1<<PD4) | (1<<PD5);
	PORTD &= ~(1<<PD0) & ~(1<<PD4) & ~(1<<PD5);
	DDRB  = 0;
	PORTB =0XFF;

	while(1)
	{
		DISPLAY_UPDATE();
		TIMER_MODE_TOGGEL();
		if(g_mode_flag)
		{
			PORTD &= ~(1<<PD5) & ~(1<<PD0);
			PORTD |=  (1<<PD4);
			TIMER_INC();

		}
		else if(g_mode_flag == 0)
		{
			PORTD &= ~(1<<PD4);
			PORTD |=  (1<<PD5);
			TIMER_DEC();
			if(g_alarm_flag)
			{
				PORTD |= (1<<PD0);
			}
		}

		if(!(PINB & (1<<PB1))) //hours increment
		{
			HOURS_INC();
		}
		else
		{
			hours_inc_flag=0;
		}

		if(!(PINB & (1<<PB0)))//hours decrement
		{
			HOURS_DEC();
		}
		else
		{
			hours_dec_flag=0;
		}

		if(!(PINB & (1<<PB4)))
		{
			MINS_INC();
		}
		else
		{
			mins_inc_flag=0;
		}

		if(!(PINB & (1<<PB3)))
		{
			MINS_DEC();
		}
		else
		{
			mins_dec_flag=0;
		}

		if(!(PINB & (1<<PB6)))
		{
			SECS_INC();
		}
		else
		{
			secs_inc_flag=0;
		}

		if(!(PINB & (1<<PB5)))
		{
			SECS_DEC();
		}
		else
		{
			secs_dec_flag=0;
		}
	}

}



void INT0_init(void)
{
	MCUCR |=  (1<<ISC01);
	MCUCR &= ~(1<<ISC00);
	DDRD  &= ~(1<<PD2);
	PORTD |=  (1<<PD2);
	GICR  |=  (1<<INT0);
}

void INT1_init(void)
{
	MCUCR |=  (1<<ISC11) | (1<<ISC10);
	DDRD  &= ~(1<<PD3);
	GICR  |=  (1<<INT1);

}

void INT2_init(void)
{
	MCUCSR &= ~(1<<ISC2);
	GICR   |=  (1<<INT2);
	DDRB   &= ~(1<<PB2);
	PORTB  |=  (1<<PB2);
}

void  TIMER1_COMPA_init(void)
{
	TCCR1A = 0;
	TCCR1B = (1<<WGM12) | (1<<CS10) | (1<<CS12);
	OCR1A  = 15624;
	TIMSK |= (1<<OCIE1A);
}

void SEVEN_SEG_init(void)
{
	DDRC  |= 0X0F;
	PORTC &= 0XF0;
	DDRA  |= 0X3F;
}

void DISPLAY_UPDATE(void)
{
	PORTA = (1<<PA0);
	PORTC = (PORTC & 0XF0) | (g_hours2 & 0X0F);
	_delay_ms(5);

	PORTA = (1<<PA1);
	PORTC = (PORTC & 0XF0) | (g_hours1 & 0X0F);
	_delay_ms(5);

	PORTA = (1<<PA2);
	PORTC = (PORTC & 0XF0) | (g_mins2 & 0X0F);
	_delay_ms(5);

	PORTA = (1<<PA3);
	PORTC = (PORTC & 0XF0) | (g_mins1 & 0X0F);
	_delay_ms(5);

	PORTA = (1<<PA4);
	PORTC = (PORTC & 0XF0) | (g_secs2 & 0X0F);
	_delay_ms(5);

	PORTA = (1<<PA5);
	PORTC = (PORTC & 0XF0) | (g_secs1 & 0X0F);
	_delay_ms(5);
}

void TIMER_INC(void)
{
	if (g_incrment_flag == 1)
	{
		// Increment seconds
		if (g_secs1 < 9)
		{
			g_secs1++;
		}
		else
		{
			g_secs1 = 0;
			g_secs2++;
			if (g_secs2 > 5) // 60 seconds reset
			{
				g_secs2 = 0;
				g_mins1++;

				// Increment minutes
				if (g_mins1 > 9)
				{
					g_mins1 = 0;
					g_mins2++;

					if (g_mins2 > 5) // 60 minutes reset
					{
						g_mins2 = 0;
						g_hours1++;


						if (g_hours1 == 10)
						{
							g_hours1 = 0;
							g_hours2++;
						}
						if (g_hours2 == 2 && g_hours1 == 4) // 24:00 reset
						{
							g_hours2 = 0;
							g_hours1 = 0;
						}
					}
				}
			}
		}
		g_incrment_flag = 0; // Reset flag
	}
}

void TIMER_DEC(void)
{
	if (g_decrment_flag == 1)
	{
		if (g_secs1 > 0)
		{
			g_secs1--;
		}
		else
		{
			g_secs1 = 9;
			if (g_secs2 > 0)
			{
				g_secs2--;
			}
			else
			{
				g_secs2 = 5;
				if (g_mins1 > 0)
				{
					g_mins1--;
				}
				else
				{
					g_mins1 = 9;
					if (g_mins2 > 0)
					{
						g_mins2--;
					}
					else
					{
						g_mins2 = 5;
						if (g_hours1 > 0)
						{
							g_hours1--;
						}
						else
						{
							g_hours1 = 9;
							if (g_hours2 > 0)
							{
								g_hours2--;
							}
							else
							{
								g_hours1 = 0;
								g_hours2 = 0;
								g_mins1 = 0;
								g_mins2 = 0;
								g_secs1 = 0;
								g_secs2 = 0;
								g_alarm_flag=1;
							}
						}
					}
				}
			}
		}
		g_decrment_flag = 0;
	}
}


void TIMER_MODE_TOGGEL(void)
{
	if(!(PINB & (1<<PB7)))
	{
		if(g_mode_pb_flag == 0)
		{
			if(g_mode_flag)
			{
				g_mode_flag = 0;
			}
			else
			{
				g_mode_flag = 1;
			}
			g_mode_pb_flag=1;
		}
	}
	else
	{
		g_mode_pb_flag=0;
	}
}

void HOURS_INC(void)
{
	if(hours_inc_flag == 0)
	{
		if(g_hours1 < 9)
		{
			g_hours1++;
		}
		else if(g_hours1 == 9)
		{
			g_hours1=0;
			g_hours2++;
			if(g_hours2 > 9)
			{
				g_secs2=0;
				g_secs1=0;
				g_mins2=0;
				g_mins1=0;
				g_hours2=0;
				g_hours1=0;
			}
		}
		hours_inc_flag=1;
	}
}


void HOURS_DEC(void)
{
	if(hours_dec_flag == 0)
	{
		if(g_hours1 != 0)
		{
			g_hours1--;
		}
		else if(g_hours2 != 0)
		{
			g_hours2--;
			g_hours1=9;
		}
		hours_dec_flag=1;
	}
}



void MINS_INC(void)
{
	if(mins_inc_flag == 0)
	{
		if(g_mins1 < 9)
		{
			g_mins1++;
		}
		else if( g_mins1 == 9)
		{
			g_mins1 = 0;
			g_mins2 ++;
			if(g_mins2 > 5)
			{
				g_mins2=0;
				g_mins1=0;
			}
		}
		mins_inc_flag = 1;
	}
}

void MINS_DEC(void)
{
	if(mins_dec_flag == 0)
	{
		if(g_mins1 != 0)
		{
			g_mins1--;
		}
		else if(g_mins2 != 0)
		{
			g_mins1=9;
			g_mins2--;
		}
		mins_dec_flag=1;
	}
}

void SECS_INC(void)
{
	if(secs_inc_flag == 0)
	{
		if(g_secs1 < 9)
		{
			g_secs1++;
		}
		else if( g_secs1 == 9)
		{
			g_secs1 = 0;
			g_secs2 ++;
			if(g_secs2 > 5)
			{
				g_secs2=0;
				g_secs1=0;
			}
		}
		secs_inc_flag = 1;
	}
}


void SECS_DEC(void)
{
	if(secs_dec_flag == 0)
	{
		if(g_secs1 != 0)
		{
			g_secs1--;
		}
		else if(g_secs2 != 0)
		{
			g_secs1=9;
			g_secs2--;
		}
		secs_dec_flag=1;
	}
}






