#include "stm32f401xc.h"
#include "i2c1.h"
#include "rcc.h"
#include "debug.h"


volatile uint8_t dma_busy = 0;

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
    //GPIOB->OSPEEDR &= ~((3 << (6*2)) | (3 << (7*2))); // clear
    //GPIOB->OSPEEDR |=  ((2 << (6*2)) | (2 << (7*2))); // medium speed

    // 5. Enable pull-up resistors
    GPIOB->PUPDR &= ~((3 << (6*2)) | (3 << (7*2))); // clear
    GPIOB->PUPDR |=  ((1 << (6*2)) | (1 << (7*2))); // pull-up

    // 6. Set alternate function AF4 for I2C1
    GPIOB->AFR[0] &= ~((0xF << (6*4)) | (0xF << (7*4))); // clear
    GPIOB->AFR[0] |=  ((4 << (6*4)) | (4 << (7*4)));      // AF4 = I2C1
		
		
		RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    GPIOB->MODER &= ~((3<<12)|(3<<14));
    GPIOB->MODER |= ((2<<12)|(2<<14)); // AF mode
    GPIOB->OTYPER |= (1<<6)|(1<<7);    // Open-drain
    GPIOB->PUPDR &= ~((3<<12)|(3<<14));
    GPIOB->PUPDR |= ((1<<12)|(1<<14)); // Pull-up
    GPIOB->AFR[0] &= ~((0xF<<24)|(0xF<<28));
    GPIOB->AFR[0] |= ((4<<24)|(4<<28)); // AF4 = I2C1 


}

void I2C1_Reg_Init(void)
{
    // 1. Enable I2C1 clock
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // 2. Disable I2C before config
    I2C1->CR1 &= ~I2C_CR1_PE;

    // 3. Configure frequency in CR2 (in MHz)
    uint32_t pclk1_mhz = RCC_Get_APB1_Clock() / 1000000;
    I2C1->CR2 &= ~I2C_CR2_FREQ;
    I2C1->CR2 |= (pclk1_mhz << I2C_CR2_FREQ_Pos);

    // 4. Configure CCR and TRISE
    I2C1->CCR &= ~I2C_CCR_CCR_Msk;

    #ifdef I2C_SPEED_100K
    // Standard mode (100 kHz)
    {
        uint32_t ccr_value = RCC_Get_APB1_Clock() / (2 * I2C_SPEED_100K);
        I2C1->CCR |= (ccr_value << I2C_CCR_CCR_Pos);
        I2C1->TRISE = pclk1_mhz + 1; // TRISE = 1000ns
        I2C1->CCR &= ~I2C_CCR_FS;    // Standard mode
    }
    #endif

		#ifdef I2C_SPEED_400K

			uint32_t pclk1 = RCC_Get_APB1_Clock();

			uint32_t ccr_value = pclk1 / (3 * I2C_SPEED_400K);

			I2C1->CCR = 0;
			I2C1->CCR |= ccr_value;
			I2C1->CCR |= I2C_CCR_FS;      // Fast mode
			I2C1->CCR &= ~I2C_CCR_DUTY;   // Duty = 2

			I2C1->TRISE = ((pclk1 / 1000000) * 300) / 1000 + 1;

		#endif

    // 5. Enable I2C
    I2C1->CR1 |= I2C_CR1_PE;
}

// I2C1 + DMA1 Stream6 TX (STM32F401 example)
void I2C1_DMA_Init(void)
{
    // 1. Clocks Enable
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // 2. DMA Stream Disable & Reset
    DMA1_Stream6->CR &= ~DMA_SxCR_EN;
    while(DMA1_Stream6->CR & DMA_SxCR_EN);

    // 3. Peripheral Address
    DMA1_Stream6->PAR = (uint32_t)&I2C1->DR;

    // 4. DMA Control Register Configuration
    DMA1_Stream6->CR = 0; // Reset
    DMA1_Stream6->CR |= (1 << DMA_SxCR_CHSEL_Pos); // Channel 1
    DMA1_Stream6->CR |= DMA_SxCR_PL_1;             // High Priority
    DMA1_Stream6->CR |= DMA_SxCR_MINC;             // Memory Increment
    DMA1_Stream6->CR |= DMA_SxCR_DIR_0;            // Mem-to-Periph

    // --- INTERRUPT ADDITION ---
    DMA1_Stream6->CR |= DMA_SxCR_TCIE;             // Transfer Complete Interrupt Enable
    
    // 5. NVIC Configuration (Global Interrupt Enable)
    NVIC_SetPriority(DMA1_Stream6_IRQn, 0);        // Priority 0 (Highest)
    NVIC_EnableIRQ(DMA1_Stream6_IRQn);             // NVIC-?? DMA1 Stream 6 ????? ???
		__enable_irq(); // Enable Global Interrupts
    // 6. Enable I2C DMA Request
    I2C1->CR2 |= I2C_CR2_DMAEN;
}


// DMA IRQ handler
void DMA1_Stream6_IRQHandler(void)
{
    if(DMA1->HISR & DMA_HISR_TCIF6)
    {
        DMA1->HIFCR |= DMA_HIFCR_CTCIF6;

        // Disable DMA stream
        DMA1_Stream6->CR &= ~DMA_SxCR_EN;
        while(DMA1_Stream6->CR & DMA_SxCR_EN);

        // Disable I2C DMA request
        I2C1->CR2 &= ~I2C_CR2_DMAEN;

        // Wait last byte transfer
        while(!(I2C1->SR1 & I2C_SR1_BTF));

        // Send stop
        I2C1_Stop();

        // Clear busy flag
        I2C1_DMA_Busy_Clear();
    }
}


// Read DMA busy status
uint8_t I2C1_DMA_Busy_Read(void)
{
    return dma_busy;
}

// Set DMA busy
void I2C1_DMA_Busy_Set(void)
{
    dma_busy = 1;
}

// Clear DMA busy
void I2C1_DMA_Busy_Clear(void)
{
    dma_busy = 0;
}

uint8_t I2C1_Start(void)
{
    uint32_t timeout = I2C_TIMEOUT;
    while(I2C1->SR2 & I2C_SR2_BUSY)
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_BUSY_ERR;
        }
    }

		if(!(I2C1->CR1 & I2C_CR1_PE))
		{
				I2C1->CR1 |= I2C_CR1_PE;
		}
    I2C1->CR1 |= I2C_CR1_START;
    timeout = I2C_TIMEOUT;
    while(!(I2C1->SR1 & I2C_SR1_SB))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_START_ERR;
        }
    }
		
    return I2C_OK;
		
}


void I2C1_SWRST_Reset(void)
{
    I2C1->CR1 |= I2C_CR1_SWRST;
    for(volatile int i=0;i<100;i++); // small delay
    I2C1->CR1 &= ~I2C_CR1_SWRST;
    I2C1_Reg_Init();
}

void I2C1_Bus_Recovery_GPIO(void)
{
    I2C1->CR1 &= ~I2C_CR1_PE;
    GPIOB->MODER &= ~(3 << (6*2));
    GPIOB->MODER |=  (1 << (6*2));
    GPIOB->MODER &= ~(3 << (7*2));
		GPIOB->MODER |=  (1 << (7*2)); // PB7 output
    for(int i=0;i<16;i++)
    {
        GPIOB->BSRR = (1 << 6);
        for(volatile int d=0; d<200; d++);
        GPIOB->BSRR = (1 << (6+16));
        for(volatile int d=0; d<200; d++);
    }
    for(volatile int d=0; d<200; d++);
    GPIOB->BSRR = (1 << 7);          // SDA high (input is high-impedance)
    GPIOB->BSRR = (1 << 6);          // SCL high
    for(volatile int d=0; d<200; d++);
    I2C1_GPIO_Init();
    I2C1->CR1 |= I2C_CR1_PE;
}


uint8_t I2C1_Send_Addr(uint8_t addr,uint8_t rw){
		uint32_t timeout = I2C_TIMEOUT;
		I2C1->DR = (addr << 1) | (rw & 0x01);
		while(!(I2C1->SR1 & (I2C_SR1_ADDR | I2C_SR1_AF))){
			if(--timeout == 0){
				//I2C1_Recover_Bus();
				return I2C_BUSY_ERR;
			}
		}
		if(I2C1->SR1 & I2C_SR1_AF){
			I2C1->SR1 &= ~I2C_SR1_AF;
			I2C1_Stop();
			return I2C_ADDR_NACK;
		}
		
		(void)I2C1->SR1;
		(void)I2C1->SR2;
		return I2C_OK;
		
}

uint8_t I2C1_Write_Data(uint8_t data)
{
    uint32_t timeout = I2C_TIMEOUT;
    I2C1->DR = data;
    while(!(I2C1->SR1 & (I2C_SR1_TXE | I2C_SR1_AF)))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_BUSY_ERR;
        }
    }
    // Check for NACK
    if(I2C1->SR1 & I2C_SR1_AF)
    {
        I2C1->SR1 &= ~I2C_SR1_AF; // clear AF
        I2C1_Stop();
        return I2C_ADDR_NACK;
    }
		
    return I2C_OK;
}


uint8_t I2C1_Write_Multi_Data(uint8_t* data, uint16_t len)
{
    uint8_t status;

    for(uint16_t i = 0; i < len; i++)
    {
        status = I2C1_Write_Data(data[i]);
        if(status != I2C_OK)
            return status; // Stop on error

        // If last byte, wait for BTF before generating STOP
        if(i == len - 1)
        {
            uint32_t timeout = I2C_TIMEOUT;
            while(!(I2C1->SR1 & I2C_SR1_BTF))
            {
                if(--timeout == 0)
                {
                    I2C1_Recover_Bus();
                    return I2C_BUSY_ERR;
                }
            }
        }
    }

    // Generate STOP after last byte
    I2C1_Stop();

    return I2C_OK;
}




uint8_t I2C1_Read_Byte(uint8_t* data, uint8_t last_byte)
{
    uint32_t timeout = I2C_TIMEOUT;
    // Enable ACK for all bytes except last
    if(!last_byte)
        I2C1->CR1 |= I2C_CR1_ACK;
    else
        I2C1->CR1 &= ~I2C_CR1_ACK;  // NACK after last byte

    // Wait until RXNE = data received or timeout
    while(!(I2C1->SR1 & I2C_SR1_RXNE))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_BUSY_ERR;
        }
    }

		if(last_byte)
		{
				I2C1->CR1 &= ~I2C_CR1_ACK;  // NACK
				I2C1->CR1 |= I2C_CR1_STOP;  // STOP before reading DR
		}
		*data = I2C1->DR; // then read

    return I2C_OK;
}

uint8_t I2C1_Read_Multi_Data(uint8_t* buf, uint16_t len)
{
    uint8_t status;

    for(uint16_t i = 0; i < len; i++)
    {
        uint8_t last = (i == len - 1) ? 1 : 0;
        status = I2C1_Read_Byte(&buf[i], last);
        if(status != I2C_OK)
            return status;

        // Wait for BTF on last byte before STOP (optional but safer)
        if(last)
        {
            uint32_t timeout = I2C_TIMEOUT;
            while(!(I2C1->SR1 & I2C_SR1_BTF))
            {
                if(--timeout == 0)
                {
                    I2C1_Recover_Bus();
                    return I2C_BUSY_ERR;
                }
            }
        }
    }

    return I2C_OK;
}


void I2C1_Recover_Bus(void)
{
    I2C1_SWRST_Reset();

    if(I2C1->SR2 & I2C_SR2_BUSY)
    {
        I2C1_Bus_Recovery_GPIO();
    }
}

uint8_t I2C1_Stop(void)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Generate STOP condition
    I2C1->CR1 |= I2C_CR1_STOP;

    // Wait until bus is actually free or timeout occurs
    while(I2C1->SR2 & I2C_SR2_BUSY)
    {
        // Optional: also wait for BTF (Byte Transfer Finished)
        if(I2C1->SR1 & I2C_SR1_BTF)
            break;

        if(--timeout == 0)
        {
            I2C1_Recover_Bus(); // Recover from bus hang
            return 1;           // Stop failed
        }
    }

    // Small delay to ensure STOP propagates on bus
    for(volatile int i = 0; i < 100; i++);

    return I2C_OK; // success
}

uint8_t I2C1_Is_Busy(void)
{
    return (I2C1->SR2 & I2C_SR2_BUSY) ? 1 : 0;
}

uint8_t I2C1_Repeated_Start(void)
{
    uint32_t timeout = I2C_TIMEOUT;

    // Generate repeated START
    I2C1->CR1 |= I2C_CR1_START;

    while(!(I2C1->SR1 & I2C_SR1_SB))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_START_ERR;
        }
    }

    return I2C_OK;
}


uint8_t I2C1_Write_Read(uint8_t addr,
                       uint8_t* tx_buf, uint16_t tx_len,
                       uint8_t* rx_buf, uint16_t rx_len)
{
    uint8_t status;

    // 1. Start + send address (write)
    status = I2C1_Start();
    if(status != I2C_OK) return status;

    status = I2C1_Send_Addr(addr, 0);
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 2. Send data
    status = I2C1_Write_Multi_Data(tx_buf, tx_len);
    if(status != I2C_OK) return status;

    // 3. Repeated Start + send address (read)
    status = I2C1_Repeated_Start();
    if(status != I2C_OK) return status;

    status = I2C1_Send_Addr(addr, 1);
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 4. Read data
    status = I2C1_Read_Multi_Data(rx_buf, rx_len);
    if(status != I2C_OK) return status;

    // 5. Stop already handled in Read_Multi_Data for last byte
    return I2C_OK;
}

uint8_t I2C1_Wait_TXE(void)
{
    uint32_t timeout = I2C_TIMEOUT;
    while(!(I2C1->SR1 & I2C_SR1_TXE))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_BUSY_ERR;
        }
    }
    return I2C_OK;
}

uint8_t I2C1_Wait_RXNE(void)
{
    uint32_t timeout = I2C_TIMEOUT;
    while(!(I2C1->SR1 & I2C_SR1_RXNE))
    {
        if(--timeout == 0)
        {
            I2C1_Recover_Bus();
            return I2C_BUSY_ERR;
        }
    }
    return I2C_OK;
}


uint8_t I2C1_Read_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len)
{
    uint8_t status;

    // 1?? Start + send slave address (write) to select register
    status = I2C1_Start();
    if(status != I2C_OK) return status;

    status = I2C1_Send_Addr(dev_addr, 0); // write mode
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 2?? Send register address
    status = I2C1_Write_Data(reg_addr);
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 3?? Repeated start + send slave address (read)
    status = I2C1_Repeated_Start();
    if(status != I2C_OK) return status;

    status = I2C1_Send_Addr(dev_addr, 1); // read mode
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 4?? Read data
    status = I2C1_Read_Multi_Data(data, len);
    if(status != I2C_OK) return status;

    // STOP already generated in Read_Multi_Data last byte
    return I2C_OK;
}

uint8_t I2C1_Write_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len)
{
    uint8_t status;

    // 1?? Start + send slave address (write)
    status = I2C1_Start();
    if(status != I2C_OK) return status;

    status = I2C1_Send_Addr(dev_addr, 0); // write mode
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 2?? Send register address
    status = I2C1_Write_Data(reg_addr);
    if(status != I2C_OK) { I2C1_Stop(); return status; }

    // 3?? Send data bytes
    for(uint16_t i=0; i<len; i++)
    {
        status = I2C1_Write_Data(data[i]);
        if(status != I2C_OK) { I2C1_Stop(); return status; }
    }

    // 4?? Generate STOP
    I2C1_Stop();

    return I2C_OK;
}

void I2C1_Scan_Bus(void)
{
    Debug_Tx_Text("Scanning I2C bus...\n");
    for(uint8_t addr=0x03; addr<=0x77; addr++)
    {
        if(I2C1_Start()==0)
        {
            if(I2C1_Send_Addr(addr,0)==0)
                Debug_Tx_Parameter_Hex_NL("Found device at: ", addr);
        }
    }
    Debug_Tx_Text("Scan complete.\n");
}

void I2C1_Init(void){
	
	I2C1_GPIO_Init();
	I2C1_DMA_Init();
	I2C1_Reg_Init();
	
	#ifdef DEBUG_ENABLE
		Debug_Init(115200);
	#endif
	
}