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
  double duty = 0;
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
			if(cam_data[i])
      {
        duty += -9.0*i + 549.0;
      }
		}
    
    sprintf(string, "\n\rduty %f\n\r", duty/7564.0+7.75);
    put(string);
    SetDutyCycleServo(duty/7564.0+7.75);
    
    duty = 0;
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
