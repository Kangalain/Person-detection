#include <msp430.h>
#include "Adafruit_AMG88xx.h"
#include <string.h>
#include "oled.h"

void printf(char *, ...);

// The I/O backend library
int io_putchar(int c) {
    while (!(UCA0IFG&UCTXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = c;
    return 0;
}

int io_puts_no_newline(const char *str) {
  uint8_t len_str = strlen(str);
  volatile uint8_t i;
  for(i=0;i<len_str;i++) {
    while (!(UCA0IFG&UCTXIFG));                // USCI_A0 TX buffer ready?
    UCA0TXBUF = str[i];
  }
  return 0;
}

void init_clocks(){
    // Startup clock system with max DCO setting ~8MHz
    CSCTL0_H = CSKEY_H;                     // Unlock CS registers
    CSCTL1 = DCOFSEL_3 | DCORSEL;           // Set DCO to 8MHz
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;   // Set all dividers
    CSCTL0_H = 0;                           // Lock CS registers
}

void init_uart() {
    // Pins, 2.1/UCA0RXD (to MCU), 2.0/UCA0TXD (from MCU)
    P2REN = 0;
    P2SEL1 |= (BIT1 | BIT0);
    P2SEL0 &= ~(BIT1 | BIT0);

    // Configure USCI_A0 for UART mode
    UCA0CTLW0 = UCSWRST;                    // Put eUSCI in reset
    UCA0CTLW0 |= UCSSEL_2;             // CLK = SMCLK
    // Baud Rate calculation
    // 8000000/(16*9600) = 52.083
    // Fractional portion = 0.083
    // User's Guide Table 21-4: UCBRSx = 0x04
    // UCBRFx = int ( (52.083-52)*16) = 1
    UCA0BRW = 52;                           // 8000000/16/9600
    //UCA0BR1 = 0x00;
    UCA0MCTLW |= UCOS16 | UCBRF_1 | 0x4900;
    UCA0CTLW0 &= ~UCSWRST;                  // Initialize eUSCI
}

//-------setting buffer to store data-----------------------
#define size 64
uint8_t buf[size];
SSD1306 display;

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer
	PM5CTL0 &= ~LOCKLPM5; // Disable GPIO power
	//-------------- UART communication-------------------
	init_clocks();
	init_uart();
	//--------------Thermal camera-------------------
	Adafruit_AMG88xx thermal_camera = Adafruit_AMG88xx();

	thermal_camera.begin(); //initializing registers and setting upt communication protocol

	//read the entire temperature heatmap values
	memset(buf,0,size);

	thermal_camera.readPixels(buf,size);

	//----------OLED display --------------
	P1DIR |= 0x01;                          // Set P1.0 to output direction

	display.init();
	display.clear_off();
	display.refresh();

	//---------- draw the border-line -----------------
	display.set_xy(0, 0, SSD1306_WHITE);
	display.set_xy(0, 63, SSD1306_WHITE);
	display.set_xy(127, 0, SSD1306_WHITE);
	display.set_xy(126, 63, SSD1306_WHITE);

	display.refresh();

	uint8_t i;
	uint8_t x;
	uint8_t y;
	uint8_t width = 2;
	uint8_t height = 8;
	uint8_t temp_data=0;
	uint8_t threshold = 10;
	uint8_t row_odd = 0;
	uint8_t column_odd = 0;

	//------------ Displaying values from the thermal camera one by one -----------
	x = 0;
	y = 0;


	for(i=0;i<size;i++){
	    temp_data = buf[i];
	    //------getting the digits of temp_data-----------
	    x = i<<1;
	    height = temp_data>>1;

	    display.fill_rect(x, y, width,height, SSD1306_WHITE);

	}

	/*
	for(i=0;i<size;i++){
	    //----read in data------
	    temp_data = buf[i];

	    //------------- find the x and y coresponding to display coordinates ---------
	    y = i>>3;
	    x = i&(0x07); // getting the last 3 values
	    //row_odd = y&(0x01);
	    //column_odd = x&(0x01);

	    y = y<<3;
	    x = x<<4;


	    if(row_odd){
	        if(column_odd==0){
	            display.fill_rect(x, y, width,height, SSD1306_WHITE);
	        }
	        else{
	            display.fill_rect(x, y, width,height, SSD1306_BLACK);
	        }
	    }
	    else{
	        if(column_odd!=0){
	            display.fill_rect(x, y, width,height, SSD1306_WHITE);
	        }
	        else{
	            display.fill_rect(x, y, width,height, SSD1306_BLACK);
	        }
	    }
	}
	*/
	display.refresh();

	return 0;
}
