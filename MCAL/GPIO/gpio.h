/**********************************************************************************************
 *
 * Module: GPIO
 *
 * File Name: GPIO.h
 *
 * Description: Header file for the TM4C123GH6PM DIO driver for TivaC Built-in Buttons and LEDs
 *
 * Author: Edges for Training Team
 *
 ***********************************************************************************************/

#ifndef GPIO_H_
#define GPIO_H_

#include "std_types.h"
#include "Common_Macros.h"

#define GPIO_PORTF_PRIORITY_MASK      0xFF1FFFFF
#define GPIO_PORTF_PRIORITY_BITS_POS  21
#define GPIO_PORTF_INTERRUPT_PRIORITY 5

#define GPIO_PORTA_PRIORITY_MASK      0xFFFFFF1F
#define GPIO_PORTA_PRIORITY_BITS_POS  5
#define GPIO_PORTA_INTERRUPT_PRIORITY 5

#define PRESSED                ((uint8)0x00)
#define RELEASED               ((uint8)0x01)

void GPIO_BuiltinButtonsLedsInit(void);

void GPIO_RedLedOn(void);
void GPIO_BlueLedOn(void);
void GPIO_GreenLedOn(void);

void GPIO_RedLedOff(void);
void GPIO_BlueLedOff(void);
void GPIO_GreenLedOff(void);

void GPIO_RedLedToggle(void);
void GPIO_BlueLedToggle(void);
void GPIO_GreenLedToggle(void);

uint8 GPIO_SW1GetState(void);
uint8 GPIO_SW2GetState(void);
uint8 GPIO_SW3GetState(void);

void GPIO_SW1EdgeTriggeredInterruptInit(void);
void GPIO_SW2EdgeTriggeredInterruptInit(void);
void GPIO_SW3EdgeTriggeredInterruptInit(void);


/**************************************************************/

typedef enum {
    PIN_INPUT,PIN_OUTPUT,UNDEFINED
}GPIO_PinDirectionType;

typedef enum{
    PORT_INPUT,PORT_OUTPUT=0xFF
}GPIO_PortDirectioType;
typedef enum{
    PULL_UP,PULL_DOWN ,NO_RESISTOR
}GPIO_ResistorType;
/****************************
 *          PINS
 ****************************/
#define PIN0_ID                0
#define PIN1_ID                1
#define PIN2_ID                2
#define PIN3_ID                3
#define PIN4_ID                4
#define PIN5_ID                5
#define PIN6_ID                6
#define PIN7_ID                7
/***************************
 *         PORTS
 ***************************/
#define GPIO_PORTA             0
#define GPIO_PORTB             1
#define GPIO_PORTC             2
#define GPIO_PORTD             3
#define GPIO_PORTE             4
#define GPIO_PORTF             5
/*******************************************************************************
 *                              Module Definitions                             *
 *******************************************************************************/

/* GPIO Registers base addresses */
#define GPIO_PORTA_BASE_ADDRESS           0x40004000
#define GPIO_PORTB_BASE_ADDRESS           0x40005000
#define GPIO_PORTC_BASE_ADDRESS           0x40006000
#define GPIO_PORTD_BASE_ADDRESS           0x40007000
#define GPIO_PORTE_BASE_ADDRESS           0x40024000
#define GPIO_PORTF_BASE_ADDRESS           0x40025000

/* GPIO Registers offset addresses */
#define PORT_DATA_REG_OFFSET              0x3FC
#define PORT_DIR_REG_OFFSET               0x400
#define PORT_ALT_FUNC_REG_OFFSET          0x420
#define PORT_PULL_UP_REG_OFFSET           0x510
#define PORT_PULL_DOWN_REG_OFFSET         0x514
#define PORT_DIGITAL_ENABLE_REG_OFFSET    0x51C
#define PORT_LOCK_REG_OFFSET              0x520
#define PORT_COMMIT_REG_OFFSET            0x524
#define PORT_ANALOG_MODE_SEL_REG_OFFSET   0x528
#define PORT_CTL_REG_OFFSET               0x52C
/* Definitions for */
#define PORT_MODE_DIO    (0U)
#define PORT_MODE1       (1U)
#define PORT_MODE2       (2U)
#define PORT_MODE3       (3U)
#define PORT_MODE4       (4U)
#define PORT_MODE5       (5U)
#define PORT_MODE6       (6U)
#define PORT_MODE7       (7U)
#define PORT_MODE8       (8U)
#define PORT_MODE9       (9U)
#define PORT_MODE14      (14U)
#define PORT_MODE_ANALOG (15U)


void GPIO_setupPinDirectionAndMode(uint8 portNum , uint8 pinNum , GPIO_PinDirectionType direction ,uint8 pinMode,uint8 resistor);
void GPIO_writePin(uint8 pinNum ,uint8 portNum,uint8 value);
uint8 GPIO_readPin(uint8 portNum , uint8 pinNum );
void GPIO_togglePin(uint8 portNum,uint8 pinNum);
void Mcu_init(void);




#endif /* GPIO_H_ */
