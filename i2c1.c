#include "stm32f401xc.h"
#include "i2c1.h"
#include "rcc.h"


#define I2C_TIMEOUT 100000

void I2C1_GPIO_Init(void)
{
    // 1. Enable GPIOB clock
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

    // 2. Set PB6, PB7 to alternate function mode (MODER = 10)
    GPIOB->MODER &= ~((3 << (6*2)) | (3 << (7*2))); // clear
    GPIOB->MODER |=  ((2 << (6*2)) | (2 << (7*2))); // AF mode

    // 3. Set output type to open-drain
    GPIOB->OTYPER |= (1 << 6) | (1 << 7);

    // 4. Set speed (Medium)
    GPIOB->OSPEEDR &= ~((3 << (6*2)) | (3 << (7*2))); // clear
    GPIOB->OSPEEDR |=  ((2 << (6*2)) | (2 << (7*2))); // medium speed

    // 5. Enable pull-up resistors
    GPIOB->PUPDR &= ~((3 << (6*2)) | (3 << (7*2))); // clear
    GPIOB->PUPDR |=  ((1 << (6*2)) | (1 << (7*2))); // pull-up

    // 6. Set alternate function AF4 for I2C1
    GPIOB->AFR[0] &= ~((0xF << (6*4)) | (0xF << (7*4))); // clear
    GPIOB->AFR[0] |=  ((4 << (6*4)) | (4 << (7*4)));      // AF4 = I2C1
		
}

void I2C1_Reg_Init(void){
	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;
	I2C1->CR1 &= ~I2C_CR1_PE;
	I2C1->CR2 &= ~I2C_CR2_FREQ;
	I2C1->CR2 |= ((RCC_Get_APB1_Clock()/1000000) << I2C_CR2_FREQ_Pos);
	I2C1->CCR &= ~I2C_CCR_CCR_Msk;
	uint32_t ccr_value = RCC_Get_APB1_Clock()/(2*100000);
	I2C1->CCR |= (ccr_value << I2C_CCR_CCR_Pos);				//100Khz Speed
	I2C1->TRISE = (RCC_Get_APB1_Clock()/1000000) + 1;		//TRISE =  1000ns
	I2C1->CR1 |= I2C_CR1_ACK;
  I2C1->CR1 |= I2C_CR1_PE;
}

uint8_t I2C1_Start(void){
	uint32_t timeout;
	I2C1->CR1 |= I2C_CR1_START;
	timeout = I2C_TIMEOUT;
	
	while(!(I2C1->SR1 & I2C_SR1_SB))
	{
			if(--timeout == 0) return 1; // timeout
	}

	return 0;
}


void I2C1_Init(void){
	
	I2C1_GPIO_Init();
	I2C1_Reg_Init();
}