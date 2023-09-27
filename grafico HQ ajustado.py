# -*- coding: utf-8 -*-
"""
Created on Mon Sep 11 18:52:18 2023

@author: berna
"""

import os
import numpy as np
import pandas as pd
import matplotlib
import matplotlib.pyplot as plt
from sklearn.metrics import r2_score


folder_path = "C:/Users/berna/Desktop/Faculdade/TESE/Ensaios2"

font = {'family' : 'normal',
        'weight' : 'normal',
        'size'   : 22}
colors=["black","blue","red"]
ic= 0

matplotlib.rc('font', **font)


frequency_data = {}

# Lê os ficheiro na pasta todos
# Os ficheiros devem um nome como "50Hz_1.txt"

for file_name in os.listdir(folder_path):
    if file_name.endswith(".txt"):

        frequency = int(file_name.split("Hz")[0])
        
        # Read the data from the file into a pandas DataFrame
        file_path = os.path.join(folder_path, file_name)
        with open(file_path, "r") as file:
            lines = file.readlines()
        
        # Extrair os valores de cada linha
        data = []
        for line in lines:
            values = line.strip().split(" ")
            if all(values):
                values = [float(value) for value in values]
                data.append(values)
        
        data = pd.DataFrame(data, columns=["Flow Rate", 
                                           "Aspiration Head", 
                                           "Compression Head", 
                                           "RPM"])
        
        


        if frequency in frequency_data:
            frequency_data[frequency].append(data)
        else:
            frequency_data[frequency] = [data]
            
            
            
plt.figure(figsize=(15,10))
for frequency, data_list in frequency_data.items():
     
    # Plot
    for i, data in enumerate(data_list):
        
        #barras de erro
        yerror= np.zeros(len(data))
        for ih in range(len(data["Compression Head"])):
            yerror[ih] = data["Compression Head"][ih]*0.015 + 0.529
        xerror= np.zeros(len(data))
        for ih in range(len(data["Flow Rate"])):
             xerror[ih] = data["Flow Rate"][ih]*0.025 + 0.009375
                 
            
        if i == 0:
            # realizar ajuste
            z = np.polyfit(data["Flow Rate"], data["Compression Head"], 2) #
            p = np.poly1d(z)
            y1=p(data["Flow Rate"])
            y2 = data["Compression Head"]
            r_squared = r2_score(y2, y1)

            print(f"R-squared:{frequency}", r_squared)
            
            #cria plots
            plt.errorbar(data["Flow Rate"], data["Compression Head"],
                         xerr = xerror, yerr=yerror,
                         color = colors[ic], fmt= "o", 
                         capsize = 6,markersize = 5)
            
            plt.plot(data["Flow Rate"], y1, label=f"{frequency}Hz",
                     color=colors[ic], linewidth = 3)

    ic+=1
            
# Definições do Plot
plt.xlabel("Caudal Volúmico (l/min)")
plt.ylabel("Altura (m)")
plt.title("Curvas de Altura (Ajuste Cúbico)")
plt.legend(reversed(plt.legend().legendHandles), reversed(["30Hz","40Hz","50Hz"]))
ax = plt.gca()
ax.set(xlim=(0, 160), ylim=(0, 60))

""" Usar para sobrepor
im =plt.imread("curvashbarreto.png")
plt.imshow(im,extent =[0,160,0,60], aspect = 'auto')
"""

plt.savefig("pump_head_curves_ajustado.png", dpi = 500)  # Guarda a imagem
plt.show()  
plt.close()  


