#include <msp430.h>
#include "Adafruit_AMG88xx.h"
#include <string.h>
#include "oled.h"
#include "printf.h"

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
	uint8_t width = 16;
	uint8_t height = 8;
	uint8_t temp_data=0;
	uint8_t threshold = 102;
	uint8_t row_odd = 0;
	uint8_t column_odd = 0;

	//------------ reading and displaying values from the thermal camera one by one -----------
	x = 0;
	y = 0;

	while(1){
	    thermal_camera.readPixels(buf,size);
	    for(i=0;i<size;i++){
	        //----read in data------

	        temp_data = buf[i];
	        //------------- find the x and y coresponding to display coordinates ---------
	        y = i>>3;
	        x = i&(0x07); // getting the last 3 values

	        y = y<<3;
	        x = x<<4;

	        if(temp_data>threshold){
	            display.fill_rect(x, y, width,height, SSD1306_WHITE);
	        }
	        else{
	            display.fill_rect(x, y, width,height, SSD1306_BLACK);
	        }

	    }
	    display.refresh();
	    __delay_cycles(10000);
	}

	return 0;
}
