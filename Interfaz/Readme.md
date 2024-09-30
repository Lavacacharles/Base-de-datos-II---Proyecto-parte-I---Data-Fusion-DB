# Proyecto de Configuración con Pybind11 y PyQt5

Este proyecto proporciona una guía paso a paso para configurar un entorno de desarrollo que utiliza Pybind11 para enlazar código C++ con Python y PyQt5 para la creación de interfaces gráficas.

## Requisitos Previos
1. **Instalar WSL2 en Windows**:
   - Instalar wsl en Windows para poder usar ubuntu, o usar Linux de frente
   - Puede seguir el siguiente enlace: https://www.youtube.com/watch?v=L4f1XHrSJEg&t=93s

2. **Instalar Herramientas de Compilación para C++ y Python**:
   - Asegúrate de instalar un compilador de C++ y Python en tu entorno WSL:
    ```bash
     sudo apt install g++
     sudo apt install python3 
     ```
   - También es recomendable instalar VsCode:
     ```bash
     code .
     ```


## Pasos de Configuración
1. **Clonar el repositorio de pybind11**:
   - Dentro del directorio de tu proyecto, clona el repositorio de Pybind11 puedes seguir el siguiente tutorial:  https://www.youtube.com/watch?v=_5T70cAXDJ0
     ```bash
     git clone https://github.com/pybind/pybind11.git
     ```

2. **Crear y entrar en el directorio `build`**:
   - Ejecuta los siguientes comandos para crear el directorio `build` y entrar en él:
     ```bash
     mkdir build
     cd build
     ```

3. **Configurar el entorno de compilación**:
   - Sal de tu directorio Build  asegura de tener el CMakeList.txt:
     ![imagen](https://github.com/user-attachments/assets/c22b88e3-39c8-4476-82c8-5c847605655e)
   - En ese mismo directorio ten todos lo archivos tales como el hash.h y otros: 
![imagen](https://github.com/user-attachments/assets/361ba71b-5da6-4252-b4c2-85c51789a5d7)

   - Dentro de la carpeta `build`, ejecuta:
     ```bash
     cmake ..
     make
     ```
   - Dentro de build deberás de correr el archivo test4.py y ya estará listo
     

## Problemas Frecuentes
Si experimentas problemas con la instalación de la librería PyQt5 para la interfaz en Python, prueba con los siguientes comandos:

1. **Actualizar el sistema**:
   ```bash
   sudo apt-get update
   sudo apt-get install build-essential libgl1-mesa-dev libglu1-mesa-dev
   sudo apt-get install libxcb-xinerama0 libxcb-shm0 libxcb-randr0 libxcb-xkb1 libxcb-icccm4 libxcb-sync1 libxcb-shape0 libxkbcommon-x11-0
pip install pyqt5
sudo apt-get install libxcb-shape0 libxcb-xinerama0 libxcb-xkb1 libxkbcommon-x11-0

   ```
   
