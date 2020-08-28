import serial
import numpy as np

def init_serial(serial_name,baud_rate):
    try:
        mspserial = serial.Serial(serial_name,baud_rate) #set baud rate to 9600
    except:
        print("Failed to connect. Check your port name")
        exit()
    return mspserial

def close_serial(serial_object):
    #closes a serial port
    serial_object.close()

def open_serial():
    serial_name = 'COM4'
    baud_rate = 9600
    serial_object = init_serial(serial_name,baud_rate)
    return serial_object

def read_serial(serial_object):
    # reads from msp430 serial
    serial_object.write(b'1')   
    #read data
    IR_conversion_factor = 0.25
    thermal_size = 64
    thermal_data = []
    count = 0

    while count<thermal_size:
        data_temp = serial_object.read()
        data_temp = IR_conversion_factor*ord(data_temp)
        thermal_data.append(data_temp)
        count +=1
       
    thermal_data = np.array(thermal_data)
    
    thermal_data = thermal_data.reshape((8,-1))

    return thermal_data