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
#include <math.h>

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#ifndef DEFAULT_SYSTEM_CLOCK
#define DEFAULT_SYSTEM_CLOCK 20485760u
#endif

#define THRESHOLD 10833
#define MIN_SPEED 30.0
#define A 30.0
#define U 61.0
#define SIGMA 15.0
#define OFFSET 1.0
#define PI 3.14159

void initialize(void);
void en_interrupts(void);
void delay(int del);

int main(void)
{ 
  uint16_t freq = 10000; /* Frequency = 10 kHz */
  uint16_t cam_data[128];
  double speed = 0;
  double duty = 0;
  double old_duty = 0;
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
		
		for (int i = 0; i<123; i++)
		{
			if(cam_data[i])
      {
        duty += ((i-183)*i+7442)*i;
        speed += 6148.0 - 1.6522*(i-61)*(i-61);
        //speed += 12296.0-3.305*(i-61)*(i-61);
//        if(i <= U-OFFSET)
//          speed += A/(sqrt(2*PI)*SIGMA)*exp(-pow((i-U+OFFSET)/SIGMA, 2)/2.0) + MIN_SPEED/123;
//        else if(i >= U+OFFSET)
//          speed += A/(sqrt(2*PI)*SIGMA)*exp(-pow((i-U-OFFSET)/SIGMA, 2)/2.0) + MIN_SPEED/123;
//        else
//          speed += A/(sqrt(2*PI)*SIGMA) + MIN_SPEED/123;
      }
		}
    
    SetDutyCycle(speed/10000.0+20.0, freq, 1, 2);
    //SetDutyCycle(speed, freq, 1, 2);
    
//    sprintf(string, "\n\rduty %f speed %f\n\r", duty, speed/10000.0+20.0);
//    put(string);

    //duty = (duty + old_duty)*0.5;
    SetDutyCycleServo((duty)/1538104.0+7.75);
    
    old_duty = duty;
    duty = 0;
    speed = 0;
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
