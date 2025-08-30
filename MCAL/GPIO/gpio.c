#include "gpio.h"
#include "tm4c123gh6pm_registers.h"

void GPIO_BuiltinButtonsLedsInit(void)
{
    /*
     * PF0 --> SW2
     * PF1 --> Red LED
     * PF2 --> Blue LED
     * PF3 --> Green LED
     * PF4 --> SW1
     * PA6 --> SW3
     */

    /* Enable clock for PORTF and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x20;
    while(!(SYSCTL_PRGPIO_REG & 0x20));

    GPIO_PORTF_LOCK_REG   = 0x4C4F434B;                       /* Unlock the GPIO_PORTF_CR_REG */
    GPIO_PORTF_CR_REG    |= (1<<0);                           /* Enable changes on PF0 */
    GPIO_PORTF_AMSEL_REG &= 0xE0;                             /* Disable Analog on PF0, PF1, PF2, PF3 and PF4 */
    GPIO_PORTF_PCTL_REG  &= 0xFFF00000;                       /* Clear PMCx bits for PF0, PF1, PF2, PF3 and PF4 to use it as GPIO pins */
    GPIO_PORTF_DIR_REG   &= ~(1<<0) & ~(1<<4);                /* Configure PF0 & PF4 as input pins */
    GPIO_PORTF_DIR_REG   |= ((1<<1) | (1<<2) | (1<<3));       /* Configure PF1, PF2 & PF3 as output pins */
    GPIO_PORTF_AFSEL_REG &= 0xE0;                             /* Disable alternative function on PF0, PF1, PF2, PF3 and PF4 */
    GPIO_PORTF_PUR_REG   |= ((1<<0)|(1<<4));                  /* Enable pull-up on PF0 & PF4 */
    GPIO_PORTF_DEN_REG   |= 0x1F;                             /* Enable Digital I/O on PF0, PF1, PF2, PF3 and PF4 */
    GPIO_PORTF_DATA_REG  &= ~(1<<1) & ~(1<<2) & ~(1<<3);      /* Clear bits 1, 2 & 3 in Data register to turn off the LEDs */
}

void GPIO_RedLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<1);  /* Red LED ON */
}

void GPIO_BlueLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<2);  /* Blue LED ON */
}

void GPIO_GreenLedOn(void)
{
    GPIO_PORTF_DATA_REG |= (1<<3);  /* Green LED ON */
}

void GPIO_RedLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<1);  /* Red LED OFF */
}

void GPIO_BlueLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<2);  /* Blue LED OFF */
}

void GPIO_GreenLedOff(void)
{
    GPIO_PORTF_DATA_REG &= ~(1<<3);  /* Green LED OFF */
}

void GPIO_RedLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<1);  /* Red LED is toggled */
}

void GPIO_BlueLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<2);  /* Blue LED is toggled */
}

void GPIO_GreenLedToggle(void)
{
    GPIO_PORTF_DATA_REG ^= (1<<3);  /* Green LED is toggled */
}

uint8 GPIO_SW1GetState(void)
{
    return ((GPIO_PORTF_DATA_REG >> 4) & 0x01);
}

uint8 GPIO_SW2GetState(void)
{
    return ((GPIO_PORTF_DATA_REG >> 0) & 0x01);
}

uint8 GPIO_SW3GetState(void)
{
    return ((GPIO_PORTA_DATA_REG >> 6) & 0x01);
}

void GPIO_SW1EdgeTriggeredInterruptInit(void)
{
    GPIO_PORTF_IS_REG    &= ~(1<<4);      /* PF4 detect edges */
    GPIO_PORTF_IBE_REG   &= ~(1<<4);      /* PF4 will detect a certain edge */
    GPIO_PORTF_IEV_REG   &= ~(1<<4);      /* PF4 will detect a falling edge */
    GPIO_PORTF_ICR_REG   |= (1<<4);       /* Clear Trigger flag for PF4 (Interrupt Flag) */
    GPIO_PORTF_IM_REG    |= (1<<4);       /* Enable Interrupt on PF4 pin */
    /* Set GPIO PORTF priority as 5 by set Bit number 21, 22 and 23 with value 2 */
    NVIC_PRI7_REG = (NVIC_PRI7_REG & GPIO_PORTF_PRIORITY_MASK) | (GPIO_PORTF_INTERRUPT_PRIORITY<<GPIO_PORTF_PRIORITY_BITS_POS);
    NVIC_EN0_REG         |= 0x40000000;   /* Enable NVIC Interrupt for GPIO PORTF by set bit number 30 in EN0 Register */
}

void GPIO_SW2EdgeTriggeredInterruptInit(void)
{
    GPIO_PORTF_IS_REG    &= ~(1<<0);      /* PF0 detect edges */
    GPIO_PORTF_IBE_REG   &= ~(1<<0);      /* PF0 will detect a certain edge */
    GPIO_PORTF_IEV_REG   &= ~(1<<0);      /* PF0 will detect a falling edge */
    GPIO_PORTF_ICR_REG   |= (1<<0);       /* Clear Trigger flag for PF0 (Interrupt Flag) */
    GPIO_PORTF_IM_REG    |= (1<<0);       /* Enable Interrupt on PF0 pin */
    /* Set GPIO PORTF priority as 5 by set Bit number 21, 22 and 23 with value 2 */
    NVIC_PRI7_REG = (NVIC_PRI7_REG & GPIO_PORTF_PRIORITY_MASK) | (GPIO_PORTF_INTERRUPT_PRIORITY<<GPIO_PORTF_PRIORITY_BITS_POS);
    NVIC_EN0_REG         |= 0x40000000;   /* Enable NVIC Interrupt for GPIO PORTF by set bit number 30 in EN0 Register */
}

void GPIO_SW3EdgeTriggeredInterruptInit(void)
{
    GPIO_PORTA_IS_REG    &= ~(1<<6);      /* PA6 detect edges */
    GPIO_PORTA_IBE_REG   &= ~(1<<6);      /* PA6 will detect a certain edge */
    GPIO_PORTA_IEV_REG   &= ~(1<<6);      /* PA6 will detect a falling edge */
    GPIO_PORTA_ICR_REG   |= (1<<6);       /* Clear Trigger flag for PA6 (Interrupt Flag) */
    GPIO_PORTA_IM_REG    |= (1<<6);       /* Enable Interrupt on PA6 pin */
    /* Set GPIO PORTA priority as 5 by set Bit number 5, 6 and 7 with value 2 */
    NVIC_PRI7_REG = (NVIC_PRI0_REG & GPIO_PORTA_PRIORITY_MASK) | (GPIO_PORTA_INTERRUPT_PRIORITY<<GPIO_PORTA_PRIORITY_BITS_POS);
    NVIC_EN0_REG         |= 0x00000001;   /* Enable NVIC Interrupt for GPIO PORTA by set bit number 0 in EN0 Register */
}

/***************************************************************************************************************************/
void GPIO_setupPinDirectionAndMode(uint8 portNum , uint8 pinNum , GPIO_PinDirectionType direction ,uint8 pinMode,uint8 resistor)
{
    volatile uint32* port_ptr=NULL_PTR;
    switch (portNum) {
    case GPIO_PORTA:
        port_ptr =(volatile uint32*) GPIO_PORTA_BASE_ADDRESS;
        break;
    case GPIO_PORTB:
        port_ptr =(volatile uint32*) GPIO_PORTB_BASE_ADDRESS;
        break;
    case GPIO_PORTC:
        port_ptr =(volatile uint32*) GPIO_PORTC_BASE_ADDRESS;
        break;
    case GPIO_PORTD:
        port_ptr =(volatile uint32*) GPIO_PORTD_BASE_ADDRESS;
        break;
    case GPIO_PORTE:
        port_ptr =(volatile uint32*) GPIO_PORTE_BASE_ADDRESS;
        break;
    case GPIO_PORTF:
        port_ptr =(volatile uint32*) GPIO_PORTF_BASE_ADDRESS;
        break;
    default:
        break;
    }
    if( ((portNum == GPIO_PORTD) && (pinNum == PIN7_ID)) || ((portNum == GPIO_PORTF) && (pinNum == PIN0_ID)) ) /* PD7 or PF0 */
    {
        *(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_LOCK_REG_OFFSET) = 0x4C4F434B;                     /* Unlock the GPIOCR register */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_COMMIT_REG_OFFSET) , pinNum);  /* Set the corresponding bit in GPIOCR register to allow changes on this pin */
    }
    else if( (portNum ==  GPIO_PORTC) && (pinNum <= PIN3_ID) ) /* PC0 to PC3 */
    {
        /* Do Nothing ...  this is the JTAG pins */
    }
    else
    {
        /* Do Nothing ... No need to unlock the commit register for this pin */
    }
    switch(pinMode){
    case PORT_MODE_DIO : /*if the mode is DIO*/
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ANALOG_MODE_SEL_REG_OFFSET),pinNum); /* disable analog functionality for the corresponding bit */
        *(volatile uint32 *) ((volatile uint8 *)port_ptr+PORT_CTL_REG_OFFSET)&= ~((0x0F)<<(pinNum * 4)); /*clear the corresponding 4 PMCx bits in the CTL reg*/
        if(PIN_OUTPUT == direction)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

        }
        else if(PIN_INPUT == direction)
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

        }
        else
        {
            /* Do Nothing */
        }

        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ALT_FUNC_REG_OFFSET),pinNum); /* Disable alternate functions */
        if(PULL_UP == resistor)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_PULL_UP_REG_OFFSET) , pinNum);/*enable pull up resistor*/
        }
        else if(PULL_DOWN == resistor)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_PULL_DOWN_REG_OFFSET) , pinNum);/*enable pull down resistor*/

        }
        else
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_PULL_UP_REG_OFFSET) , pinNum);/*disable pull up resistor*/
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_PULL_DOWN_REG_OFFSET) , pinNum);/*disable pull down resistor*/
        }
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_DIGITAL_ENABLE_REG_OFFSET),pinNum);/*Enable digital I/O*/
        /*Set pin initial value*/
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_DATA_REG_OFFSET),pinNum);
        break;
    case PORT_MODE_ANALOG: /*If mode is analog*/
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ALT_FUNC_REG_OFFSET),pinNum); /* Enable alternate functions */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_DIGITAL_ENABLE_REG_OFFSET),pinNum);/*Disable Digital I/O*/
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ANALOG_MODE_SEL_REG_OFFSET),pinNum); /* Enable Analog functions */

        if(PIN_OUTPUT == direction)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

        }
        else if(PIN_INPUT == direction)
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

        }
        else
        {
            /* Do Nothing */
        }

        break;
    default: /*All other modes are digital */
        CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ANALOG_MODE_SEL_REG_OFFSET),pinNum); /* disable analog functionality for the corresponding bit */
        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_ALT_FUNC_REG_OFFSET),pinNum); /* Enable alternate functions */
        *(volatile uint32 *) ((volatile uint8 *)port_ptr+PORT_CTL_REG_OFFSET)&= ~((0x0F)<<(pinNum * 4)); /*clear the corresponding 4 PMCx bits in the CTL reg*/
        *(volatile uint32 *) ((volatile uint8 *)port_ptr+PORT_CTL_REG_OFFSET)|=((pinMode)<<(pinNum * 4));/*set the alternate function as the mode provided*/
        if(PORT_OUTPUT == direction)
        {
            SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Set the corresponding bit in the GPIODIR register to configure it as output pin */

        }
        else if(PORT_INPUT == direction)
        {
            CLEAR_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr + PORT_DIR_REG_OFFSET) , pinNum); /* Clear the corresponding bit in the GPIODIR register to configure it as input pin */

        }
        else
        {
            /* Do Nothing */
        }

        SET_BIT(*(volatile uint32 *)((volatile uint8 *)port_ptr+PORT_DIGITAL_ENABLE_REG_OFFSET),pinNum);/*Enable digital I/O*/
        break;

    }

}

void GPIO_writePin(uint8 pinNum ,uint8 portNum,uint8 value)
{

    volatile uint32* port_ptr = NULL_PTR;
    switch (portNum) {
    case GPIO_PORTA:
        port_ptr = &GPIO_PORTA_DATA_REG;
        break;
    case GPIO_PORTB:
        port_ptr = &GPIO_PORTB_DATA_REG;
        break;
    case GPIO_PORTC:
        port_ptr = &GPIO_PORTC_DATA_REG;
        break;
    case GPIO_PORTD:
        port_ptr = &GPIO_PORTD_DATA_REG;
        break;
    case GPIO_PORTE:
        port_ptr = &GPIO_PORTE_DATA_REG;
        break;
    case GPIO_PORTF:
        port_ptr = &GPIO_PORTF_DATA_REG;
        break;
    default:
        break;
    }

    if(port_ptr != NULL_PTR)
    {
        if(LOGIC_HIGH == value)
        {
            SET_BIT(*port_ptr,pinNum);
        }
        else
        {
            CLEAR_BIT(*port_ptr,pinNum);

        }
    }
}
uint8 GPIO_readPin(uint8 portNum , uint8 pinNum )
{
    volatile uint32* port_ptr = NULL_PTR;
    uint8 output=0;
    switch (portNum) {
    case GPIO_PORTA:
        port_ptr = &GPIO_PORTA_DATA_REG;
        break;
    case GPIO_PORTB:
        port_ptr = &GPIO_PORTB_DATA_REG;
        break;
    case GPIO_PORTC:
        port_ptr = &GPIO_PORTC_DATA_REG;
        break;
    case GPIO_PORTD:
        port_ptr = &GPIO_PORTD_DATA_REG;
        break;
    case GPIO_PORTE:
        port_ptr = &GPIO_PORTE_DATA_REG;
        break;
    case GPIO_PORTF:
        port_ptr = &GPIO_PORTF_DATA_REG;
        break;
    default:
        break;
    }
    if(GET_BIT(*port_ptr,pinNum) == LOGIC_HIGH)
    {
        output = LOGIC_HIGH;
    }
    else if(GET_BIT(*port_ptr,pinNum) == LOGIC_LOW)
    {
        output = LOGIC_LOW;
    }
    else
    {

    }
    return output;
}

void GPIO_togglePin(uint8 portNum,uint8 pinNum)
{
    volatile uint32* port_ptr = NULL_PTR;
    switch (portNum) {
    case GPIO_PORTA:
        port_ptr = &GPIO_PORTA_DATA_REG;
        break;
    case GPIO_PORTB:
        port_ptr = &GPIO_PORTB_DATA_REG;
        break;
    case GPIO_PORTC:
        port_ptr = &GPIO_PORTC_DATA_REG;
        break;
    case GPIO_PORTD:
        port_ptr = &GPIO_PORTD_DATA_REG;
        break;
    case GPIO_PORTE:
        port_ptr = &GPIO_PORTE_DATA_REG;
        break;
    case GPIO_PORTF:
        port_ptr = &GPIO_PORTF_DATA_REG;
        break;
    default:
        break;
    }
    if(port_ptr != NULL_PTR)
    {
        TOGGLE_BIT(*port_ptr,pinNum);
    }
    else
    {

    }
}
void Mcu_init(void)
{
    /* Enable clock for All PORTs and wait for clock to start */
    SYSCTL_RCGCGPIO_REG |= 0x3F;
    while(!(SYSCTL_PRGPIO_REG & 0x3F));
}
