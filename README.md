# Data Fusion DB - Proyecto 1
***
### Proyecto P1 - Base de datos II - Sección I

### Team
- Juan Aquino
- Claudio Echarre
- Piero Guerrero
- Marcelino Maita
- Aaron Navarro
***
El proyecto DataFusionDB consiste en la integración (**Fusion**) de diferentes modelos de datos y
técnicas avanzadas de organización de información dentro de una única base de datos (**DB**).
El propósito de construir una plataforma versátil que unifique datos estructurados y no
estructurados, incluyendo documentos textuales en principio para luego incluir objetos multimedia.
***
# Objetivos
### Generales
- Soporte de modelo relacion basado en tablas.
- Implementación de técnicas de organización de información.
### Específicos
- Implementación de un mini gestor de base de datos optimizado para la gestión de la información.
- Aplicación y comparación en tiempos de ejecución entre cada técnica de organización e indexación de archivos físicos.
***
# Técnicas de indexacción y Organización de Archivos
Para la gestión de los archivos físicos, se eligieron 3 estrategias para la organización de archivos e índices.
- ***AVL File*** 
- ***B+Tree Indexing***
- ***Extendible Hashing Indexing***

## AVL File
Un AVL File es una estructura de datos que implementa un árbol AVL (Adelson-Velsky y Landis) para 
organizar archivos en memoria secundaria (como discos duros) de manera eficiente. Un árbol AVL es
un tipo de árbol binario de búsqueda que se mantiene balanceado mediante la aplicación de rotaciones
después de cada operación de inserción o eliminación, garantizando un tiempo de acceso logarítmico 
*$O(log$ $n)$* en las operaciones de búsqueda, inserción y eliminación.

> [!IMPORTANT]
> Se optimizó los inserts y removes implementando un FreeList LIFO

### Estructuras 

En el archivo físico, se tiene como cabecera, los datos del root y del último eliminado. Al iniciar la tabla, la 
estructura ***HeadAVL*** tendra los siguientes valores:

**HeadAVL** (size=8B)

| Root | NextDel |
|------|---------|
| -1   | -1      | 

Posterior al head, se guardan los registros con la siguiente estructura:
**NodeAVL** (size=16B+sizeof(Record))

| NextDel | Left    | Right    | Height | Record    | 
|---------|---------|----------|--------|-----------|
| -1(long)| -1(long)| -1(long) | 0(int) | record(R) |

### Algoritmos

> [!NOTE]
> Se optimizó la carga del buffer al escribir y leer en las posiciones fisicas exactas para 
> acceder a los indices de los hijos, altura, nextDel y data. De esta forma, no se sobrecarga 
> el buffer con todo el registro.
#### **Insert(R record)**

1. Inserción en BST
   1. Insertar en nodo hijo izquierdo si es menor que el padre.
   2. O insertar en nodo hijo derecho si es mayor que el padre.
   3. No insertar elementos repetidos.
   4. Insertar si no hay mas hijos que evaluar.
   5. Verificación si existe un registro eliminado.
      1. Se inserta en la posición fisica del registro eliminado.
      2. O se inserta al final de la tabla.
2. Actualización de alturas.
   1. Se actualiza todas las alturas desde el padre hasta la raíz.
3. Balanceo y rotaciones si es necesario. 
   1. Se optiene el factor de balanceo desde el padre hasta la raíz.
   2. Se evalua el factor de balanceo:
      1. fb > 1 && key < node->left: rotación simple a la derecha
      2. fb < -1 && key > node->right: rotación simple a la izquierda
      3. fb > 1 && key > node->left: rotación doble (izquierda - derecha)
      4. fb < -1 && key < node->right: rotación doble (derecha - izquierda)
#### **Remove(T key)**
1. Eliminación en BST
   
   1. Buscar en hijo izquierdo, si key < node.
   2. Buscar en hijo derecho, si key > node.
   3. Eliminar si se encuentra.
   2. Nodo contiene dos hijos:
      1. Buscar succesor y reemplazar el valor del nodo a eliminar con el sucesor.
      3. Eliminar sucesor en su ubicación original.
   3. Si el nodo a eliminar tiene un hijo o no tiene hijos:
      1. Si no tiene hijos, eliminar
      2. Si tiene un hijo, se redirecciona los índices del hijo al padre del nodo eliminado.
   4. FreeList
      1. Se copia el valor del nextDel en el registro eliminado.
      2. Se actualiza el nextDel del head con la posición del registro eliminado.
2. Actualización de alturas 
   1. Actualizar las alturas desde el padre hasta la raíz.
3. Balanceo y rotaciones si es necesario 
   1. Calcular el factor de balanceo desde el padre hasta la raíz.
   4. Evaluar el factor de balanceo:
      1. fb > 1 && key < node->left: rotación simple a la derecha
      2. fb < -1 && key > node->right: rotación simple a la izquierda
      3. fb > 1 && key > node->left: rotación doble (izquierda - derecha)
      4. fb < -1 && key < node->right: rotación doble (derecha - izquierda)
   
#### **Search (T key)**
Búsqueda en BST
1. Comenzar desde la raíz del árbol.
2. Comparar la clave key con el nodo actual:
   1. Si key es igual al valor del nodo, se ha encontrado el nodo. Retornar el nodo.
   2. Si key es menor que el valor del nodo, continuar la búsqueda en el subárbol izquierdo.
   3. Si key es mayor que el valor del nodo, continuar la búsqueda en el subárbol derecho.
3. Si se llega a un nodo null, significa que la clave no está en el árbol. Retornar false.

#### **RangeSearch**
1. Comenzar desde root.
2. Si el valor del null está dentro del rango [low, high]:
   1. Agregar el record a una lista de resultados.
   2. Continuar búsqueda en ambos subárboles (izquierdo y derecho). 
3. Si el valor del nodo actual es menor que low:
   1. Continuar la búsqueda solo en el subárbol derecho.
4. Si el valor del nodo actual es mayor que high:
   1. Continuar la búsqueda solo en el subárbol izquierdo.
5. Devolver la lista de resultados.

### Evolución de tiempos de ejecución


| ms          | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|-------------|------|-------|------|-------|-------|-------|
| buildTable  |      |       |      |       |       |       |
| add         |      |       |      |       |       |       |
| remove      |      |       |      |       |       |       |
| search      |      |       |      |       |       |       |
| rangeSearch |      |       |      |       |       |       |


## B+Tree Indexing




## Extendible Hashing Indexing




***
# Compilador y GUI




***
# Resultados experimentales

Para el ánalisis de los rendimientos entre técnicas, se utilizaron las tablas de tiempo de ejecución. 
A partir de ello, se generaron los diferentes gráficos que fueron importantes para visualizar las complejidades de cada 
algoritmo.   
## Análisis de Rendimientos

### Construir tabla desde file

#### Tiempos de ejecución

| buildFromFile      | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|--------------------|------|-------|------|-------|-------|-------|
| AVLFile            |      |       |      |       |       |       |
| B+Tree             |      |       |      |       |       |       |
| Extendible Hashing |      |       |      |       |       |       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis
- [X] asdsa
- [ ] asdsa
- [ ] asdsa

### Insertar registro
#### Tiempos de ejecución

| add                | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|--------------------|------|-------|------|-------|-------|-------|
| AVLFile            |      |       |      |       |       |       |
| B+Tree             |      |       |      |       |       |       |
| Extendible Hashing |      |       |      |       |       |       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis
- [X] asdsa
- [ ] asdsa
- [ ] asdsa
- 
### Eliminar registro
#### Tiempos de ejecución

| remove             | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|--------------------|------|-------|------|-------|-------|-------|
| AVLFile            |      |       |      |       |       |       |
| B+Tree             |      |       |      |       |       |       |
| Extendible Hashing |      |       |      |       |       |       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis
- [X] asdsa
- [ ] asdsa
- [ ] asdsa
- 
### Búsqueda de registro
#### Tiempos de ejecución

| search             | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|--------------------|------|-------|------|-------|-------|-------|
| AVLFile            |      |       |      |       |       |       |
| B+Tree             |      |       |      |       |       |       |
| Extendible Hashing |      |       |      |       |       |       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis
- [X] asdsa
- [ ] asdsa
- [ ] asdsa

### Búsqueda por rango
#### Tiempos de ejecución

| rangeSearch        | N=10 | N=100 | N=1K | N=10K | N=100K | N=1M |
|--------------------|------|-------|------|-------|-------|-------|
| AVLFile            |      |       |      |       |       |       |
| B+Tree             |      |       |      |       |       |       |
| Extendible Hashing |      |       |      |       |       |       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis
- [X] asdsa
- [ ] asdsa
- [ ] asdsa

***
# Execute
