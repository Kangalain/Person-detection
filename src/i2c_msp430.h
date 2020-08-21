/*
 * i2c_msp430.h
 *
 *  Created on: Aug 20, 2020
 *      Author: Kangabire
 */

#ifndef I2C_MSP430_H_
#define I2C_MSP430_H_
#include <stdint.h>
void i2c_init();
void i2c_write(uint8_t reg_addr,uint8_t *tx_buffer, uint8_t quantity, uint8_t AMG88xx_ADDRESS);
void i2c_read(uint8_t reg_addr, uint8_t *rx_buffer, uint8_t quantity, uint8_t AMG88xx_ADDRESS);

#endif /* I2C_MSP430_H_ */
