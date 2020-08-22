/*
 * i2c_msp430.cpp
 *
 *  Created on: Aug 20, 2020
 *      Author: Kangabire
 *      These function definitions are for msp430
 */
#include <msp430.h>
#include "i2c_msp430.h"

void i2c_init() {
    // Configure GPIO
    P1OUT &= ~BIT0;             // Clear P1.0 output latch
    P1DIR |= BIT0;              // For LED
    // Configure interface ports
    P7SEL0 |=  (BIT0 | BIT1);   // Connect ports P7.0 and P7.1 to
    P7SEL1 &= ~(BIT0 | BIT1);   // eUSCI-B1, SDA and SCL, respectively
    P7DIR  &= ~(BIT0 | BIT1);   // Configure as if they are inputs
    UCB2CTLW0 = UCSWRST;        // Reset I2C interface for configuration
    UCB2CTLW0 =                 // USCI - B2 configuration register
                UCMST         | // Master mode
                UCSYNC        | // Synchronous mode
                UCMODE_3      | // I2C mode
                UCSSEL__ACLK | // Select ACLK 10kHz
                UCTR          | // Transmitter mode
                UCSWRST       | // Don't release reset (yet)
                0;
    UCB2CTLW1 =
                UCCLTO_0      | // Clock low time-out select (disabled)
                UCASTP_0      | // Automatic STOP condition generation (disabled)
                UCGLIT_0      | // Deglitch time (50ns)
                0;
    UCB2BRW = 2;               // Bit clock divider 10kHz/2 = 5kHz
    UCB2IE |= UCRXIE0; //enable interrupts
    UCB2IE |= UCTXIE0; // enable interrupts
    UCB2CTLW0 &= ~UCSWRST;      // Clear reset bit to start operation
    __delay_cycles(50);
}
void i2c_write(uint8_t reg_addr,uint8_t *tx_buffer, uint8_t quantity,uint8_t AMG88xx_ADDRESS) {
    UCB2I2CSA = AMG88xx_ADDRESS; // Set slave address
    UCB2CTLW0 |= UCTR | UCTXSTT; // Start request as transmitter
    while(!(UCB2IFG & UCTXIFG)); // Wait until TXBUFF is empty
    UCB2TXBUF = reg_addr; // Send first data byte
    while(UCB1CTLW0 & UCTXSTT); // Wait for slave's response
    if (UCB2IFG & UCNACKIFG) { // If there was no response
        UCB2IFG &= ~UCNACKIFG; // Clear NACK flag
        UCB2CTLW0 |= UCTXSTP; // Request a stop
        while (UCB2CTLW0 & UCTXSTP); // And wait until stop was actually sent
        UCB2IFG &= ~UCSTPIFG; // Clear stop flag (not actually using this..)
    }
    else { // If there was a reply from the slave,
        uint8_t j;

        for(j=0; j<quantity;j++){
            while(!(UCB2IFG & UCTXIFG));
            UCB2TXBUF = *tx_buffer++;
        }
    }
    while(!(UCB2IFG & UCTXIFG)); // Wait for the last byte to be loaded to shift register
    UCB2CTLW0 |= UCTXSTP; // Request a stop condition
    while (UCB2CTLW0 & UCTXSTP); // Wait until stop was sent
    UCB2IFG &= ~UCTXIFG; // Clear TX flag
}
void i2c_read(uint8_t reg_addr,uint8_t *rx_buffer, uint8_t quantity, uint8_t AMG88xx_ADDRESS) {
    uint8_t i;
    UCB2I2CSA = AMG88xx_ADDRESS;             // Set slave address
    UCB2CTLW0 |= UCTR | UCTXSTT;            // Start request as transmitter
    while(!(UCB2IFG & UCTXIFG));            // Wait until TXBUFF is empty
    UCB2TXBUF = reg_addr;                   // Send first data byte
    while(UCB1CTLW0 & UCTXSTT);             // Wait for slave's response
    while(!(UCB2IFG & UCTXIFG0));            // Wait for the last byte to be loaded to shift register
    UCB2CTLW0 |= UCTXSTP;                   // Request a stop condition
    while (UCB2CTLW0 & UCTXSTP);            // Wait until stop was sent
    UCB2IFG &= ~UCTXIFG;                    // Clear TX flag
    UCB2CTLW0 &= ~UCTR;                     // Put I2C bus in receive mode
    UCB2CTLW0 |= UCTXSTT;                   // Start request as a receiver

    uint8_t useless_data;
    for(i=0;i<quantity;i++) {               // Loop until data is collected
        while(!(UCB2IFG & UCRXIFG0));        // Wait until RXBUF is full
        // Generate I2C stop condition (with a nack before that, which is handled by hardware automatically)
        // Only do this for the last byte
        if(i == quantity - 1) UCB2CTL1 |= UCTXSTP;
        rx_buffer[i] = UCB2RXBUF;               // Put data into buffer
    }
    while (UCB2CTLW0 & UCTXSTP);            // Wait until stop was sent
}




