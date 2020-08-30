#For Animation
from matplotlib import pyplot as plt
from matplotlib import colors as colors
from matplotlib import animation as animation
from matplotlib import cm
from serial_port import read_serial
import numpy as np

def animate(i,serial_object,ax,fig):

    # Read temperature readings from TMP102
    data = read_serial(serial_object)
    # Draw x and y lists

    ax.cla() #clear the axis
    #ax.imshow(data,cmap=cm.binary) #,norm=colors.Normalize(vmin=vmin, vmax=vmax))
    ax.stem(data, use_line_collection=True)
    
    # Format plot
    plt.title('IR thermal camera AMG8833')

def display_data(serial_object):
    #initializing the variables for animation
    fig, ax = plt.subplots(1, 1)
    
    inter_frame = 2000 # time between frames is in milliseconds
    
    ani = animation.FuncAnimation(fig,animate,fargs=(serial_object,ax,fig),interval=inter_frame)
    plt.show()