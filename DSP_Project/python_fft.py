import matplotlib.pyplot as plt
import numpy as np
import csv

#%% FFT Function

def FFT_Function(signal, time): 
    # Signal = Data List; Time = Time List
    Fs = len(signal)/time[-1]
    Ts = 1.0/Fs; # sampling interval 
    print("Ts = ", Ts)
    ts = np.arange(0,time[-1],Ts) # time vector
    print("Length of ts is = ", len(ts))
    y = signal # the data to make the fft from
    n = len(y) # length of the signal
    k = np.arange(n) 
    T = n/Fs
    frq = k/T # two sides frequency range
    frq = frq[range(int(n/2))] # one side frequency range
    print("length of frq = ", len(frq))
    Y = np.fft.fft(y)/n # fft computing and normalization
    Y = Y[range(int(n/2))]
    print("length of Y =", len(Y))
    return Y, frq

#%%

t = []
data1 = []

with open('/Users/gracejackson/Desktop/ME433/DSP_Project/sigA.csv') as f:
    # open the csv file
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

FFT_Transformation = []
FRQ = []

FFT_Transformation, FRQ = FFT_Function(data1, t)
    
fig, (ax1, ax2) = plt.subplots(2, 1)
ax1.set_title("Signal A")
ax1.plot(t,data1,'b')
ax1.set_xlabel('Time')
ax1.set_ylabel('Amplitude')
ax2.loglog(FRQ,abs(FFT_Transformation),'b') # plotting the fft
ax2.set_xlabel('Freq (Hz)')
ax2.set_ylabel('|Y(freq)|')
plt.show()