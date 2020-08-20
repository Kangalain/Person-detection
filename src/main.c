#include <msp430.h> 
#include <stdint.h>
#include <string.h>
/**
 * main.c
 */
/*
//------------ UART_CABLE communication ------------------------
void uart_cable_init() {
    // uart communicaton setup
    // P2.1/UCA0RXD (to MCU from the desktop)
    // P2.0/UCA0TDX (from MCU to the desktop)
    P2SEL1 |= (BIT1 | BIT0);
    P2SEL0 &= ~(BIT1 | BIT0);

    UCA0CTLW0 |= UCSWRST;
    // Setting Baud rate! Number of symbols we can send per second
    UCA0CTLW0 |= UCSSEL_2; // CLK = SMCLK 1MHz
    UCA0BRW = 6; //UCBRx
    UCA0MCTLW |= UCOS16 | UCBRF_6 | 0x2000;
    UCA0CTLW0 &= ~UCSWRST; // Release eUSCI for operation

}
void uart_cable_write(uint8_t *data) {
    while(!(UCA0IFG & UCTXIFG)); // busy wait until transmitting or receiving interrupt is over
    UCA0TXBUF = *data;
    _delay_cycles(100000);
}

//---------------I2C protocol-----------------------
#define SLAVE_ADDRESS 0x69;
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
                UCSSEL__SMCLK | // Select SMCLK
                UCTR          | // Transmitter mode
                UCSWRST       | // Don't release reset (yet)
                0;
    UCB2CTLW1 =
                UCCLTO_0      | // Clock low time-out select (disabled)
                UCASTP_0      | // Automatic STOP condition generation (disabled)
                UCGLIT_0      | // Deglitch time (50ns)
                0;
    UCB2BRW = 10;               // Bit clock divider 1M/10 = 100kHz
    UCB2CTLW0 &= ~UCSWRST;      // Clear reset bit to start operation
    __delay_cycles(50);
}
void i2c_write(uint8_t reg_addr, uint8_t data) {
    UCB2I2CSA = SLAVE_ADDRESS; // Set slave address
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
        while(!(UCB2IFG & UCTXIFG));
        UCB2TXBUF = data;
    }
    while(!(UCB2IFG & UCTXIFG)); // Wait for the last byte to be loaded to shift register
    UCB2CTLW0 |= UCTXSTP; // Request a stop condition
    while (UCB2CTLW0 & UCTXSTP); // Wait until stop was sent
    UCB2IFG &= ~UCTXIFG; // Clear TX flag
}
void i2c_read(uint8_t reg_addr, uint8_t quantity,uint8_t *rx_buffer) {
    uint8_t i;
    UCB2I2CSA = SLAVE_ADDRESS;             // Set slave address
    UCB2CTLW0 |= UCTR | UCTXSTT;            // Start request as transmitter
    while(!(UCB2IFG & UCTXIFG));            // Wait until TXBUFF is empty
    UCB2TXBUF = reg_addr;                   // Send first data byte
    while(UCB1CTLW0 & UCTXSTT);             // Wait for slave's response
    while(!(UCB2IFG & UCTXIFG));            // Wait for the last byte to be loaded to shift register
    UCB2CTLW0 |= UCTXSTP;                   // Request a stop condition
    while (UCB2CTLW0 & UCTXSTP);            // Wait until stop was sent
    UCB2IFG &= ~UCTXIFG;                    // Clear TX flag
    UCB2CTLW0 &= ~UCTR;                     // Put I2C bus in receive mode
    UCB2CTLW0 |= UCTXSTT;                   // Start request as a receiver
    for(i=0;i<quantity;i++) {               // Loop until data is collected
        while(!(UCB2IFG & UCRXIFG));        // Wait until RXBUF is full
        // Generate I2C stop condition (with a nack before that, which is handled by hardware automatically)
        // Only do this for the last byte
        if(i == quantity - 1) UCB2CTL1 |= UCTXSTP;
        *rx_buffer++ = UCB2RXBUF;               // Put data into buffer
    }
    while (UCB2CTLW0 & UCTXSTP);            // Wait until stop was sent
}

//----------- thermal camera read buffer ---------------

int main(void){
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power

    // initializing buffer rx_buffer
    uint8_t rx_buffer[64];
    unsigned int count = 0;
    while(count<64){ //initialize rx_buffer to zero
        rx_buffer[count] = 0;
    }
    //creating a pointer to the rx_buffer
    //uint8_t *rx_buffer_pointer;
    //rx_buffer_pointer = rx_buffer;

    //read from thermal camera
    //uint8_t reg_add = 0x00;
    //i2c_init();
    //i2c_read(reg_add,64,rx_buffer_pointer);

    //--------------------- UART communication -----------------------

    count = 0;
    uint8_t data;
    uint8_t *rx_buffer_pointer;
    rx_buffer_pointer = rx_buffer;
    while(count<64){
        uart_cable_write(rx_buffer_pointer);
        rx_buffer_pointer++;
        ++count;
    }
    return 0;
}
*/
