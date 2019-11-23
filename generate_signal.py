
import math
import numpy as np
import matplotlib.pyplot as plt

samples = 128
window = np.hamming(samples)
PI = 3.14159
toRad = PI / 180
freq = 1000
sampFreq = 16000
period = 1 / sampFreq
fspan = sampFreq / samples
timespan = []
freqspan = []
signal1 = []
signal2 = []
signal3 = []
signal4 = []
phase1 = 0 * toRad
phase2 = 5 * toRad
phase3 = 10 * toRad
phase4 = 15 * toRad


def toADC(value):
    return int(round((value * 511) + 512,0))
    # return value
    
for i in range(samples):
    timespan.append(i*period)
    freqspan.append(i*fspan)

for i in range(samples):
    signal1.append(toADC(math.cos((2*PI*freq*timespan[i]) + phase1 )))
    signal2.append(toADC(math.cos((2*PI*freq*timespan[i]) + phase2 )))
    signal3.append(toADC(math.cos((2*PI*freq*timespan[i]) + phase3 )))
    signal4.append(toADC(math.cos((2*PI*freq*timespan[i]) + phase4 )))
    
fft1 = np.fft.fft(signal1)
fft2 = np.fft.fft(signal2)
fft3 = np.fft.fft(signal3)
fft4 = np.fft.fft(signal4)
    
# for i in timespan:
    # print( round(i,6), end='\t')
    # print( toADC(math.cos((2*PI*freq*i) + phase1 )),  end='\t')
    # print( toADC(math.cos((2*PI*freq*i) + phase2 )),  end='\t')
    # print( toADC(math.cos((2*PI*freq*i) + phase3 )),  end='\t')
    # print( toADC(math.cos((2*PI*freq*i) + phase4 )))
    # print(i)
    
for i in range(samples):
    print(signal1[i])
    print(signal2[i])
    print(signal3[i])
    print(signal4[i])
    
# plt.plot(signal1)
# plt.show()

# for i in range(samples):
    # print("{}".format(signal1[i]))
    
freqSignal = np.fft.fftfreq(len(signal1), timespan[1] - timespan[0])

# plt.plot( freqSignal, np.abs(fft1) )
# plt.show()
# print (fft1)
# print (np.abs(fft1))

# index = np.argmax(np.abs(fft1))
# angle = np.angle(fft1, deg=True)
# print ("Index is {} Angle is {}".format(index, angle[index]))
# index = np.argmax(np.abs(fft2))
# angle = np.angle(fft2, deg=True)
# print ("Index is {} Angle is {}".format(index, angle[index]))
# index = np.argmax(np.abs(fft3))
# angle = np.angle(fft3, deg=True)
# print ("Index is {} Angle is {}".format(index, angle[index]))
# index = np.argmax(np.abs(fft4))
# angle = np.angle(fft4, deg=True)
# print ("Index is {} Angle is {}".format(index, angle[index]))
