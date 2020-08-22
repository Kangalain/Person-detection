# Person-detection
In this project, I use the thermal camera Adafruit AMG8833 to record temperature data of the environement. For visualization purposes, I display these images on a low level OLED display. After acquiring this heatmap data, I send it to a computer for processing using the UART cable or wireless communication. The processing algorithm is a Convolutional Neural Network (CNN) trained to identify the shape of a person. This device could then be used to do people counting and hopefully measure the distance between individuals. These two functionalities would allow the device to check and enforce social distancing and help contain the spread of covid-19.
