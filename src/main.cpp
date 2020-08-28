#include <msp430.h>
#include "Adafruit_AMG88xx.h"
#include <string.h>
#include "oled.h"
#include "printf.h"

//-------setting buffer to store data-----------------------
#define size 64
uint16_t buf[size];
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


	//----------OLED display --------------
	P1DIR |= 0x01;                          // Set P1.0 to output direction

	display.init();
	display.clear_off();
	display.refresh();

	//---------- draw the border-line -----------------

	display.drawFastVLineInternal(30, 0, 64, SSD1306_WHITE);
	display.drawFastVLineInternal(98, 0, 64, SSD1306_WHITE);

	display.drawFastHLineInternal(0, 0, 30, SSD1306_WHITE);
	display.drawFastHLineInternal(98, 0, 30, SSD1306_WHITE);

	display.drawFastHLineInternal(0, 63, 30, SSD1306_WHITE);
	display.drawFastHLineInternal(98, 63, 30, SSD1306_WHITE);

	display.refresh();

	uint8_t i;
	uint8_t x;
	uint8_t x_offset = 32;
	uint8_t y;
	uint8_t width = 8;
	uint8_t height = 8;
	uint8_t temp_data;
	uint8_t threshold =125;
	int pixel_on;
	float above_threshold;
	uint16_t send_raw_data;
	uint8_t send_data; // stores the last 8 bits of temp_data

	//------------ reading and displaying values from the thermal camera one by one -----------
	x = 0;
	y = 0;

	while(1){
	    thermal_camera.readPixels(buf,size);
	    for(i=0;i<size;i++){
	        //----read in data------

	        temp_data = buf[i]>>8 ;
	        //------------- find the x and y coresponding to display coordinates ---------
	        x = i>>3;
	        y = i&(0x07); // getting the last 3 values

	        y = y<<3;
	        x = x<<3;

	        above_threshold = temp_data - threshold;
	        pixel_on = (above_threshold>0)? 1:0;
	        if(pixel_on){
	            display.fill_rect(x+x_offset, y, width,height, SSD1306_WHITE);
	        }
	        else{
	            display.fill_rect(x+x_offset, y, width,height, SSD1306_BLACK);
	        }
	    }
	    //sending data over UART communication
	    uint8_t send_request;
	    send_request= UCA0RXBUF;
	    if(send_request){
	        __delay_cycles(1000);
	        for(i=0;i<size;i++){
	            send_raw_data = buf[i];
	            send_data = send_raw_data >> 8; //first 8 bits
	            io_putchar(send_data);
	        }
	    }
	    display.refresh();
	    __delay_cycles(10000);
	}

	return 0;
}
