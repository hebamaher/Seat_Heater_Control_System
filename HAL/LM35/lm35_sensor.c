/*
 * lm35_sensor.c
 *
 *  Created on: Aug 3, 2024
 *      Author: Heba Maher
 */

#include "lm35_sensor.h"
#include "adc.h"

uint8 LM35_getTemperature(uint8 channel){
    uint8 temp = 0;
    uint16 adc_value = 0;
    adc_value = ADC0_ReadChannel((channel_num)channel); //output of adc stored in adc reg that is being returned from this function

    temp = (uint8)(((uint32)adc_value * SENSOR_MAX_TEMPERATURE * ADC_REF_VOLT)/(SENSOR_MAX_VOLT * ADC_MAXIMUM_VALUE)); //using adc value in temp calc
    return temp;
}



