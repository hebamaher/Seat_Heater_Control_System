/*
 * adc.h
 *
 *  Created on: Aug 2, 2024
 *      Author: Heba Maher
 */

#ifndef MCAL_ADC_ADC_H_
#define MCAL_ADC_ADC_H_

#include "std_types.h"

#define ADC_MAXIMUM_VALUE           0xFFF
#define ADC_REF_VOLT                3.3

typedef enum {
    PE3_PIN,PE2_PIN,PE1_PIN,PE0_PIN,PD3_PIN,PD2_PIN,PD1_PIN,PD0_PIN,
    PE5_PIN,PE4_PIN,PB4_PIN,PB5_PIN
} channel_num;

/*******************************************************************************
 *                            Functions Prototypes                             *
 *******************************************************************************/

void ADC0_Init();
uint32 ADC0_ReadChannel(channel_num channel);


#endif /* MCAL_ADC_ADC_H_ */
