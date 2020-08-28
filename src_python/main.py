from display import display_data
from serial_port import open_serial, close_serial,read_serial

if __name__ == '__main__':
    # Connect to serial port first
    # Make sure to replace this with YOUR MSP430s serial port
    
    #PongApp().run()
    #initializing the serial port
    
    mspserial = open_serial()
    
    display_data(mspserial)
    
    close_serial(mspserial)
      