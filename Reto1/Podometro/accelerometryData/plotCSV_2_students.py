######################################################################
#   PYTHON CODE.
######################################################################
# Copyright (C) 2024. F.E.Segura-Quijano (FES) fsegura@uniandes.edu.co
#  
# Este trabajo está licenciado bajo la Licencia: 
# Creative Commons Atribución-NoComercial 4.0 Internacional.
# Para ver una copia de esta licencia, visita 
# http://creativecommons.org/licenses/by-nc/4.0/ o envía una carta
# a Creative Commons, PO Box 1866, Mountain View, CA 94042, USA.
######################################################################

######################################################################
# # Este código fue generado con la asistencia de ChatGPT de OpenAI.
######################################################################

######################################################################
# LIBRARIES.
######################################################################
# pip install pandas matplotlib
# pip install --upgrade matplotlib
import pandas as pd
import matplotlib.pyplot as plt

######################################################################
# INITIAL CONFIGURATION AND DATA LOAD
#####################################################################´
# Cargar los datos desde un archivo CSV
try:
    data = pd.read_csv('acceleration_2024-08-10_13-17-29.csv')
    # data = pd.read_csv('test1.csv')
except Exception as e:
    print(f"Error al leer el archivo CSV: {e}")
    exit()

######################################################################
# DATA MANIPULATION
#####################################################################´
# Conversión a numérico y eliminación de filas con NaN
columnas = ['time', 'ax', 'ay', 'az']
for columna in columnas:
    if columna in data.columns:
        data[columna] = pd.to_numeric(data[columna], errors='coerce')
    else:
        print(f"La columna '{columna}' no se encuentra en el archivo CSV")
        exit()

data.dropna(inplace=True)


######################################################################
# DATA VALIDATION
#####################################################################´
# Comprobación de los datos
for columna in columnas:
    print(f"Datos de {columna}:", data[columna].head())
    print(f"Longitud de {columna}:", len(data[columna]))

######################################################################
# DATA VISUALIZATION
#####################################################################´
# Gráfico
plt.figure(figsize=(10, 6))

# Graficar cada señal contra el tiempo
plt.plot(data['time'], data['ax'], label='Señal 1')
plt.plot(data['time'], data['ay'], label='Señal 2')
plt.plot(data['time'], data['az'], label='Señal 3')

plt.title('Gráfico de las Señales')
plt.xlabel('Tiempo')
plt.ylabel('Valor de las Señales')
plt.legend()
plt.grid(True)
plt.show()

