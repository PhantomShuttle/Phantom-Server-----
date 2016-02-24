
/*
 * Server.c
 *
 * Created: 2016/2/23 20:43:49
 *  Author: x
 */ 


# define F_CPU 16000000UL//cpu????????????????????—¨????
#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <math.h>
#include "Oled.h"
#define Pk 2
#define Ik 1
#define Dk 2.1
#define Dk 2.1
#define Blank 2
#define D0_L	PORTA &= ~(1 << PA4)/*µÍµçÆ½*/
#define D0_H	PORTA |= (1 << PA4)/*??????*/
#define D1_L	PORTA &= ~(1 << PA5)/*??????*/
#define D1_H	PORTA |= (1 << PA5)/*??????*/
int AD[8],ch[8],ch_last[8],ff,co=0;;
float P,I,D,PID;
void port_init(void)
{   DDRA = 0xf0;        //0-3????
	PORTA= 0x00;
	DDRB = 0xbf;        //pb4 0 miso ????11110111
	PORTB= 0x0f;
	DDRC = 0xff; 		   //PC????
	PORTC= 0xff; 		   //????? ???????
	DDRD = 0xF7;              //???3??????????
	PORTD=0x00;
}
/********AD????????********/
void adc_init(void)
{ ADCSRA = 0x00;  //ADC????????????0
	ADMUX = 0x44; //AVCC??¦Ï????,????AD4
	ACSR=1<<ACD;//??????????
	ADCSRA = 0x83;  // ???ADC,????,8???
}

/********AD???********/
unsigned int adc_read(unsigned char ADCChannel)
{ unsigned int data,h,l;unsigned char i;
	ADMUX = (ADMUX & 0xf0)+(ADCChannel & 0x0f);
	data = 0;
	for (i=0;i<4;i++)                        //??????
	{     ADCSRA|=0x40;                   //??????
		while ((ADCSRA&0x10)==0);       //???????
		ADCSRA|=0x10;                   //????¦Ë
		l=ADCL;                         //??¦Ë????
		h=ADCH;                         //??¦Ë????
		h<<=8;                          //???????
		h|=l;                           //
		data+=h;                        //???????
	}
	data/=4;                              //??????
	return data;
}
/********?????1?????********/
void timer0_init(void)
{   SREG = 0x80;          // ???????§Ø?
	
	TCCR0=0x03;
	TCNT0=0X00;
	OCR0=0X70;
	TIMSK|=0x03;        //????§Ø????
}



int main(void)
{   port_init();
	OledInit ();
	timer0_init();
	adc_init()   ;              //A/D????? 
    while(1)
    { ch[0]=((adc_read(0) ))*0.8+ch[0]*0.2;
		ch[1]=((adc_read(1) ))*0.8+ch[1]*0.2;
		
		P=(ch[0]-ch[1])*Pk;
		
		if (ch[0]>ch[1]+Blank)
		{I+=Ik;	}
		else if(ch[0]<ch[1]-Blank)
		{I-=Ik;	}
			
		D=(ch[0]-ch_last[0])*Dk;
		ch_last[0]=ch[0];
		
			
		PID=P+D+I;
		if (PID<0)
		{PID=-PID;}
		
		if (PID>0xfe)
		{PID=0xfe;
		}
		if ((PID<15))
		{co++;
			if (co>200)
			{co=200;
			}
		}
		else{co=0;}
		
		if (co>10)
		{TCCR0=0x00;
			D0_L;
			D1_L;
		}
		else{
			TCCR0=0x03;
			OCR0=0xff-PID;
		}
		OCR0=0xff-PID;
	
		Cache_MDigit5_int( ch[0],0,0,1);Cache_MDigit5_int(ch[1],0,50,1);
		Cache_MDigit5_int(P,4,0,1);Cache_MDigit5_int(I,4,50,1);
		Cache_MDigit5_int(D,6,0,1);
		
        //TODO:: Please write your application code 
    }
}
ISR(TIMER0_OVF_vect)//????§Ø? T1
{TCNT0=0X00;
D0_L;
D1_L;
}
	ISR(TIMER0_COMP_vect)//????§Ø? T1
	{if (ch[0]>ch[1])
	{   D0_L;
		D1_H;
		ff=0;	
	}
	else{
		 D0_H;
		 D1_L;
		
		ff=5;}
	}
