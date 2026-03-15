#include "adc.h"
#include "gpio.h"

#define ADC_BUFFER_SIZE	4
#define DMA_ENABLE

uint16_t ADC_Buffer[ADC_BUFFER_SIZE];   // 4 axis (2 joystick)

typedef struct {
    uint16_t ch[ADC_BUFFER_SIZE];   // general-purpose array for ADC channels
} ADC_RawValues;



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
    // Enable ADC1 clock
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // 12-bit resolution (default)
    ADC1->CR1 &= ~ADC_CR1_RES;

    // Right alignment
    ADC1->CR2 &= ~ADC_CR2_ALIGN;

    // Sampling time
    ADC1->SMPR2 = 0;
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP0_Pos); // CH0
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP1_Pos); // CH1
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP4_Pos); // CH4
    ADC1->SMPR2 |= (3 << ADC_SMPR2_SMP5_Pos); // CH5

    // ADC clock prescaler PCLK2 / 4
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= (1 << ADC_CCR_ADCPRE_Pos);

    // Single conversion mode (optional, for non-DMA use)
    ADC1->CR1 &= ~ADC_CR1_SCAN;
    ADC1->CR2 &= ~ADC_CR2_CONT;

    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;
}



void ADC_DMA_Init(void)
{
    // 1?? Enable clocks
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    // ---------------- DMA CONFIG ----------------
    DMA2_Stream0->CR &= ~DMA_SxCR_EN;
    while(DMA2_Stream0->CR & DMA_SxCR_EN); // wait disabled

    DMA2_Stream0->PAR  = (uint32_t)&ADC1->DR;   // Peripheral address
    DMA2_Stream0->M0AR = (uint32_t)ADC_Buffer;  // Memory buffer
    DMA2_Stream0->NDTR = ADC_BUFFER_SIZE;     // Number of conversions

    DMA2_Stream0->CR = 0;
    DMA2_Stream0->CR |= (0 << DMA_SxCR_CHSEL_Pos); // Channel 0 (ADC1)
    DMA2_Stream0->CR |= DMA_SxCR_MINC;             // Memory increment
    DMA2_Stream0->CR |= DMA_SxCR_CIRC;             // Circular mode
    DMA2_Stream0->CR |= (1 << DMA_SxCR_MSIZE_Pos); // 16-bit memory
    DMA2_Stream0->CR |= (1 << DMA_SxCR_PSIZE_Pos); // 16-bit peripheral
    DMA2_Stream0->CR |= (2 << DMA_SxCR_PL_Pos);    // High priority
    DMA2_Stream0->CR |= DMA_SxCR_EN;

    // ---------------- ADC CONFIG ----------------
    ADC1->CR1 = 0;
    ADC1->CR1 |= ADC_CR1_SCAN;  // Enable scan mode (multi-channel)

    ADC1->CR2 = 0;
    ADC1->CR2 |= ADC_CR2_CONT;  // Continuous conversion
    ADC1->CR2 |= ADC_CR2_DMA;   // Enable DMA
    ADC1->CR2 |= ADC_CR2_DDS;   // Continuous DMA request

    // Sampling time already configured in ADC_Reg_Init() if called

    // Number of conversions = 4
    ADC1->SQR1 &= ~ADC_SQR1_L;
    ADC1->SQR1 |= (ADC_BUFFER_SIZE - 1) << 20;

    // Channel sequence
    ADC1->SQR3 = 0;
    ADC1->SQR3 |= (0 << 0);   // 1st ? CH0
    ADC1->SQR3 |= (1 << 5);   // 2nd ? CH1
    ADC1->SQR3 |= (4 << 10);  // 3rd ? CH4
    ADC1->SQR3 |= (5 << 15);  // 4th ? CH5

    // ADC prescaler
    ADC->CCR &= ~ADC_CCR_ADCPRE;
    ADC->CCR |= (1 << ADC_CCR_ADCPRE_Pos);

    // Enable ADC
    ADC1->CR2 |= ADC_CR2_ADON;

    // Start conversion
    ADC1->CR2 |= ADC_CR2_SWSTART;
}




uint16_t ADC_Read_Channel(uint8_t channel)
{
    #ifndef DMA_ENABLE
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
		#endif
		#ifdef DMA_ENABLE
		if(channel > ADC_BUFFER_SIZE) return 0;   // adjust 4 ? NUM_ADC_CHANNELS
    return ADC_Buffer[channel];
		#endif
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


void ADC_Init(void){
		ADC_GPIO_Init();
		ADC_Reg_Init();
		#ifdef DMA_ENABLE
		ADC_DMA_Init();
		#endif
}
