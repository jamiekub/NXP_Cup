#ifndef ISR_H_
#define ISR_H_

void ADC0_IRQHandler(void);
void FTM3_IRQHandler(void);
void PIT0_IRQHandler(void);
int* getLine(void);

#endif /* ISR_H_ */
