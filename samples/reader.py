# import wave
# import struct



# with wave.open('wav_mic1.wav', 'rb') as file:
    # print("Opened file with params {}".format(str(file.getparams())))
    # length = file.getnframes()
    # for i in range(0,10):
        # waveData = file.readframes(1)
        # data = struct.unpack("<h", waveData)
        # print(int(data[0]))
    
import soundfile as sf
import matplotlib.pyplot as plt
import numpy as np
import scipy.fftpack

# # Number of samplepoints
# N = 600
# # sample spacing
# T = 1.0 / 800.0
# x = np.linspace(0.0, N*T, N)
# y = np.sin(50.0 * 2.0*np.pi*x) + 0.5*np.sin(80.0 * 2.0*np.pi*x)
# yf = scipy.fftpack.fft(y)
# xf = np.linspace(0.0, 1.0/(2.0*T), N/2)

# fig, ax = plt.subplots()
# ax.plot(xf, 2.0/N * np.abs(yf[:N//2]))
# plt.show()



data, fs = sf.read('wav_mic1.wav')
plt.plot(data)
plt.ylabel('freq: {}'.format(str(fs)))
plt.show()
