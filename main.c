#include <stc15.h>

#define bool bit
#define false (0)
#define true (!false)
#define SET_BIT(p,n) ((p) |= (1 << n))
#define INV_BIT(p,n) ((p) ^= (1 << n))
#define CLR_BIT(p,n) ((p) &= ~(1 << n))

#define red_led P30
#define green_led P31
#define ps_on P32
#define reset P54
//#define ps_on P54
//#define reset P32
#define sc_det P33
#define relay P55
#define ON 0xFF
#define OFF 0x00

#define sc_det_delay 1500 // Wait 100ms for 5v is up

void delay_ms(unsigned int ms);
void halt();
void relayON();
void relayOFF();
void blink_green();
void check_sc();

unsigned int i, j;
unsigned int reset_delay;
bool relay_on;
bool sc_det_delayed;

void main(void)
{
	//IT1 = 0;
	//EX1 = 0;
	//EA = 1;

  //Set P30 as push-pull output RED
	CLR_BIT(P3M1,0);
 	SET_BIT(P3M0,0);
  //Set P31 as push-pull output GREEN
	CLR_BIT(P3M1,1);
	SET_BIT(P3M0,1);
  //Set P32 as high impedance input-only ps_on
	SET_BIT(P3M1,2);
	CLR_BIT(P3M0,2);
  //Set P33 as high impedance input-only sc_det
	SET_BIT(P3M1,3);
	CLR_BIT(P3M0,3);
  //Set P54 as high impedance input-only reset
	SET_BIT(P5M1,4);
	CLR_BIT(P5M0,4);
  //Set P55 as push-pull output RELAY
	CLR_BIT(P5M1,5);
	SET_BIT(P5M0,5);

/* For signal testing
while(1)
	{
		if (ps_on) blink_green();
		if (sc_det) blink_green();
		if (reset) blink_green();
	}
//*/
	
  green_led = OFF;
	red_led = OFF;
	relay = OFF;
	relay_on = false;
	sc_det_delayed = false;
	delay_ms(500);
	
	while(1)
	{
		if (reset) {
			if(relay_on){
				reset_delay = 10 * 5; // 10 sec, depends of blink_green delay
				while(reset && reset_delay){
					blink_green();
					reset_delay--;
					if (ps_on) reset_delay = 0;
				}
				if(reset) relayOFF();
			}
		}
		else if (ps_on){
			if(!relay_on) relayON();
			else{
				reset_delay = 2 * 5; // 2 sec, depends of blink_green delay
				while(ps_on && reset_delay){
					blink_green();
					reset_delay--;
				}
				if (ps_on) relayOFF();
			}
		}
		if(reset || ps_on){
			green_led = ON;
			while(reset || ps_on) check_sc();
			green_led = OFF;
		}
		delay_ms(100);
		check_sc();
	}
}
	
void delay_ms(unsigned int ms) {
	unsigned int i, j;
	for (i = 0; i < ms; i++) {
		for (j = 0; j < 1090; j++);
	}
}

void relayON(){
	relay = ON;
	red_led = ON;
	sc_det_delayed = false;
	relay_on = true;
}

void relayOFF(){
	relay = OFF;
	red_led = OFF;
	relay_on = false;
}

void blink_green(){
	check_sc();
	green_led = ON;
	delay_ms(100);
	check_sc();
	green_led = OFF;
	delay_ms(100);
}

void halt(){
	relay = OFF;
	green_led = OFF;
	while(1){
		red_led = ON;
		delay_ms(50);
		red_led = OFF;
		delay_ms(50);
	}
}

void check_sc(){
	if(relay_on) {
		if (!sc_det_delayed){
			delay_ms(sc_det_delay);
			sc_det_delayed = true;
		}
    if (!sc_det) halt();
	}
}

//void exint1() interrupt 2 {
//  halt();
//}