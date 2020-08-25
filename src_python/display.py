#For Animation
import matplotlib.pyplot as plt
import matplotlib.colors as colors
import matplotlib.animation as animation
from serial_port import read_serial

def animate(i,serial_object,ax):

    # Read temperature readings from TMP102
    data = read_serial(serial_object)

    # Draw x and y lists
    ax.imshow(data)

    # Format plot
    plt.title('IR thermal camera AMG8833')

def display_data(serial_object):
    #initializing the variables for animation
    fig, ax = plt.subplots(1, 1)
    
    inter_frame = 3000 # time between frames is in milliseconds
    
    ani = animation.FuncAnimation(fig,animate,fargs=(serial_object,ax),interval=inter_frame)
    plt.show()