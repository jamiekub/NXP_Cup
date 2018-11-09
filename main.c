/*
 * Main Method for testing the PWM Code for the K64F
 * PWM signal can be connected to output pins are PC3 and PC4
 * 
 * Author:  
 * Created:  
 * Modified:  
 */
#include "stdio.h"
#include "MK64F12.h"
#include "uart.h"
#include "PWM.h"
#include "camera_FTM.h"
#include "isr.h"
#include <string.h>

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#ifndef DEFAULT_SYSTEM_CLOCK
#define DEFAULT_SYSTEM_CLOCK 20485760u
#endif

#define THRESHOLD 10833

void initialize(void);
void en_interrupts(void);
void delay(int del);

int main(void)
{ 
  uint16_t freq = 10000; /* Frequency = 10 kHz */
  uint16_t cam_data[128];
  double firstZeros = 0;
	int ones = 0;
	double secondZeros = 0;
  double ratio = 1;
  char string[100];
  
	// Initialize UART and PWM
	initialize();
	
	//Step 3
	//Generate 20% duty cycle at 10kHz
	//SetDutyCycle(20,10000,1);
	//SetDutyCycle(20,10000,0);
	//for(;;) ;  //then loop forever
	
	//Step 9
	for(;;)  //loop forever
	{ 
    memcpy(cam_data, (void*)getLine(), sizeof(cam_data));
    filter(cam_data);
		
	  SetDutyCycle(30, freq, 1, 2);
		
		for (int i = 0; i<123; i++)
		{
			if(cam_data[i] == 0 && ones == 0)
			{
				firstZeros += 1;
			}
			else if(cam_data[i])
			{
				ones += 1;
			}
			else
			{
				secondZeros += 1;
			}	
		}
    
		//If > 1.1 turns left, if < .9 turns right
		ratio = firstZeros/secondZeros;
    
    //sprintf(string, "\n\rfirst0 %f second0 %f ratio %f\n\r", firstZeros, secondZeros, ratio);
    //put(string);
    
		if(ratio < 5.0 && ratio > 1.0)
		{
			SetDutyCycleServo(6.0); //left
		}
//		else if(ratio > 1.3)
//		{
//			SetDutyCycleServo(5.5);
//		}
//		else if(ratio < 0.9 && ratio > 0.7)
//		{
//			SetDutyCycleServo(8.5);
//		}
		else if(ratio < 0.4)
		{
			SetDutyCycleServo(9.0); //right
		}
    else
    {
      SetDutyCycleServo(7.6);
    }
    
    ones = 0;
    firstZeros = 0;
    secondZeros = 0;
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
  InitFTM3(); // To generate CLK, SI, and trigger ADC
  InitADC0();
  InitPIT();	// To trigger camera read based on integration time
  // Initialize the FlexTimer
  InitPWM();
}
