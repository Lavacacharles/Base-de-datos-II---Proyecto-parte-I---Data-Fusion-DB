"""
import csv
import pandas as pd
def rewrite():
    with open("out.csv", encoding="utf-8") as File, open("out_clean.csv", 'w') as outFile:
        csv_reader = csv.reader(File, delimiter=',')
        n=0
        for line in csv_reader:
            print("Line: ", n)
            count = 3
            reg = ""
            for field in line:
                if count == 0:
                    break
                else:
                    if reg != "":
                        reg +=","
                    reg += field
                count -= 1
            reg +="\n"
            outFile.writelines(reg)
            n +=1
    return

def sorted(inputFile, outFile):
    df = pd.read_csv(inputFile, on_bad_lines='skip')
    df_ordenado = df.sort_values(by='url')
    df_ordenado.to_csv(outFile, index=False)
#rewrite()
sorted('out_clean.csv','out_sorted.csv')
"""
import matplotlib.pyplot as plt
import numpy as np
from numpy.ma.core import zeros
from scipy.interpolate import PchipInterpolator


def generate_compare3(avl, hash, sequential, algoritm):
    n_puntos = np.logspace(np.log10(min(n_datos)), np.log10(max(n_datos)), num=600)
    cs_AVL = PchipInterpolator(n_datos, avl)
    cs_Hash = PchipInterpolator(n_datos, hash)
    cs_Sequential = PchipInterpolator(n_datos, sequential)

    sv_AVL = cs_AVL(n_puntos)
    sv_Hash = cs_Hash(n_puntos)
    sv_Sequential = cs_Sequential(n_puntos)

    fig, axs = plt.subplots(1, 1, figsize=(10, 5))
    # Graficar los resultados
    axs.plot(n_puntos, sv_AVL, label="Add AVL", color='blue', marker='', markersize=5)
    axs.plot(n_puntos, sv_Hash, label="Add Hash", color='orange', marker='', markersize=5)
    axs.plot(n_puntos, sv_Sequential, label="Add Sequential", color='green', marker='', markersize=5)
    axs.set_xscale('log')  # Usar escala logarítmica en el eje x
    axs.set_xlabel('Tamaño de los datos (N)')
    axs.set_ylabel('Tiempo de ejecución (segundos)')
    axs.set_title('Comparación de tiempos de ejecución de '+algoritm)
    axs.grid(which='both', linestyle='--', linewidth=0.5, color='gray')
    axs.minorticks_on()
    # Mostrar la gráfica
    plt.show()
    return

def generate_compare2(avl, hash, algoritm):
    n_puntos = np.logspace(np.log10(min(n_datos)), np.log10(max(n_datos)), num=600)
    cs_AVL = PchipInterpolator(n_datos, avl)
    cs_Hash = PchipInterpolator(n_datos, hash)
    #cs_Sequential = PchipInterpolator(n_datos, sequential)

    sv_AVL = cs_AVL(n_puntos)
    sv_Hash = cs_Hash(n_puntos)
    #sv_Sequential = cs_Sequential(n_puntos)

    fig, axs = plt.subplots(1, 1, figsize=(10, 5))
    # Graficar los resultados
    axs.plot(n_puntos, sv_AVL, label="Add AVL", color='blue', marker='', markersize=5)
    axs.plot(n_puntos, sv_Hash, label="Add Hash", color='orange', marker='', markersize=5)
    #axs.plot(n_puntos, sv_Sequential, label="Add Sequential", color='green', marker='', markersize=5)
    axs.set_xscale('log')  # Usar escala logarítmica en el eje x
    axs.set_xlabel('Tamaño de los datos (N)')
    axs.set_ylabel('Tiempo de ejecución (segundos)')
    axs.set_title('Comparación de tiempos de ejecución de '+algoritm)
    axs.grid(which='both', linestyle='--', linewidth=0.5, color='gray')
    axs.minorticks_on()
    # Mostrar la gráfica
    plt.show()
    return


n_datos = [10, 100, 1000, 10000, 100000, 1000000]

# Tiempos de ejecución para AVL (en segundos)
build_AVL = [0.383, 7.989, 122.22, 1556.27, 17735.63, 201655.4]
add_AVL = [0.0023,0.083,0.196,0.204,0.25,0.31]
search_AVL = [0.001,0.001,0.002,0.002,0.002,0.002]
rangeSearch_AVL = [0.001,0.011,0.097,1.011,7.28, 19.03]
remove_AVL= [0.013, 0.087, 0.138, 0.154, 0.22, 0.27]

# Tiempos de ejecución para AVL (en segundos)
build_Hash = [0.001,0.389,0.776,1.438,3.456,20.910]
add_Hash = [0.00,0.00,0.00,0.00,0.00,0.00]
search_Hash = [0.00,0.00,0.00,0.00,0.00,0.00]
rangeSearch_Hash = [0.000,0.000,0.000,0.000,0.003,0.051]
remove_Hash= [0.00,0.00,0.00,0.00,0.00,0.00]

add_Sequential = [0.181,2.035,17.084,253.253,4858.953,0.00]
search_Sequential = [0.007,0.02,0.042,0.361,4.639,0.00]
rangeSearch_Sequential = [0.001,0.001,0.001,0.007,0.001,0.00]
remove_Sequential= [0.268,4.1,35.03,436.299,447,0.00]

generate_compare2(add_AVL, add_Hash, "Add")
generate_compare2(search_AVL, search_Hash , "Search")
generate_compare2(rangeSearch_AVL, rangeSearch_Hash, "RangeSearch")
generate_compare2(remove_AVL, remove_Hash, "Remove")
