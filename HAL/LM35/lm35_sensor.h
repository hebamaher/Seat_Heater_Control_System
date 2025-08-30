/*
 * lm35_sensor.h
 *
 *  Created on: Aug 3, 2024
 *      Author: Heba Maher
 */

#ifndef HAL_LM35_LM35_SENSOR_H_
#define HAL_LM35_LM35_SENSOR_H_

#include "std_types.h"

#define SENSOR_1_CHANNEL_ID         PE3_PIN
#define SENSOR_2_CHANNEL_ID         PE2_PIN
#define SENSOR_MAX_TEMPERATURE      45
#define SENSOR_MAX_VOLT             3.3

uint8 LM35_getTemperature(uint8 channel);



#endif /* HAL_LM35_LM35_SENSOR_H_ */
