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
#include "camera_FTM.h"
#include "isr.h"

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#ifndef DEFAULT_SYSTEM_CLOCK
#define DEFAULT_SYSTEM_CLOCK 20485760u
#endif

void initialize(void);
void en_interrupts(void);
void delay(int del);

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
		uint16_t freq = 10000; /* Frequency = 10 kHz */
		
	  SetDutyCycle(50, freq, 1, 1);
		SetDutyCycleServo(5);
		delay(100);
		SetDutyCycle(0, freq, 1, 1);
		SetDutyCycleServo(10);
		delay(100);
		SetDutyCycle(50, freq, 0, 1);
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
  InitGPIO(); // For CLK and SI output on GPIO
  InitFTM2(); // To generate CLK, SI, and trigger ADC
  InitADC0();
  InitPIT();	// To trigger camera read based on integration time
  // Initialize the FlexTimer
  InitPWM();
}
