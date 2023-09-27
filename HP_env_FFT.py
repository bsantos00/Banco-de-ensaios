import os
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
from scipy.signal import butter, filtfilt, hilbert
from scipy.fft import fft, fftfreq


font = {'family' : 'normal',
        'weight' : 'normal',
        'size'   : 16}
matplotlib.rc('font', **font)

# Parametros HPF Butterworth
cutoff_freq = 5 # Frequência de corte
order = 3  # Ordem do HPF


sampling_freq = 800  # Taxa da amostragem em HZ«z

#accesso ao ficheiro de texto
folder_path = "C:/Users/berna/Desktop/Faculdade/TESE/EnsaiosVib"
file_name = 'logcomvib.txt'
file_path = os.path.join(folder_path, file_name)

data_buffer=[]

# Leitura do ficheiro de texto
with open(file_path, 'r') as file:
    for line in file:
        data = int(line.strip())
        data_buffer.append(data)
buffer_size= len(data_buffer)


# Criar Butterworth HPF
nyquist_freq = 0.5 * sampling_freq
cutoff = cutoff_freq / nyquist_freq
b, a = butter(order, cutoff, btype='highpass', analog=False)


# Aplica HPF
filtered_data = filtfilt(b, a, data_buffer)



#transformada de Hilbert
analytic_data = hilbert(filtered_data)
envelope = np.abs(analytic_data)

#retirar DC bias
mean = np.mean(envelope)
for i in range(len(envelope)):
    envelope[i] -= mean


# Realiza FFT
freq = fftfreq(buffer_size, 1 / sampling_freq)
mask = (freq >= 0)
fft_data = fft(envelope) * 2 / buffer_size
fft_data = np.abs(fft_data[mask])


def plot(ndeplot):
    if ndeplot == 1:
      plt.plot(freq[mask], fft_data)
      plt.xlabel('Frequência (Hz)',font)
      plt.ylabel('Amplitude',font)
      plt.title('Espetro de frequências (sinal de envelope)',font)
      plt.savefig("Espetro de frequências envelope.png", dpi = 100,bbox_inches = 'tight')
    elif ndeplot == 2:
      plt.plot(range(len(envelope)),envelope,label='Envelope')
      plt.plot(range(len(filtered_data)),filtered_data,label = 'Sinal filtrado')
      plt.legend()
      plt.xlabel('Amostras')
      plt.ylabel('Amplitude')
      plt.title('Geração do envelope')
      plt.xlim(1000,1150)
      plt.savefig("Envelope gerado.png", dpi = 100,bbox_inches = 'tight')
    elif ndeplot == 3:
      plt.plot(range(len(filtered_data)),filtered_data, color = "tab:orange")
      plt.xlabel('Amostras')
      plt.ylabel('Amplitude')
      plt.title('Excerto de sinal filtrado')
      plt.xlim(1000,1150)
      plt.savefig("DT excerto filtrado.png", dpi = 100, bbox_inches = 'tight')
    elif ndeplot ==5:
      plt.plot(range(len(data_buffer)),data_buffer,color = "tab:orange")
      plt.xlabel('Amostras')
      plt.ylabel('Amplitude')
      plt.title('Excerto de sinal base')
      plt.xlim(1000,1150)
      plt.savefig("DT excerto.png", dpi = 100,bbox_inches = 'tight' )  
      
  

plot(1)

plt.grid(True)
plt.show()

