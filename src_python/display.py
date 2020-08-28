#For Animation
from matplotlib import pyplot as plt
from matplotlib import colors as colors
from matplotlib import animation as animation
from serial_port import read_serial
import numpy as np

def animate(i,serial_object,ax):

    # Read temperature readings from TMP102
    data = read_serial(serial_object)
    n_rows_tot = data.shape[0] - 1
    n_columns_tot = data.shape[1] -1

    # fill in the zeros
    indices = np.array(np.where(data==0))
    row_indices = indices[0]
    column_indices = indices[1]
    n_zero_data = row_indices.size
    

    for ii in range(n_zero_data):
        if row_indices[ii]< n_rows_tot:
            x1 = row_indices[ii]
            x2 = row_indices[ii] + 1

        else:
            x1 = row_indices[ii]-1
            x2 = row_indices[ii]
        
        if column_indices[ii]< n_columns_tot:
            y1 = column_indices[ii]
            y2 = column_indices[ii] + 1

        else:
            y1 = column_indices[ii]-1
            y2 = column_indices[ii]
        
        average = data[[x1,y1],[x2,y2]]
        n_positive = np.count_nonzero(average)
        average = sum(average)
        average = average/n_positive

        data[row_indices[ii],column_indices[ii]] = average

    # Draw x and y lists
    vmin = 28
    vmax = 30
    ax.imshow(data) #,norm=colors.Normalize(vmin=vmin, vmax=vmax))
    
    # Format plot
    plt.title('IR thermal camera AMG8833')

def display_data(serial_object):
    #initializing the variables for animation
    fig, ax = plt.subplots(1, 1)
    
    inter_frame = 300 # time between frames is in milliseconds
    
    ani = animation.FuncAnimation(fig,animate,fargs=(serial_object,ax),interval=inter_frame)
    plt.hot()
    plt.show()