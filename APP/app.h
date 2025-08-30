/*
 * app.h
 *
 *  Created on: Aug 3, 2024
 *      Author: Heba Maher
 */

#ifndef APP_APP_H_
#define APP_APP_H_

#include "std_types.h"
#include "adc.h"
#include "heater.h"
#include "FreeRTOS.h"
#include "semphr.h"
/* Define the lengths of the queues that will be added to the queue set. */
#define QUEUE_LENGTH      5

/* Binary semaphores have an effective length of 1. */
#define BINARY_SEMAPHORE_LENGTH 1

#define DRIVER_SEAT     0
#define PASSENGER_SEAT  1

typedef struct{
    uint8 seat_type;
    uint8 *heater_level;
    uint8 *prev_heater_level;
    SemaphoreHandle_t* semaphore;
}ButtonTaskParameterType;

typedef struct{
    channel_num channel_id;
    uint8 *current_temp;
}MonitorTempTaskParameterType;

typedef struct{
    uint8 seat_type;
    uint8 *current_temp;
    uint8 *desired_temp;
    uint8 *heater_intensity;
}ControlTempTaskParameterType;

typedef struct{
    heater_type *heater;
    uint8 *heater_intensity;
}HeatingTaskParameterType;

/* Diagnostics */
typedef struct
{
    uint32 TimeStamp;
    uint8 error_code;
}Diagnostics_type;

typedef struct{
    uint8 seat_type;
    uint8 *current_temp;
    uint8 *error_code;
}FailureHandlingTaskParameterType;

typedef struct{
    uint8 seat_type;
    uint8 *current_temp;
    uint8 *desired_temp;
    uint8 *heater_intensity;
}DisplayStateTaskParameterType;

void APP_Init(void);
void APP_Start(void);

/* FreeRTOS tasks */
void vButtonTask(void *pvParameters);
void vMonitorTemperatureTask(void *pvParameters);
void vControlTemperatureTask(void *pvParameters);
void vHeatingTask(void *pvParameters);
void vDisplayStateTask(void *pvParameters);
void vFailureHandlingTask(void *pvParameters);
void vRunTimeMeasurementsTask(void *pvParameters);

#endif /* APP_APP_H_ */
