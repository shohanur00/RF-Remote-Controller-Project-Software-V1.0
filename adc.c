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

void ADC_Reg_Init(void)
{
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 12-bit resolution (default)
    ADC1->CR1 &= ~ADC_CR1_RES;

    // Right alignment
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // Sampling time
    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP0_Pos);
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP1_Pos);
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP4_Pos);
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP5_Pos);

    // ADC clock prescaler
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= (1 << ADC_CCR_ADCPRE_Pos); // PCLK2 / 4

    // Single conversion
    ADC1->SQR1 = 0;

    // Default channel order
    ADC1->SQR3 = 0;

    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;
}

void ADC_DMA_Init(void){
	
}

void ADC_Init(void){
	ADC_GPIO_Init();
	ADC_Reg_Init();
}


uint16_t ADC_Read_Channel(uint8_t channel)
{
    // Clear previous channel selection
    ADC1->SQR3 = 0;

    // Set channel as 1st conversion
    ADC1->SQR3 |= (channel << ADC_SQR3_SQ1_Pos);

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;

    // Wait for conversion complete
    while(!(ADC1->SR & ADC_SR_EOC));

    // Read ADC value
    return (uint16_t)ADC1->DR;
}

void ADC_Read_Multi(uint8_t *channels, uint16_t *results, uint8_t count)
{
    for(uint8_t i = 0; i < count; i++)
    {
        // Select channel as first conversion
        ADC1->SQR3 = 0;
        ADC1->SQR3 |= (channels[i] << ADC_SQR3_SQ1_Pos);

        // Start conversion
        ADC1->CR2 |= ADC_CR2_SWSTART;

        // Wait for conversion complete
        while(!(ADC1->SR & ADC_SR_EOC));

        // Store result
        results[i] = (uint16_t)ADC1->DR;
    }
}