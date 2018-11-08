/*
 * Main Method for testing the PWM Code for the K64F
 * PWM signal can be connected to output pins are PC3 and PC4
 * 
 * Author:  
 * Created:  
 * Modified:  
 */

#include "MK64F12.h"
#include "uart.h"
#include "PWM.h"

void initialize();
void en_interrupts();
void delay();

int main(void)
{
	// Initialize UART and PWM
	initialize();

	// Print welcome over serial
	put("Running... \n\r");
	
	//Step 3
	//Generate 20% duty cycle at 10kHz
	//SetDutyCycle(20,10000,1);
	//SetDutyCycle(20,10000,0);
	//for(;;) ;  //then loop forever
	
	//Step 9
	for(;;)  //loop forever
	{
		uint16_t dc = 0;
		uint16_t freq = 10000; /* Frequency = 10 kHz */
		uint16_t dir = 0;
		char c = 48;
		int i=0;
		
	  SetDutyCycle(50, freq, 1);
		SetDutyCycleServo(5);
		delay(100);
		SetDutyCycle(0, freq, 1);
		SetDutyCycleServo(10);
		delay(100);
		SetDutyCycle(50, freq, 0);
		delay(100);

	}
	return 0;
}


/**
 * Waits for a delay (in milliseconds)
 * 
 * del - The delay in milliseconds
 */
void delay(int del){
	int i;
	for (i=0; i<del*50000; i++){
		// Do nothing
	}
}

void initialize()
{
	// Initialize UART
	uart_init();	
	
	// Initialize the FlexTimer
	InitPWM();
}
