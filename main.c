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
#include "Lab_6_pwm.h"

void initialize();
void en_interrupts();
void delay();

int main(void)
{
	// Initialize UART and PWM
	initialize();

	// Print welcome over serial
	put("Running... \n\r");
	
  int forward = 1;
  int phase = 0;
  int count = 0;
  
  while (1)
  {
    // Turn off all coils, Set GPIO pins to 0
    GPIOD_PCOR = (1UL << 0) | (1UL << 1) | (1UL << 2) | (1UL << 3);
    
    // Set one pin high at a time
    if (forward)
    {
      if(phase == 0)
      {
        GPIOD_PSOR = (1UL << 0);
        phase++;
      }
      else if(phase == 1)
      {
        GPIOD_PSOR = (1UL << 1);
        phase++;
      }
      else if(phase == 2)
      {
        GPIOD_PSOR = (1UL << 2);
        phase++;
      }
      else
      {
        GPIOD_PSOR = (1UL << 3);
        phase = 0;
        count++;
      }
    }
    else // reverse
    {
      if (phase == 0)
      {
        GPIOD_PSOR = (1UL << 3);
        phase++;
      }
      else if(phase == 1)
      {
        GPIOD_PSOR = (1UL << 2);
        phase++;
      }
      else if(phase == 2)
      {
        GPIOD_PSOR = (1UL << 1);
        phase++;
      }
      else
      {
        GPIOD_PSOR = (1UL << 0);
        phase = 0;
        count++;
      }
    }
    
    if(count == 100)
    {
      forward ^= 1;
      count = 0;
    }
    delay(10);
  }
}

/**
 * Waits for a delay (in milliseconds)
 * 
 * del - The delay in milliseconds
 */
void delay(int del){
	int i;
	for (i=0; i<del*5000; i++){
		// Do nothing
	}
}

void initialize()
{
	// Initialize UART
	uart_init();	
	
	// Initialize the FlexTimer
	InitPWM();
  
  // Enable clocks on Port D
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
  
  // Configure the Signal Multiplexer for the Port D GPIO Pins
  PORTD_PCR0 = PORT_PCR_MUX(1);
  PORTD_PCR1 = PORT_PCR_MUX(1);
  PORTD_PCR2 = PORT_PCR_MUX(1);
  PORTD_PCR3 = PORT_PCR_MUX(1);
  
  // Configure the GPIO Pins for Output
  GPIOD_PDDR = (1UL << 0) | (1UL << 1) | (1UL << 2) | (1UL << 3);
}
