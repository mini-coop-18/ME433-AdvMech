
import matplotlib.pyplot as plt
import numpy as np
import csv

which_one = input('\n Which One Should we Plot? ')

if (which_one == "A"): 
    file_name = '/Users/gracejackson/Desktop/ME433/DSP_Project/sigA.csv'
if (which_one == "B"): 
    file_name = '/Users/gracejackson/Desktop/ME433/DSP_Project/sigB.csv'
if (which_one == "D"): 
    file_name = '/Users/gracejackson/Desktop/ME433/DSP_Project/sigD.csv'
if (which_one == "C"): ##default is the square wave
    file_name = '/Users/gracejackson/Desktop/ME433/DSP_Project/sigC.csv'


t=[]
data1 = []

with open(file_name) as f:
    # open the csv file
    print("Opening the CSV File")
    reader = csv.reader(f)
    for row in reader:
        # read the rows 1 one by one
        t.append(float(row[0])) # leftmost column
        data1.append(float(row[1])) # second column

print("Data Loaded. Sample Rt is = ", len(data1)/t[-1])
# plt.plot(t,data1)
# plt.show()

### FIR 
def FIR_Function(data1, time, weight_list, Title_list): 
    i_list = np.arange(len(weight_list), len(data1)-1-len(weight_list), 1)
    j_list = np.arange(0,len(weight_list))
    avg_list = []
    new_data1 = []
    for i in i_list:
        avg_list = []
        j = 0
        for j in j_list: #rolling average for the 
            avg_list.append(data1[i+j]*weight_list[j])
            j = j+1
        average = sum(avg_list)
        new_data1.append(average)
        i = i+1
    i_list = time[-1]/len(data1)*i_list
    plt.plot(t, data1, color = "black", label = "old")
    plt.plot(i_list, new_data1, color = "red", label = "new")
    plt.title("FIR of Signal " + str(which_one) + " with fL of "+str(Title_list[0]) +" and a bl of "+str(Title_list[1]))
    plt.legend()
    plt.show()
    return 


### IIR 
def IIR_Function(weight_a, weight_b, data1, time): 
    if (weight_a + weight_b != 1):
        print("Oh No! Check your A and B!")
        return
    new_data1 = []
    delta_list = []
    i_list = np.arange(1,len(data1)-1,1)
    for i in i_list: 
        IIR_Eq = weight_a*data1[i-1]+weight_b*data1[i]
        new_data1.append(IIR_Eq)
        delta_list.append(data1[i]-IIR_Eq)
    i_list_plot = i_list*time[-1]/len(data1) 
    plt.plot(time, data1, color = "black", label = "old")
    plt.plot(i_list_plot, new_data1, color = "red", label = "new")
    # plt.plot(delta_list)
    plt.title("IIR Average of Signal " + str(which_one) + " with A = "+str(weight_a) + " and B = "+ str(weight_b))
    plt.legend()
    plt.show()
    return

### Rolling Average 

def Rolling_Av_Function(num_rolling_average, data1, time_max, time, which_one):
    i_list = np.arange(num_rolling_average, len(data1)-1-num_rolling_average, 1)
    j_list = np.arange(0,num_rolling_average)
    avg_list = []
    new_data1 = []
    for i in i_list:
        avg_list = []
        j = 0
        for j in j_list: #rolling average for the 
            avg_list.append(data1[i+j])
            j = j+1
        average = sum(avg_list)/len(avg_list)
        new_data1.append(average)
        i = i+1
    i_list = time_max/len(data1)*i_list
    plt.plot(t, data1, color = "black", label = "old")
    plt.plot(i_list, new_data1, color = "red", label = "new")
    plt.title("Running Average of Signal " + str(which_one) + " with n of "+str(num_rolling_average))
    plt.legend()
    plt.show()
    return 

### FFT 
def FFT_Function(signal, time, which_one): 
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
    fig, (ax1, ax2) = plt.subplots(2, 1)
    ax1.set_title("Signal "+ str(which_one))
    ax1.plot(time,signal,'b')
    ax1.set_xlabel('Time')
    ax1.set_ylabel('Amplitude')
    ax2.loglog(frq,abs(Y),'b') # plotting the fft
    ax2.set_xlabel('Freq (Hz)')
    ax2.set_ylabel('|Y(freq)|')
    plt.show()  
    return  



#FFT_Function(data1, t, which_one)
#Rolling_Av_Function(20, data1, t[-1], t, which_one)
IIR_Function(.5,.5,data1,t)

h = [
    0.000000000000000000,
    0.000039971709254945,
    0.000178880516172880,
    0.000451632890391095,
    0.000902427043845815,
    0.001584341847776175,
    0.002557141709553105,
    0.003883278118543198,
    0.005622307657833765,
    0.007824175685252519,
    0.010522006098930569,
    0.013725165553637933,
    0.017413416664412075,
    0.021532929545010519,
    0.025994785235023039,
    0.030676389462698558,
    0.035425941569136428,
    0.040069799424469039,
    0.044422279219245159,
    0.048297162200910133,
    0.051519978745269569,
    0.053940027063659052,
    0.055441073513212230,
    0.055949777051524152,
    0.055441073513212237,
    0.053940027063659052,
    0.051519978745269562,
    0.048297162200910154,
    0.044422279219245166,
    0.040069799424469039,
    0.035425941569136428,
    0.030676389462698576,
    0.025994785235023042,
    0.021532929545010515,
    0.017413416664412068,
    0.013725165553637937,
    0.010522006098930574,
    0.007824175685252522,
    0.005622307657833773,
    0.003883278118543203,
    0.002557141709553106,
    0.001584341847776176,
    0.000902427043845815,
    0.000451632890391097,
    0.000178880516172882,
    0.000039971709254945,
    0.000000000000000000,
]

Title_List =  [150, 200]## Cutoff freq, BL

# FIR_Function(data1, t, h, Title_List)

