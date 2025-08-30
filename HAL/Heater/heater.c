/*
 * heater.c
 *
 *  Created on: Aug 3, 2024
 *      Author: Heba Maher
 */

#include "heater.h"
#include "gpio.h"

void HEATER_Init(heater_type *heater_parameters){
    GPIO_setupPinDirectionAndMode(heater_parameters->portNum, heater_parameters->PinNums[0], PIN_OUTPUT, PORT_MODE_DIO, NO_RESISTOR);
    GPIO_setupPinDirectionAndMode(heater_parameters->portNum, heater_parameters->PinNums[1], PIN_OUTPUT, PORT_MODE_DIO, NO_RESISTOR);
    GPIO_setupPinDirectionAndMode(heater_parameters->portNum, heater_parameters->PinNums[2], PIN_OUTPUT, PORT_MODE_DIO, NO_RESISTOR);
}
void rgb_redLedON(heater_type* led)
{
    GPIO_writePin(led->PinNums[0], led->portNum, LOGIC_HIGH);
}
void rgb_greenLedON(heater_type* led)
{
    GPIO_writePin(led->PinNums[1], led->portNum, LOGIC_HIGH);
}
void rgb_blueLedON(heater_type* led)
{
    GPIO_writePin(led->PinNums[2], led->portNum, LOGIC_HIGH);
}
void rgb_redLedOFF(heater_type* led)
{
    GPIO_writePin(led->PinNums[0], led->portNum, LOGIC_LOW);
}
void rgb_greenLedOFF(heater_type* led)
{
    GPIO_writePin(led->PinNums[1], led->portNum, LOGIC_LOW);
}
void rgb_blueLedOFF(heater_type* led)
{
    GPIO_writePin(led->PinNums[2], led->portNum, LOGIC_LOW);;
}
void rgb_redLedToggle(heater_type* led)
{
    GPIO_togglePin(led->portNum, led->PinNums[0]);
}
void rgb_greenLedToggle(heater_type* led)
{
    GPIO_togglePin(led->portNum, led->PinNums[1]);
}
void rgb_blueLedToggle(heater_type* led)
{
    GPIO_togglePin(led->portNum, led->PinNums[2]);
}
void rgb_allLedsOff(heater_type* led)
{
    uint8 i;
    for(i=0;i<3;i++)
    {
        GPIO_writePin(led->PinNums[i], led->portNum, LOGIC_LOW);
    }

}
void HEATER_setHeatingIntensity(heater_type* heater, uint8 HeatingIntensity){
    switch(HeatingIntensity){
    case OFF:
        rgb_greenLedOFF(heater);
        rgb_blueLedOFF(heater);
        break;
    case LOW:
        rgb_greenLedON(heater);
        rgb_blueLedOFF(heater);
        break;
    case MEDIUM:
        rgb_greenLedOFF(heater);
        rgb_blueLedON(heater);
        break;
    case HIGH:
        rgb_greenLedON(heater);
        rgb_blueLedON(heater);
        break;
    }

}
