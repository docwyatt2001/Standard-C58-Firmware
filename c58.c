#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include "include/avrlibdefs.h"
#include "include/avrlibtypes.h"
#include "include/c58.h"
#include "include/delay_basic.h"
volatile unsigned int milliseconds;
char mode,busy_state,ax;
char mem_state;
char f_changed;
char rpt,vfo;
unsigned int f_step;
// shift;
unsigned long  freq,freq_bkup,freq2;

//******* memory
char	 		  autore[]  EEMEM="IW3IIF";
 unsigned long	   mem1	EEMEM=51000;
 unsigned long	   mem2	EEMEM=52000;
 unsigned long	   mem3	EEMEM=53000;
 unsigned long	   mem4	EEMEM=54000;
 unsigned long	   mem5	EEMEM=55000;
 unsigned long	   vfo_mem	EEMEM=55000;

void lcd ( unsigned long fu);
void pll ( unsigned long fu );
void scan_memory(void);
unsigned long get_mem(char num);
void sc_all(void);
void beep(void);


void beep(void)
{
timer_on;
milliseconds=0;
sbi(PORTD,5);
while(milliseconds<100);
cbi(PORTD,5);
timer_off;
}




ISR(TIMER1_COMPA_vect)
{
milliseconds=milliseconds+1;
}


void sc_all(void)
{
unsigned long  freq1,delta;


freq=freq/10000;
freq1=freq*10000; delta=0;													// leaving only the MHz

timer_on;
while(1)
	{
	if((delta+f_step)<10000){delta=delta+f_step;freq=freq1+delta;}
	else {freq=freq1; delta=0;}
	lcd(freq);pll(freq);
	milliseconds=0;
	while((milliseconds<200));
		{
		cbi(PORTD,1);														// remove control from col. 2 - ccl
		sbi(PORTD,0);_delay_loop_1(30);										// checks busy
		if(((PINA & busy)!=0) & (mode == mode_fm))							// just busy fm channel
		{
			cbi(PORTD,0);													// removes busy Control
			sbi(PORTD,1);_delay_loop_1(30);									// controls ccl second column
			milliseconds=0;													// reset timer and stay listening for 3 seconds
			while((milliseconds<2000) & ((PINB&ccl)==0));					// but it ends around if I press ccl
			milliseconds=0;
			if((PINB & ccl)!=0)
				{beep();cbi(PORTD,1);timer_off;return;}

			}
		cbi(PORTD,0);														// removes busy Control
		sbi(PORTD,1);_delay_loop_1(30);										// controls ccl second column
		if((PINB & ccl)!=0)
				{beep();cbi(PORTD,1);timer_off;return;}
		}
	}

}


void scan_memory(void)
{
timer_on;
while(1)
	{
	if(mem_state<5)mem_state++;
	else mem_state=1;
	freq=get_mem(mem_state);												// select a memory
	lcd(freq);pll(freq);
	milliseconds=0;
	while((milliseconds<1000));												// 1 second then switch Memory
		{
		cbi(PORTD,1);														// remove control from the col. 2 - rcl button
		sbi(PORTD,0);_delay_loop_1(30);										// checks busy
		if((PINA & busy)!=0)												// busy channel
		{
			cbi(PORTD,0);													// removes busy Control
			sbi(PORTD,1);_delay_loop_1(30);									// controls ccl second column
			milliseconds=0;													// reset timer and stay listening 3 seconds
			while((milliseconds<3000) & ((PINB & ccl)==0));					// but it ends around if you press ccl
			milliseconds=0;
			if((PINB & ccl)!=0)												// ccl check if the timer had not finished
				{cbi(PORTD,1);timer_off;mem_state=0;return;}

			}
		cbi(PORTD,0);														// removes busy Control
		sbi(PORTD,1);_delay_loop_1(30);										// controls ccl second column
		if((PINB & ccl)!=0)
				{cbi(PORTD,1);timer_off;mem_state=0;return;}
		}
	}
}

 unsigned long get_mem(char num)
{
 unsigned long ff=0;
switch(num)
	{
	case 1:ff=eeprom_read_dword(&mem1);break;
	case 2:ff=eeprom_read_dword(&mem2);break;
	case 3:ff=eeprom_read_dword(&mem3);break;
	case 4:ff=eeprom_read_dword(&mem4);break;
	case 5:ff=eeprom_read_dword(&mem5);break;
	case 6:ff=eeprom_read_dword(&vfo_mem);break;
	}
return(ff);
}

void set_mem( unsigned long fw,char num)
{
switch(num)
	{
	case 1:eeprom_write_dword(&mem1,fw);break;
	case 2:eeprom_write_dword(&mem2,fw);break;
	case 3:eeprom_write_dword(&mem3,fw);break;
	case 4:eeprom_write_dword(&mem4,fw);break;
	case 5:eeprom_write_dword(&mem5,fw);break;
	case 6:eeprom_write_dword(&vfo_mem,fw);break;
	}
}

void memory(void)												            // Was RCL pressed
{
	freq_bkup=freq;															// It will be stored if you press ent
																			// everything until I press ccl

 while(1)
  {
	if((PINB & rcl)!=0)
			{
			beep();
			if(mem_state<5)mem_state++;
			else mem_state=1;
			freq=get_mem(mem_state);										// select a memory
			lcd(freq);pll(freq);
			while ((PINB & rcl)!=0);										// releases rcl
			}
	if((PINB & ent)!=0)														// checks ent
			{
			set_mem(freq_bkup,mem_state);									// if pressed, we save freq. VFO
			beep();
			while ((PINB & ent)!=0);										// releases
			}
																			// controls ccl
		cbi(PORTD,2);														// rcl column off
		sbi(PORTD,1);
// ***********  column for ccl
		_delay_loop_1(30);
		if((PINB & ccl)!=0)
			{
			freq=freq_bkup;f_changed=yes;									// recovers VFO
			mem_state=0;													// restores VFO
			cbi(PORTD,1);beep();
			return;
			}
		cbi(PORTD,1);
		sbi(PORTD,2);
// ***********  column for mem
		_delay_loop_1(30);
		}

	}




void lcd ( unsigned long fu)
{
char display[5];char *dummy;
char d1,d2,d3,d4;
dummy=ultoa(fu,display,10);													//prepare ascii string
d4=0;d3=0;d2=0;d1=0;
switch(mem_state)
	{
	case 1:d4=4;break;
	case 2:d3=4;break;
	case 4:d2=0x0b;break;
	case 3:d2=4;break;
	case 5:d1=4;break;
	}

sbi(PORTC,7);																//ce=1
// ******************************* groups digits eg. 49375 = 144.937.5
if(f_step!=1)
{
	PORTC=0x83|d1;															// No point and we active digit 01234
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[3]-'0')|128;												// parts from 3 except with 100Hz step
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x83|d2;															// no point there and active
	sbi(PORTC,6);cbi(PORTC,6);												// strobe third
	PORTC=(display[2]-'0')|128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x83|d3;															// no point, and there activates the second
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[1]-'0')|128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x8b|d4;															//  point to the right and we turned on before
																			// 0x8f = active 1 M
																			//
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[0]-'0')|128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x82;																//  ??
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	if(mem_state!=0)PORTC=1|128;											// M
	else PORTC=128;

	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=0x82;																//  ??
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
}
else																		// **********	solo step 100 Hz
{
	PORTC=0x83|d1;															// No point and we active digit 01234
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[4]-'0')|128;												//
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x83|d2;															// point and we active
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[3]-'0')|128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x83|d3;															// no point there and active
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[2]-'0')|128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x83|d4;															// no point there and active
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=(display[1]-'0')|128;												// last shown hundreds khz
	sbi(PORTC,6);cbi(PORTC,6);												// strobe

	PORTC=0x82;																//  ??
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	if(mem_state!=0)PORTC=1|128;											// M
	else PORTC=128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=0x82;																//  ??
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
	PORTC=128;
	sbi(PORTC,6);cbi(PORTC,6);												// strobe
}

cbi(PORTC,7);																// there down

}

void pll ( unsigned long fu )
{
#define vxoA 8800
#define vxoB 28800
 unsigned int N,N2;
 unsigned int vxo;
unsigned short dac_hi,dac_lo;
if(fu>=60000)
	{
	vxo=vxoB;
	sbi(PORTD,7);cbi(PORTD,6);												// select VXO for> = 146MHz
	}
else
	{
	vxo=vxoA;
	sbi(PORTD,6);cbi(PORTD,7);												// vxo per 144-145.999.9
	}
																			// calculates PLL and DAC data
N=(fu-vxo)/100;																// a 9 bit
N2=fu-vxo-100*N;
N=N & 0xff;																	// bit8 already active
dac_hi=N2/10;dac_lo=N2-dac_hi*10;											// dac in BCD!!

//****** Send the data for the PLL

sbi(PORTA,2);cbi(PORTA,1);sbi(PORTA,0);										// 5 s2s2s0 101
PORTC=N>>4;																	// high nibble
sbi(PORTC,5);cbi(PORTC,5);													// strobe

cbi(PORTA,0);																// 4 s2s1s0 100
PORTC=N & 0x0f;																// low nibble
sbi(PORTC,5);cbi(PORTC,5);													// strobe

// ********* Send the data to the DAC

sbi(PORTA,0);sbi(PORTA,1);													// 7 s2s1s0 111
PORTC=dac_hi;																// BCD nibble top
sbi(PORTC,5);cbi(PORTC,5);													// strobe

cbi(PORTA,0);																// 6 s2s1s0 110
PORTC=dac_lo;																// BCD nibble down
sbi(PORTC,5);cbi(PORTC,5);													// strobe


}

void scankey(void)
{

	//unsigned short _MHZ_=5;

	// ****** test encoder
	sbi(PORTD,0);																// *************  primary column
	_delay_loop_1(30);
	
	if((PINB & freq_tuning) != 0)												// Test for frequency up/down from the four inputs
	{																			// Assumption: Only one action at any one instance
		unsigned short _timeout_ = 7;
	
		f_changed = yes;
		if((PINB & down) != 0 || (PINB & mic_down) != 0)
		{
			freq = freq - f_step;
			if(freq < 40000) freq += 40000;										// If it goes below the 144.000Mhz, wrap it to 148Mhz minus f_step
		}
		else 
		{
			freq = freq + f_step;
			if(freq > 79999) freq = 40000;										// If goes over 147.999Mhz, reset it 144.000Mhz
		}
		
		if(PINB & mic_pressed) 													// Double the duration of the wait if using mic keys
		{
			 _timeout_ = 14;
		}

		while((PINB & freq_tuning) != 0);										// Wait for the pulse end on any input
		timer_on;
		milliseconds = 0;
		while(milliseconds < _timeout_);
		timer_off;
		cbi(PORTD,0);

		return;																	// Skip testing the other lines
	}

/*	
	if((PINB&down)!=0)
	{
		freq=freq-f_step;f_changed=yes;
		while((PINB&down)!=0);													// wait for the pulse end
		timer_on;
		milliseconds=0;
		while(milliseconds<7);
		timer_off;
		cbi(PORTD,0);
		return;																	// not test other lines
	}
	if((PINB&up)!=0)
	{
		freq=freq+f_step;f_changed=yes;
		while((PINB&up)!=0);													// wait for the pulse end
		timer_on;
		milliseconds=0;
		while(milliseconds<7); 
		timer_off;
		cbi(PORTD,0);
		return;																	// not test other lines
	}

	//if((PINB&mic_up)!=0)														// Mic channel up
	//if((PINB&mic_down)!=0)													// Mic channel down
										
	// must check Mhz digit (144.000 Mhz - 147.999 Mhz) for rollover after these 4 routines
*/	

	//if((PINB&busy)!=0)busy_state=1;											// busy test for automatic scanning
	cbi(PORTD,0);

	sbi(PORTD,1);																// ***********  second column
	_delay_loop_1(30);
	if((PINA & simplex)!=0)rpt=no;												//simplex  
	else
	{
		if((PINA & duplex)!=0)rpt=plus;											// preparing for the shift R2 TX FM
		else rpt=minus;
	}

	unsigned short _MHZ_;
	
	if((PINB & mhz)!=0)
	{
		f_changed=yes;
		_MHZ_=freq/10000;														// check Mhz digit (144Mhz - 147Mhz)
		if(_MHZ_!=7)freq=freq+10000;
		else freq=freq-30000;
		beep();
		while((PINB & mhz)!=0);													// wait release
	}

	if((PINB & scan_memo)!=0) {beep();scan_memory();}
	if((PINB & scan_all)!=0) {beep();sc_all();}
	//if((PINB&ccl)!=0) while ((PINB&ccl)!=0);

	if((PINB & ccl)!=0)
	{
		freq_bkup=freq;freq=freq2;freq2=freq_bkup;								//exchanges
		f_changed=yes;
		beep();
		while((PINB & ccl)!=0);													// wait release
		//cbi(PORTD,1);return;
	}



	cbi(PORTD,1);

	sbi(PORTD,3);																// fourth column of the first 3 to control the FM sw
	_delay_loop_1(30);
	if((PINA & fm)!=0)															// dial fm
	{
		if(mode==mode_ssb)														// so changed, was ssb
		{

			cbi(PORTD,4);														// NB off
			mode=mode_fm;														// determines the steps
			freq=freq/125;
			freq=freq*125;
			f_step=125;
			f_changed=yes;
		}																		// otherwise it was already fm, do nothing
	}
	else																		// select ssb
	{
		if(mode==mode_fm)														// changed, was fm
		{
			 mode=mode_ssb;
			 freq=freq/10;
			 freq=freq*10;
			 f_step=10;
			 f_changed=yes;
		 }
	}
	cbi(PORTD,3);

	sbi(PORTD,2);																// third column
	_delay_loop_1(30);

	if((PINB & nb)!=0)
	{
		if(mode!=mode_fm)														// NB only in SSB
		{
			beep();
			if((PIND & 16)==0)sbi(PORTD,4);
			else cbi(PORTD,4);
			while ((PINB & nb)!=0);
			//cbi(PORTD,2);return;
		}
	}

	if((PINB & ent)!=0)															// ent checks out of the loop memories
	{
		set_mem(freq,6);														// if I press, we save freq. VFO will resume at power on
		beep();
		while ((PINB & ent)!=0);												// releases
		//cbi(PORTD,2);return;
	}

	if((PINB & rcl)!=0) memory();
	if((PINB & step)!=0)
	{
		beep();
		if(mode==mode_fm)
		{
			if(f_step==125)f_step=250;											//12.5 or 25 FM
			else f_step=125;
		}
		else
		{
			if(f_step==1)f_step=10;												// 100 hz,1 khz or 5 khz in ssb/cw
			else if(f_step==10)f_step=50;
			else if(f_step==50)f_step=1;
		}
		while((PINB & step)!=0);												// wait release
	}

	cbi(PORTD,2);

}

//***************************************************//

int main(void)
{
// *************************  variables

//**************************  configures pin
DDRB=0;																		// keyboard input
DDRD=0xff;																	// Low: keyboard output; High: NB, BUZZ, VXOA, VXOB
DDRC=0xff;																	// Low: lcd data and tms 1024 data; high (free PC4), tms 1024 STB, STB lcd, lcd EN
DDRE=0;																		// free with PE0-INT2 encoder future
DDRA=7;																		// out: s0,s1,s2 tms 1024; in +TXsense, bkup switch

OCR1AH=0;OCR1AL=0x7d;														// so CTC prescaler 8
TCCR1A=0;TCCR1B=0x0A;

																			//	GICR = 64; // enable int0
sei();
rpt=no;

freq=get_mem(6);															//recalls VFO memory
freq2=freq;																	//equalizes the two VFO
vfo=A;
f_changed=yes;mem_state=0;

while(1)
{
	scankey();
	if(f_changed==yes)
	{
			pll(freq);
			f_changed=no;
	}
	
	lcd(freq);

	if((PINA & tx_sense)!=0)													// activates TX
		{
		freq_bkup=freq;
		if(mode==mode_fm)
			{
			if(rpt==plus)freq=freq+6000;
			else if (rpt==minus)freq=freq-6000;
			}
		pll(freq);															// display does not change
		lcd(freq);
		while((PINA & tx_sense)!=0);											// wait to pass in rx
		freq=freq_bkup;														// restores f reception
		f_changed=yes;
		}


}
return 0;
}





