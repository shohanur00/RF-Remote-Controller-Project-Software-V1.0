#include "adc.h"
#include "gpio.h"


void ADC_GPIO_Init(void){
	
	//PA0,PA1,PA4,PA5
	GPIO_EnableClock(GPIOA);
	GPIO_Init(GPIOA,0,GPIO_MODE_ANALOG,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	GPIO_Init(GPIOA,1,GPIO_MODE_ANALOG,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	GPIO_Init(GPIOA,4,GPIO_MODE_ANALOG,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);
	GPIO_Init(GPIOA,5,GPIO_MODE_ANALOG,GPIO_OTYPE_PP,GPIO_NOPULL,GPIO_SPEED_LOW);

}

void ADC_Reg_Init(void){
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	//Resolution Select
	ADC1->CR1 |= (0 << ADC_CR1_RES_Pos);
	//Scan Mode 
	ADC1->CR1 |= ADC_CR1_SCAN;
	//Alignment Right
	ADC1->CR2 &= ~ADC_CR2_ALIGN;
	//End Of Conversion Selection
	ADC1->CR2 |= ADC_CR2_EOCS
	//Continuous Conversion Mode
	ADC1->CR2 |= ADC_CR2_CONT;
	//Sampling Cycle 56 cycle
	ADC1->SMPR2 |= (3U << ADC_SMPR2_SMP0_Pos);
	ADC1->SMPR2 |= (3U << ADC_SMPR2_SMP1_Pos);
	ADC1->SMPR2 |= (3U << ADC_SMPR2_SMP4_Pos);
	ADC1->SMPR2 |= (3U << ADC_SMPR2_SMP5_Pos);
	ADC->CCR |= (3U << ADC_CCR_ADCPRE_Pos);
	//ADC Enable
	ADC1->CR2 |= ADC_CR2_ADON;
	//Start Conversion
	ADC1->CR2 |= ADC_CR2_SWSTART;
}

void ADC_DMA_Init(void){
	
}

void ADC_Init(void){
	
	
}