#ifndef I2C1_H
#define I2C1_H

#include "stm32f401xc.h"
#include <stdint.h>

// -------------------------
// I2C1 Status Codes
// -------------------------
#define I2C_OK            0
#define I2C_BUSY_ERR      1
#define I2C_START_ERR     2
#define I2C_ADDR_NACK     3

#define I2C_TIMEOUT       100000

#define DEBUG_ENABLE

// -------------------------
// Initialization
// -------------------------
void I2C1_GPIO_Init(void);
void I2C1_Reg_Init(void);
void I2C1_Init(void);

// -------------------------
// Bus control / Recovery
// -------------------------
uint8_t I2C1_Start(void);
uint8_t I2C1_Stop(void);
void I2C1_SWRST_Reset(void);
void I2C1_Bus_Recovery_GPIO(void);
void I2C1_Recover_Bus(void);
uint8_t I2C1_Is_Busy(void);
uint8_t I2C1_Repeated_Start(void);

// -------------------------
// Address / Data
// -------------------------
uint8_t I2C1_Send_Addr(uint8_t addr, uint8_t rw);
uint8_t I2C1_Write_Data(uint8_t data);
uint8_t I2C1_Write_Multi_Data(uint8_t* data, uint16_t len);
uint8_t I2C1_Read_Byte(uint8_t* data, uint8_t last_byte);
uint8_t I2C1_Read_Multi_Data(uint8_t* buf, uint16_t len);
uint8_t I2C1_Write_Read(uint8_t addr,
                        uint8_t* tx_buf, uint16_t tx_len,
                        uint8_t* rx_buf, uint16_t rx_len);
uint8_t I2C1_Wait_TXE(void);
uint8_t I2C1_Wait_RXNE(void);

// -------------------------
// Register access helpers
// -------------------------
uint8_t I2C1_Read_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len);
uint8_t I2C1_Write_Register(uint8_t dev_addr, uint8_t reg_addr, uint8_t* data, uint16_t len);

// -------------------------
// Utility
// -------------------------
void I2C1_Scan_Bus(void);

#endif // I2C1_H