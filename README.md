# Person-detection
In this project, I use the thermal camera Adafruit AMG8833 to record temperature data of the environement. For visualization purposes, I display these images on a low level OLED display. After acquiring this heatmap data, I send it to a computer for processing using the UART cable or wireless communication. The processing algorithm is a Convolutional Neural Network (CNN) trained to identify the shape of a person. This device could then be used to do people counting and hopefully measure the distance between individuals. These two functionalities would allow the device to check and enforce social distancing and help contain the spread of covid-19.

# Bug with my current code
So far I have been able to read in some data from the thermal camera using I2C communication on MSP430FR5994. The problem is that the thermal camera AMG8833 is supposed to return an array of 64 temperature readings. With my current I2C communication protocol code, I am reading 32 non zero temperature readings from AMG8833. The other 32 readings are zero. I think the problem might have to do with the function i2c_read() which is under the file i2c_msp430.cpp file in the src folder. 

Even though I am not reading all the data from the thermal camera, I have been able to display the non-zero temperature readings on a OLED display. 
