/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"
#include "list.h"
#include "timers.h"


/* MCAL includes. */
#include "gpio.h"
#include "uart0.h"
#include "GPTM.h"
#include "adc.h"
#include "tm4c123gh6pm_registers.h"
/* HAL includes. */
#include "heater.h"
#include "lm35_sensor.h"
#include "string.h"
#include "app.h"

#define RUNTIME_MEASUREMENTS_TASK_PERIODICITY       (1000U)

#define MONITOR_TEMP_TASK_PERIODICITY               (150U)
#define CONTROL_TEMP_TASK_PERIODICITY               (100U)

#define DISPLAY_TASK_PERIODICITY                    (200U)

/*  Tasks Handlers */
TaskHandle_t DriverSeatButtonTaskHandler;
TaskHandle_t PassengerSeatButtonTaskHandler;

TaskHandle_t DriverMonitorTempTaskHandler;
TaskHandle_t PassengerMonitorTempTaskHandler;

TaskHandle_t DriverControlTempTaskHandler;
TaskHandle_t PassengerControlTempTaskHandler;

TaskHandle_t DriverHeatingTaskHandler;
TaskHandle_t PassengerHeatingTaskHandler;

TaskHandle_t DriverDisplayStateTaskHandler;
TaskHandle_t PassengerDisplayStateTaskHandler;

TaskHandle_t DriverSeatFailureHandlingTaskHandler;
TaskHandle_t PassengerSeatFailureHandlingTaskHandler;

TaskHandle_t RunTimeMeasurementsTaskHandler;

/* FreeRTOS Binary Semaphores */
SemaphoreHandle_t xDriverButtonSemaphore1;
SemaphoreHandle_t xDriverButtonSemaphore2;
SemaphoreHandle_t xPassengerButtonSemaphore;

/*  Tasks Parameters */
uint8 driver_seat_heater_level = OFF;
uint8 driver_seat_prev_heater_level = OFF;
uint8 passenger_seat_heater_level = OFF;
uint8 passenger_seat_prev_heater_level = OFF;

ButtonTaskParameterType driver_seat_button_task_parameters = {DRIVER_SEAT, &driver_seat_heater_level, &driver_seat_prev_heater_level, &xDriverButtonSemaphore1};
ButtonTaskParameterType passenger_seat_button_task_parameters = {PASSENGER_SEAT, &passenger_seat_heater_level, &passenger_seat_prev_heater_level, &xPassengerButtonSemaphore};

uint8 driver_seat_current_temp = 0;
uint8 passenger_seat_current_temp = 0;
MonitorTempTaskParameterType driver_seat_monitor_task_parameters = {SENSOR_1_CHANNEL_ID, &driver_seat_current_temp};
MonitorTempTaskParameterType passenger_seat_monitor_task_parameters = {SENSOR_2_CHANNEL_ID, &passenger_seat_current_temp};

uint8 driver_seat_heater_intensity = 0;
uint8 passenger_seat_heater_intensity = 0;
ControlTempTaskParameterType driver_seat_control_task_parameters = {DRIVER_SEAT, &driver_seat_current_temp, &driver_seat_heater_level, &driver_seat_heater_intensity};
ControlTempTaskParameterType passenger_seat_control_task_parameters = {PASSENGER_SEAT, &passenger_seat_current_temp, &passenger_seat_heater_level, &passenger_seat_heater_intensity};

heater_type driver_heater={GPIO_PORTF, {PIN1_ID, PIN3_ID, PIN2_ID}};
heater_type passenger_heater={GPIO_PORTD, {PIN0_ID, PIN1_ID, PIN2_ID}};
HeatingTaskParameterType driver_seat_heating_task_parameters = {&driver_heater, &driver_seat_heater_intensity};
HeatingTaskParameterType passenger_seat_heating_task_parameters = {&passenger_heater, &passenger_seat_heater_intensity};

/* Diagnostics */
Diagnostics_type DiagnosticsArray[10];
uint8 Diagnostics_index = 0;

uint8 driver_error_code = 0;
uint8 passenger_error_code = 0;
FailureHandlingTaskParameterType driver_seat_failuer_handling_task_parameters = {DRIVER_SEAT, &driver_seat_current_temp, &driver_error_code};
FailureHandlingTaskParameterType passenger_seat_failuer_handling_task_parameters = {PASSENGER_SEAT, &passenger_seat_current_temp, &passenger_error_code};

DisplayStateTaskParameterType driver_seat_display_task_parameters = {DRIVER_SEAT, &driver_seat_current_temp, &driver_seat_heater_level, &driver_seat_heater_intensity};
DisplayStateTaskParameterType passenger_seat_display_task_parameters = {PASSENGER_SEAT, &passenger_seat_current_temp, &passenger_seat_heater_level, &passenger_seat_heater_intensity};

/* FreeRTOS Queues */
//QueueHandle_t driver_seat_heating_level_queue;
//QueueHandle_t passenger_seat_heating_level_queue;
//QueueHandle_t current_temp_queue;

QueueHandle_t driver_seat_failure_queue;
QueueHandle_t passenger_seat_failure_queue;



//SemaphoreHandle_t current_temp_mutex;
SemaphoreHandle_t desired_temp_mutex;
SemaphoreHandle_t heater_intensity_mutex;
SemaphoreHandle_t uart_mutex;

SemaphoreHandle_t xHeaterIntensitySemaphore;

/* Definitions for the event bits in the event group. */
#define BUTTON_TASK_BIT     ( 1UL << 0UL )  /* SW1 event bit 0, which is set by button SW1 task. */
#define MONITOR_TASK_BIT    ( 1UL << 1UL )  /* SW2 event bit 1, which is set by button SW2 task. */
#define CONTROL_TASK_BIT    ( 1UL << 2UL )  /* SW1 event bit 0, which is set by button SW1 task. */

/* Run Time Measurements */
uint32 ullTasksOutTime[14];
uint32 ullTasksInTime[14];
uint32 ullTasksExecutionTime[14];
uint32 ullTasksTotalTime[14];

void APP_Init(void)
{
    /* Setup the hardware for use with the Tiva C board. */
    Mcu_init();
    UART0_Init();
    GPIO_BuiltinButtonsLedsInit();
    GPIO_SW1EdgeTriggeredInterruptInit();
    GPIO_SW2EdgeTriggeredInterruptInit();
    HEATER_Init(&passenger_heater);
    GPIO_SW3EdgeTriggeredInterruptInit();
    ADC0_Init();
    GPTM_WTimer0Init();

    /* Create a queue capable of containing 5 uint8 values. */
    driver_seat_failure_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8));
    passenger_seat_failure_queue = xQueueCreate(QUEUE_LENGTH, sizeof(uint8));
    /* Create a binary semaphore */
    //current_temp_mutex = xSemaphoreCreateMutex();
    //desired_temp_mutex = xSemaphoreCreateMutex();
    heater_intensity_mutex = xSemaphoreCreateMutex();
    uart_mutex = xSemaphoreCreateMutex();

    xHeaterIntensitySemaphore = xSemaphoreCreateBinary();
    xDriverButtonSemaphore1 = xSemaphoreCreateBinary();
    xDriverButtonSemaphore2 = xSemaphoreCreateBinary();
    xPassengerButtonSemaphore = xSemaphoreCreateBinary();

    /* Create Tasks here */
    xTaskCreate(vButtonTask, "Driver Seat Button Task", 64, &driver_seat_button_task_parameters, 4, &DriverSeatButtonTaskHandler);
    xTaskCreate(vMonitorTemperatureTask, "Driver Seat Monitoring Temp Task", 64, &driver_seat_monitor_task_parameters, 3, &DriverMonitorTempTaskHandler);
    xTaskCreate(vControlTemperatureTask, "Driver Seat Control Temp Task", 64, &driver_seat_control_task_parameters, 2, &DriverControlTempTaskHandler);
    xTaskCreate(vHeatingTask, "Driver Seat Heating Task", 64, &driver_seat_heating_task_parameters, 2, &DriverHeatingTaskHandler);
    xTaskCreate(vFailureHandlingTask, "Driver Seat Failure Handling Task", 64, &driver_seat_failuer_handling_task_parameters, 1, &DriverSeatFailureHandlingTaskHandler);
    xTaskCreate(vDisplayStateTask, "Driver Seat Display Task", 64, &driver_seat_display_task_parameters, 4, &DriverDisplayStateTaskHandler);

        xTaskCreate(vButtonTask, "Passenger Seat Button Task", 64, &passenger_seat_button_task_parameters, 4, &PassengerSeatButtonTaskHandler);
        xTaskCreate(vMonitorTemperatureTask, "Passenger Seat Monitoring Temp Task", 64, &passenger_seat_monitor_task_parameters, 3, &PassengerMonitorTempTaskHandler);
        xTaskCreate(vControlTemperatureTask, "Passenger Seat Control Temp Task", 64, &passenger_seat_control_task_parameters, 2, &PassengerControlTempTaskHandler);
        xTaskCreate(vHeatingTask, "Passenger Seat Heating Task", 64, &passenger_seat_heating_task_parameters, 2, &PassengerHeatingTaskHandler);
        xTaskCreate(vFailureHandlingTask, "Passenger Seat Failure Handling Task", 64, &passenger_seat_failuer_handling_task_parameters, 1, &PassengerSeatFailureHandlingTaskHandler);
        xTaskCreate(vDisplayStateTask, "Passenger Seat Display Task", 64, &passenger_seat_display_task_parameters, 1, &PassengerDisplayStateTaskHandler);

    xTaskCreate(vRunTimeMeasurementsTask, "RunTime Measurements Task", 256, NULL, 1, &RunTimeMeasurementsTaskHandler);

    vTaskSetApplicationTaskTag( DriverSeatButtonTaskHandler, ( TaskHookFunction_t ) 1 );
    vTaskSetApplicationTaskTag( PassengerSeatButtonTaskHandler, ( TaskHookFunction_t ) 2 );
    vTaskSetApplicationTaskTag( DriverMonitorTempTaskHandler, ( TaskHookFunction_t ) 3 );
    vTaskSetApplicationTaskTag( PassengerMonitorTempTaskHandler, ( TaskHookFunction_t ) 4 );
    vTaskSetApplicationTaskTag( DriverControlTempTaskHandler, ( TaskHookFunction_t ) 5 );
    vTaskSetApplicationTaskTag( PassengerControlTempTaskHandler, ( TaskHookFunction_t ) 6 );
    vTaskSetApplicationTaskTag( DriverHeatingTaskHandler, ( TaskHookFunction_t ) 7 );
    vTaskSetApplicationTaskTag( PassengerHeatingTaskHandler, ( TaskHookFunction_t ) 8 );
    vTaskSetApplicationTaskTag( DriverDisplayStateTaskHandler, ( TaskHookFunction_t ) 9 );
    vTaskSetApplicationTaskTag( PassengerDisplayStateTaskHandler, ( TaskHookFunction_t ) 10 );
    vTaskSetApplicationTaskTag( DriverSeatFailureHandlingTaskHandler, ( TaskHookFunction_t ) 11 );
    vTaskSetApplicationTaskTag( PassengerSeatFailureHandlingTaskHandler, ( TaskHookFunction_t ) 12 );
    vTaskSetApplicationTaskTag( RunTimeMeasurementsTaskHandler, ( TaskHookFunction_t ) 13 );

    /* Should never reach here!  If you do then there was not enough heap
    available for the idle task to be created. */
    //for (;;);

}

void APP_Start(void){
    vTaskStartScheduler();
}
void vButtonTask(void *pvParameters)
{
    ButtonTaskParameterType *input = (ButtonTaskParameterType*) pvParameters;
    /* Event-Based Task */
    for (;;) {
        xSemaphoreTake(*(input->semaphore),portMAX_DELAY);
        xSemaphoreTake(desired_temp_mutex,portMAX_DELAY);
        *(input->prev_heater_level) = *(input->heater_level);
        if(*(input->heater_level) != HIGH){
            *(input->heater_level) += NEXT_LEVEL;
        }
        else{
            *(input->heater_level) = OFF;
        }
        xSemaphoreGive(desired_temp_mutex);
    }

}
void vMonitorTemperatureTask(void *pvParameters){
    MonitorTempTaskParameterType *temp_sensor = (MonitorTempTaskParameterType*)pvParameters;
    /* Establish the task's period */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        vTaskDelayUntil( &xLastWakeTime, MONITOR_TEMP_TASK_PERIODICITY );
        *(temp_sensor->current_temp) = LM35_getTemperature((channel_num)temp_sensor->channel_id);
    }

}

void vControlTemperatureTask(void *pvParameters){
    ControlTempTaskParameterType *temp_control = (ControlTempTaskParameterType*)pvParameters;
    uint8 current_temp = 0;
    uint8 desired_temp = 0;
    int8_t diff_value = 0;
    /* Establish the task's period. */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        vTaskDelayUntil( &xLastWakeTime, CONTROL_TEMP_TASK_PERIODICITY );
        current_temp = *(temp_control->current_temp);
        desired_temp = *(temp_control->desired_temp);

        /* Check if Temp out of range */
        if (current_temp > 40 || current_temp < 5)
        {
            switch(temp_control->seat_type)
            {
            case DRIVER_SEAT:
                xQueueSend(driver_seat_failure_queue, &current_temp, 0);
                break;

            case PASSENGER_SEAT:
                xQueueSend(passenger_seat_failure_queue, &current_temp, 0);
                break;
            }

        }
        else if (current_temp <= 40 && current_temp >= 5)
        {
            switch(temp_control->seat_type)
            {
            case DRIVER_SEAT:
                rgb_redLedOFF(&driver_heater);
                break;

            case PASSENGER_SEAT:
                rgb_redLedOFF(&passenger_heater);
                break;

            default:
                continue;
            }
        }

        diff_value = desired_temp - current_temp;

        /* failure handle task */
        if(current_temp<5 || current_temp>40){
            xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
            *(temp_control->heater_intensity) = OFF;
            xSemaphoreGive(heater_intensity_mutex);
        }
        else{
            if((diff_value <= 2) || (desired_temp == OFF)){
                xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
                *(temp_control->heater_intensity) = OFF;
                xSemaphoreGive(heater_intensity_mutex);
            }
            else if( (diff_value > 2) && (diff_value < 5) ){
                xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
                *(temp_control->heater_intensity) = LOW;
                xSemaphoreGive(heater_intensity_mutex);

            }
            else if( (diff_value >= 5) && (diff_value < 10) ){
                xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
                *(temp_control->heater_intensity) = MEDIUM;
                xSemaphoreGive(heater_intensity_mutex);

            }
            else{
                xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
                *(temp_control->heater_intensity) = HIGH;
                xSemaphoreGive(heater_intensity_mutex);
            }
        }
        /* Synchronization between Control Task and Heating Task */
        xSemaphoreGive(xHeaterIntensitySemaphore);
    }
}

void vHeatingTask(void *pvParameters){
    HeatingTaskParameterType *heater = (HeatingTaskParameterType*)pvParameters;
    uint8 intensity = 0;
    /* Event-Based Task */
    for(;;){
        /* Synchronization between Control Task and Heating Task */
        if(xSemaphoreTake(xHeaterIntensitySemaphore,portMAX_DELAY)){
            intensity = *(heater->heater_intensity);
        }
        HEATER_setHeatingIntensity(heater->heater, intensity);
    }
}

void vFailureHandlingTask(void *pvParameters){
    FailureHandlingTaskParameterType *failure_handler = (FailureHandlingTaskParameterType*)pvParameters;
    uint8 current_temp = 0;
    for(;;){
        current_temp = *(failure_handler->current_temp);

        switch(failure_handler->seat_type){

        case DRIVER_SEAT:
            xQueueReceive(driver_seat_failure_queue, &current_temp, portMAX_DELAY);
            rgb_redLedON(&driver_heater);

            xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
            driver_seat_heater_intensity = OFF;
            xSemaphoreGive(heater_intensity_mutex);

            if(*(failure_handler->current_temp) > 40){
                *(failure_handler->error_code) = 1;
            }
            else if(*(failure_handler->current_temp) < 5){
                *(failure_handler->error_code) = 2;
            }
            /* Adding Diagnostics */
            taskENTER_CRITICAL();/* ----------- Critical Section ----------- */

            DiagnosticsArray[Diagnostics_index].TimeStamp = GPTM_WTimer0Read();
            DiagnosticsArray[Diagnostics_index].error_code = *(failure_handler->error_code);
            Diagnostics_index = (Diagnostics_index + 1) % 10;

            taskEXIT_CRITICAL(); /* ----------- Critical Section ----------- */

            break;

        case PASSENGER_SEAT:
            xQueueReceive(passenger_seat_failure_queue, &current_temp, portMAX_DELAY);

            rgb_redLedON(&passenger_heater);

            xSemaphoreTake(heater_intensity_mutex, portMAX_DELAY);
            passenger_seat_heater_intensity = OFF;
            xSemaphoreGive(heater_intensity_mutex);

            /* Adding Diagnostics */
            taskENTER_CRITICAL();/* ----------- Critical Section ----------- */

            DiagnosticsArray[Diagnostics_index].TimeStamp = GPTM_WTimer0Read();
            DiagnosticsArray[Diagnostics_index].error_code = *(failure_handler->error_code);
            Diagnostics_index = (Diagnostics_index + 1) % 10;

            taskEXIT_CRITICAL(); /* ----------- Critical Section ----------- */

            break;
        }
    }
}

void vDisplayStateTask(void *pvParameters){
    DisplayStateTaskParameterType *display_state = (DisplayStateTaskParameterType*)pvParameters;
    /* Establish the task's period. */
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for(;;){
        vTaskDelayUntil( &xLastWakeTime, DISPLAY_TASK_PERIODICITY );

        switch(display_state->seat_type){
        case DRIVER_SEAT:
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            UART0_SendString("Driver Seat State: \r\n");
            break;
        case PASSENGER_SEAT:
            xSemaphoreTake(uart_mutex, portMAX_DELAY);
            UART0_SendString("Passenger Seat State: \r\n");
            break;
        }

        UART0_SendString("Current Temperature: ");

        UART0_SendInteger(*(display_state->current_temp));

        UART0_SendString("\r\n");

        UART0_SendString("Heating Level: ");
        switch(*(display_state->desired_temp)){
        case OFF:
            UART0_SendString("OFF\r\n");
            break;
        case LOW:
            UART0_SendString("LOW\r\n");
            break;
        case MEDIUM:
            UART0_SendString("MEDIUM\r\n");
            break;
        case HIGH:
            UART0_SendString("HIGH\r\n");
            break;
        }
        UART0_SendString("Heater Intensity: ");

        switch(*(display_state->heater_intensity)){
        case OFF:
            UART0_SendString("OFF\r\n");
            break;
        case LOW:
            UART0_SendString("LOW\r\n");
            break;
        case MEDIUM:
            UART0_SendString("MEDIUM\r\n");
            break;
        case HIGH:
            UART0_SendString("HIGH\r\n");
            break;
        }
        xSemaphoreGive(uart_mutex);
    }
}

void vRunTimeMeasurementsTask(void *pvParameters){
    TickType_t xLastWakeTime = xTaskGetTickCount();
    for (;;)
    {
        uint8 ucCounter, ucCPU_Load;
        uint32 ullTotalTasksTime = 0;
        vTaskDelayUntil(&xLastWakeTime, RUNTIME_MEASUREMENTS_TASK_PERIODICITY);
        for(ucCounter = 1; ucCounter < 8; ucCounter++)
        {
            ullTotalTasksTime += ullTasksTotalTime[ucCounter];
        }
        ucCPU_Load = (ullTotalTasksTime * 100) /  GPTM_WTimer0Read();

        taskENTER_CRITICAL();
        UART0_SendString("CPU Load is ");
        UART0_SendInteger(ucCPU_Load);
        UART0_SendString("% \r\n");
        taskEXIT_CRITICAL();
    }
}

void GPIOPortF_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(GPIO_PORTF_RIS_REG & (1<<4))
    {
        /* Synchronize with the input task for each edge triggered interrupt */
        xSemaphoreGiveFromISR(xDriverButtonSemaphore1, &xHigherPriorityTaskWoken);
        /* Clear Trigger flag for PF4 (Interrupt Flag) */
        GPIO_PORTF_ICR_REG   |= (1<<4);
    }
    else if(GPIO_PORTF_RIS_REG & (1<<0))
    {
        /* Synchronize with the input task for each edge triggered interrupt */
        xSemaphoreGiveFromISR(xPassengerButtonSemaphore, &xHigherPriorityTaskWoken);
        /* Clear Trigger flag for PF0 (Interrupt Flag) */
        GPIO_PORTF_ICR_REG   |= (1<<0);
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}

void GPIOPortA_Handler(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if(GPIO_PORTA_RIS_REG & (1<<6)){
        /* Synchronize with the input task for each edge triggered interrupt */
        xSemaphoreGiveFromISR(xDriverButtonSemaphore2, &xHigherPriorityTaskWoken);
        /* Clear Trigger flag for PA6 (Interrupt Flag) */
        GPIO_PORTA_ICR_REG |= (1<<6);
    }
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/
