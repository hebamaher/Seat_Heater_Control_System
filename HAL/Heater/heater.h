/*
 * heater.h
 *
 *  Created on: Aug 3, 2024
 *      Author: Heba Maher
 */

#ifndef HAL_HEATER_HEATER_H_
#define HAL_HEATER_HEATER_H_

#include "std_types.h"

#define DRIVER_SEAT_HEATER_ID       (0u)
#define PASSENGER_SEAT_HEATER_ID    (1u)

#define OFF                         20
#define LOW                         25
#define MEDIUM                      30
#define HIGH                        35

#define NEXT_LEVEL                  5

typedef struct{
    uint8 portNum;
    uint8 PinNums[3];
}heater_type;
void HEATER_setHeatingIntensity(heater_type* heater, uint8 HeatingIntensity);
void HEATER_Init(heater_type *heater_parameters);
void rgb_init(heater_type* led);
void rgb_redLedON(heater_type* led);
void rgb_greenLedON(heater_type* led);
void rgb_blueLedON(heater_type* led);
void rgb_redLedOFF(heater_type* led);
void rgb_greenLedOFF(heater_type* led);
void rgb_blueLedOFF(heater_type* led);
void rgb_redLedToggle(heater_type* led);
void rgb_greenLedToggle(heater_type* led);
void rgb_blueLedToggle(heater_type* led);
void rgb_allLedsOff(heater_type* led);

#endif /* HAL_HEATER_HEATER_H_ */
