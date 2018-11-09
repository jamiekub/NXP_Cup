/*
 * Freescale Cup linescan camera code
 *
 *	This method of capturing data from the line
 *	scan cameras uses a flex timer module, periodic
 *	interrupt timer, an ADC, and some GPIOs.
 *	CLK and SI are driven with GPIO because the FTM2
 *	module used doesn't have any output pins on the
 * 	development board. The PIT timer is used to 
 *  control the integration period. When it overflows
 * 	it enables interrupts from the FTM2 module and then
 *	the FTM2 and ADC are active for 128 clock cycles to
 *	generate the camera signals and read the camera 
 *  output.
 *
 *	PTB8			- camera CLK
 *	PTB23 		- camera SI
 *  ADC0_DP1 	- camera AOut
 *
 * Author:  Kubeck / Manahan
 * Created:  11/20/15
 * Modified:  11/23/15
 */

#include "MK64F12.h"
#include "camera_FTM.h"
#include "uart.h"
#include "stdio.h"
#include "isr.h"

// Default System clock value
// period = 1/20485760  = 4.8814395e-8
#define DEFAULT_SYSTEM_CLOCK 20485760u 

// Integration time (seconds)
// Determines how high the camera values are
// Don't exceed 100ms or the caps will saturate
// Must be above 1.25 ms based on camera clk 
//	(camera clk is the mod value set in FTM2)
#define INTEGRATION_TIME .0075f

//int camera(void)
//{
//	int i;
//	
//	for(;;) {

//		if (debugcamdata) {
//			// Every 2 seconds
//			//if (capcnt >= (2/INTEGRATION_TIME)) {
//			if (capcnt >= (500)) {
//				GPIOB_PCOR = (1 << 22);
//				// send the array over uart
//				sprintf(str,"%i\n\r",-1); // start value
//				put(str);
//				for (i = 0; i < 127; i++) {
//					sprintf(str,"%i\n", line[i]);
//					put(str);
//				}
//				sprintf(str,"%i\n\r",-2); // end value
//				put(str);
//				capcnt = 0;
//				GPIOB_PSOR |= (1 << 22);
//			}
//		}

//	} //for
//} //main

int debugdata = 0;
char string [100];

void filter(uint16_t* cam_data)
{  
  double maxval = 0.0;
  
   if (debugdata == 2) {
			// Every 2 seconds
		  // send the array over uart
			sprintf(string,"%i\n\r",-1); // start value
			put(string);
			for (int i = 0; i < 127; i++) {
				sprintf(string,"%i\n", cam_data[i]);
				put(string);
			}
				sprintf(string,"%i\n\r",-2); // end value
				put(string);
		}
  
    if (debugdata)
    {
      put("\n\r");
      for (int i = 0; i < 128; i++) {
  	  	sprintf(string," %i ", cam_data[i]);
		  	put(string);
		  }
      put("\n\r");    
    }
   
   for (int i = 0; i<123; i++)
   {
     cam_data[i] = cam_data[i]/5.0 + cam_data[i+1]/5.0 + cam_data[i+2]/5.0 + cam_data[i+3]/5.0 + cam_data[i+4]/5.0;
     if (cam_data[i] > maxval)
       maxval = cam_data[i];
   }

   if (debugdata)
   {
     put("\n\r");
     for (int i = 0; i < 123; i++) {
  	 	sprintf(string," %i ", cam_data[i]);
		 	put(string);
		 }
     put("\n\r");    
   }
    
    for (int i = 0; i<123; i++)
    {
      if (cam_data[i] >= 0.7*maxval)
      {
        cam_data[i] = 1;
      }
      else
      {
        cam_data[i] = 0;
      }
    }
    
    if (debugdata)
    {
      put("\n\r");
      for (int i = 0; i < 123; i++) {
  	  	sprintf(string,"%i", cam_data[i]);
			  put(string);
		  }
      put("\n\r");
    }
  }

/* Initialization of FTM2 for camera */
void InitFTM3(){
	// Enable clock
	SIM_SCGC3 |= SIM_SCGC3_FTM3_MASK;

	// Disable Write Protection
	FTM3_MODE |= FTM_MODE_WPDIS_MASK;
	
	// Set output to '1' on init
	FTM3_OUTINIT |= FTM_OUTINIT_CH0OI_MASK;
	
  // Set the Counter Initial Value to 0
	FTM3_CNTIN = 0;
  
	// Initialize the CNT to 0 before writing to MOD
	FTM3_CNT = 0;
	
	// Set the period (~10us)
	FTM3_MOD = FTM_MOD_MOD(DEFAULT_SYSTEM_CLOCK/100000.0);
	
  // Set edge-aligned mode
	FTM3_C0SC |= FTM_CnSC_MSB_MASK;
  
	// 50% duty
	FTM3_C0V = FTM_CnV_VAL(DEFAULT_SYSTEM_CLOCK/200000.0);
	
	// Enable High-true pulses
	// ELSB = 1, ELSA = 0
	FTM3_C0SC |= FTM_CnSC_ELSB_MASK;
  FTM3_C0SC &=~ FTM_CnSC_ELSA_MASK;
	
  // init external hardware trigger 
  FTM3_EXTTRIG |= FTM_EXTTRIG_INITTRIGEN_MASK;
  
	// Enable hardware trigger from FTM2
	FTM3_EXTTRIG |= FTM_EXTTRIG_CH0TRIG_MASK;
	
	// Don't enable interrupts yet (disable)
	FTM3_SC &=~ FTM_SC_TOIE_MASK;
	
	// No prescalar, system clock
	FTM3_SC = FTM_SC_PS(000);
  FTM3_SC = FTM_SC_CLKS(01);
	
	// Set up interrupt
	NVIC_EnableIRQ(FTM3_IRQn);
	
	return;
}

/* Initialization of PIT timer to control 
* 		integration period
*/
void InitPIT(void){
	// Setup periodic interrupt timer (PIT)
  SIM_SCGC6 |= SIM_SCGC6_PIT_MASK;
	
	// Enable clock for timers
	PIT_MCR &= ~PIT_MCR_MDIS_MASK;
	
	// Enable timers to continue in debug mode
	PIT_MCR &= ~PIT_MCR_FRZ_MASK; // In case you need to debug
	
	// PIT clock frequency is the system clock
	// Load the value that the timer will count down from
	PIT_LDVAL0 = PIT_LDVAL_TSV(INTEGRATION_TIME*DEFAULT_SYSTEM_CLOCK);
	
  // Clear interrupt flag
	PIT_TFLG0 = PIT_TFLG_TIF_MASK;
	
	// Enable timer interrupts
	PIT_TCTRL0 |= PIT_TCTRL_TIE_MASK;
	
	// Enable the timer
	PIT_TCTRL0 |= PIT_TCTRL_TEN_MASK;

	// Enable PIT interrupt in the interrupt controller
	NVIC_EnableIRQ(PIT0_IRQn);
	return;
}


/* Set up pins for GPIO
* 	PTB9 		- camera clk
*		PTB23		- camera SI
*		PTB22		- red LED
*/
void InitGPIO(void){
	// Enable LED and GPIO so we can see results
  // Enable clocks on Ports B and E for LED timing
  SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;

  // Configure the Signal Multiplexer for GPIO
  PORTB_PCR9  = PORT_PCR_MUX(1);
  PORTB_PCR23 = PORT_PCR_MUX(1);  
  PORTB_PCR22 = PORT_PCR_MUX(1);	
	
  
  // Switch the GPIO pins to output mode
  GPIOB_PDDR |= (1UL << 22);
  GPIOB_PDDR |= (1UL << 9);
  GPIOB_PDDR |= (1UL << 23);

  // Turn off the LEDs
  GPIOB_PSOR = (1UL << 22);
	return;
}

/* Set up ADC for capturing camera data */
void InitADC0(void) {
  unsigned int calib;
  // Turn on ADC0
  SIM_SCGC6 |= SIM_SCGC6_ADC0_MASK;

  // Set ADC input pin
	ADC0_SC1A &= ADC_SC1_ADCH(0);
  
	// Set to single ended mode	
	ADC0_SC1A &= ~(ADC_SC1_DIFF_MASK);
	
	// Single ended 16 bit conversion, no clock divider
	ADC0_CFG1 |= ADC_CFG1_MODE(3);
    
  // Do ADC Calibration for Singled Ended ADC. Do not touch.
  ADC0_SC3 = ADC_SC3_CAL_MASK;
  while ( (ADC0_SC3 & ADC_SC3_CAL_MASK) != 0 );
  calib = ADC0_CLP0; calib += ADC0_CLP1; calib += ADC0_CLP2;
  calib += ADC0_CLP3; calib += ADC0_CLP4; calib += ADC0_CLPS;
  calib = calib >> 1; calib |= 0x8000;
  ADC0_PG = calib;
    
  // Select hardware trigger.
  ADC0_SC2 |= ADC_SC2_ADTRG_MASK;
	
	// Set up FTM2 trigger on ADC0
	SIM_SOPT7 |= SIM_SOPT7_ADC0TRGSEL(11); // FTM3 select
	SIM_SOPT7 |= SIM_SOPT7_ADC0ALTTRGEN_MASK; // Alternative trigger en.
	SIM_SOPT7 &=~ SIM_SOPT7_ADC0PRETRGSEL_MASK; // Pretrigger A
	
	// Enable NVIC interrupt
  NVIC_EnableIRQ(ADC0_IRQn);
  
  // Enable interrupt
  ADC0_SC1A |= ADC_SC1_AIEN_MASK;
}
