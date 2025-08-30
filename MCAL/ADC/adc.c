/*
 * adc.c
 *
 *  Created on: Aug 2, 2024
 *      Author: Heba Maher
 */


#include "adc.h"
#include "tm4c123gh6pm_registers.h"

void GPIO_SetupADCPins(void){
    /*
     PE3 -> Analog-to-digital converter input 0.
     PE2 -> Analog-to-digital converter input 1.
     PE1 -> Analog-to-digital converter input 2.
     PE0 -> Analog-to-digital converter input 3.
     PD3 -> Analog-to-digital converter input 4.
     PD2 -> Analog-to-digital converter input 5.
     PD1 -> Analog-to-digital converter input 6.
     PD0 -> Analog-to-digital converter input 7.
     PE5 -> Analog-to-digital converter input 8.
     */

    SYSCTL_RCGCGPIO_REG  |= 0x10;         /* Enable clock for GPIO PORTE */
    while(!(SYSCTL_PRGPIO_REG & 0x10));   /* Wait until GPIO PORTE clock is activated and it is ready for access */

    GPIO_PORTE_AMSEL_REG |= 0x3F;         /* Enable Analog on PE0 & PE1 & PE2 & PE3 & PE4 & PE5 */
    GPIO_PORTE_DIR_REG   &= 0xC0;         /* Configure PE0 & PE1 & PE2 & PE3 & PE4 & PE5 as input pins */
    GPIO_PORTE_AFSEL_REG |= 0x3F;         /* Enable alternative function on PE0 & PE1 & PE2 & PE3 & PE4 & PE5 */
    /* Clear PMCx bits for PE0 & PE1 & PE2 & PE3 & PE4 & PE5 with value 0 to use PE0 & PE1 & PE2 & PE3 & PE4 & PE5 as ADC Channels */
    GPIO_PORTE_PCTL_REG  &= 0xFF000000;
    GPIO_PORTE_DEN_REG   &= 0xC0;         /* Disable Digital I/O on PE0 & PE1 & PE2 & PE3 & PE5 */

    /*******************************************************************************/

//    SYSCTL_RCGCGPIO_REG  |= 0x08;         /* Enable clock for GPIO PORTD */
//    while(!(SYSCTL_PRGPIO_REG & 0x08));   /* Wait until GPIO PORTD clock is activated and it is ready for access */
//
//    GPIO_PORTD_AMSEL_REG |= 0x0F;         /* Enable Analog on PD0 & PD1 & PD2 & PD3 */
//    GPIO_PORTD_DIR_REG   &= 0xF0;         /* Configure PD0 & PD1 & PD2 & PD3 as input pins */
//    GPIO_PORTD_AFSEL_REG |= 0x0F;         /* Enable alternative function on PD0 & PD1 & PD2 & PD3 */
//    /* Clear PMCx bits for PD0 & PD1 & PD2 & PD3 with value 0 to use PD0 & PD1 & PD2 & PD3 as ADC Channels */
//    GPIO_PORTD_PCTL_REG  &= 0xFFFF0000;
//    GPIO_PORTD_DEN_REG   &= 0xF0;         /* Disable Digital I/O on PD0 & PD1 & PD2 & PD3 */

    /*******************************************************************************/

    SYSCTL_RCGCGPIO_REG  |= 0x02;         /* Enable clock for GPIO PORTB */
    while(!(SYSCTL_PRGPIO_REG & 0x02));   /* Wait until GPIO PORTB clock is activated and it is ready for access */

    GPIO_PORTB_AMSEL_REG |= 0x30;         /* Enable Analog on PB4 & PB5 */
    GPIO_PORTB_DIR_REG   &= 0xCF;         /* Configure PB4 & PB5 as input pins */
    GPIO_PORTB_AFSEL_REG |= 0x30;         /* Enable alternative function on PB4 & PB5 */
    /* Clear PMCx bits for PB4 & PB5 with value 0 to use PB4 & PB5 as ADC Channels */
    GPIO_PORTB_PCTL_REG  &= 0xFF00FFFF;
    GPIO_PORTB_DEN_REG   &= 0xCF;         /* Disable Digital I/O on PB4 & PB5 */


}
void ADC0_Init(){

    GPIO_SetupADCPins();

    SYSCTL_RCGCADC_REG |= 0x01;  /* Enable ADC0 Clock */
    while(!(SYSCTL_RCGCADC_REG & 0x01)); /* Wait until ADC0 clock is activated and it is ready for access */
}

uint32 ADC0_ReadChannel(uint8 channel)
{

    /* Initialize the sequencer */
    ADC0_ADCACTSS_REG     = (0x00000000);         /* De-activate all the sample sequencers */
    ADC0_ADCSSMUX0_REG    = channel;              /* the first sequencer of sequencer0 is for AIN1 */
    ADC0_ADCSSCTL0_REG   |= 0x6;                  /* The first sequencer is the last sequencer - Enable interrupt */
    ADC0_ADCACTSS_REG    |=(1<<0);                /* Enable the sample sequencer logic for sequencer 0*/

    /* Start Conversion */
    ADC0_ADCPSSI_REG = (ADC0_ADCPSSI_REG & 0xFFFFFFF0 ) | ( (1<<0) );

    /* Check on the flag till the conversation is done */
    while(!(ADC0_ADCRIS_REG & 0x00000001));

    /* Clear the flag */
    ADC0_ADCISC_REG |= 0x00000001;

    /* return the data */
    return (uint32) (ADC0_ADCSSFIFO0_REG);
}
