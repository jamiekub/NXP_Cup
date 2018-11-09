#ifndef FTM_H_
#define FTM_H_

#include <stdint.h>

void InitGPIO(void);
void InitADC0(void);
void InitPIT(void);
void InitFTM3(void);
void filter(uint16_t*);

#endif /* FTM_H_ */
