#ifndef PWM_H_
#define PWM_H_

void SetDutyCycle(unsigned int DutyCycle, unsigned int Frequency, int dir, int wheelSel);
void SetDutyCycleServo(double DutyCycle);
void InitPWM(void);
void PWM_ISR(void);

#endif /* PWM_H_ */
