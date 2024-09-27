# Data Fusion DB - Proyecto 1

### Proyecto P1 - Base de datos II - Sección I

### Team

- Juan Aquino
- Claudio Echarre
- Piero Guerrero
- Marcelino Maita
- Aaron Navarro

---

El proyecto DataFusionDB consiste en la integración (**Fusion**) de diferentes modelos de datos y
técnicas avanzadas de organización de información dentro de una única base de datos (**DB**).
El propósito de construir una plataforma versátil que unifique datos estructurados y no
estructurados, incluyendo documentos textuales en principio para luego incluir objetos multimedia.

# Objetivos

### Generales

- Soporte de modelo relacion basado en tablas.
- Implementación de técnicas de organización de información.

### Específicos

- Implementación de un mini gestor de base de datos optimizado para la gestión de la información.
- Aplicación y comparación en tiempos de ejecución entre cada técnica de organización e indexación de archivos físicos.

# Técnicas de indexación y Organización de Archivos

Para la gestión de los archivos físicos, se eligieron 3 estrategias para la organización de archivos e índices.

- **_AVL File_**
- **_B+Tree Indexing_**
- **_Extendible Hashing Indexing_**

## AVL File

Un AVL File es una estructura de datos que implementa un árbol AVL (Adelson-Velsky y Landis) para
organizar archivos en memoria secundaria (como discos duros) de manera eficiente. Un árbol AVL es
un tipo de árbol binario de búsqueda que se mantiene balanceado mediante la aplicación de rotaciones
después de cada operación de inserción o eliminación, garantizando un tiempo de acceso logarítmico
$O(log n)$ en las operaciones de búsqueda, inserción y eliminación.

> [!IMPORTANT]
> Se optimizó los inserts y removes implementando un FreeList LIFO

### Estructuras

En el archivo físico, se tiene como cabecera, los datos del root y del último eliminado. Al iniciar la tabla, la
estructura **_HeadAVL_** tendra los siguientes valores:

**HeadAVL** (size=8B)

| Root | NextDel |
| ---- | ------- |
| -1   | -1      |

Posterior al head, se guardan los registros con la siguiente estructura:
**NodeAVL** (size=16B+sizeof(Record))

| NextDel  | Left     | Right    | Height | Record    |
| -------- | -------- | -------- | ------ | --------- |
| -1(long) | -1(long) | -1(long) | 0(int) | record(R) |

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
   4. Nodo contiene dos hijos:
      1. Buscar succesor y reemplazar el valor del nodo a eliminar con el sucesor.
      2. Eliminar sucesor en su ubicación original.
   5. Si el nodo a eliminar tiene un hijo o no tiene hijos:
      1. Si no tiene hijos, eliminar
      2. Si tiene un hijo, se redirecciona los índices del hijo al padre del nodo eliminado.
   6. FreeList
      1. Se copia el valor del nextDel en el registro eliminado.
      2. Se actualiza el nextDel del head con la posición del registro eliminado.

2. Actualización de alturas
   1. Actualizar las alturas desde el padre hasta la raíz.
3. Balanceo y rotaciones si es necesario
   1. Calcular el factor de balanceo desde el padre hasta la raíz.
   2. Evaluar el factor de balanceo:
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

| seg         | N=10  | N=100 | N=1K   | N=10K   | N=100K   | N=1M(aprox) |
| ----------- | ----- | ----- | ------ | ------- | -------- | ----------- |
| buildTable  | 0.383 | 7.989 | 122.22 | 1556.27 | 17735.63 | 201655.4    |
| add         | 0.023 | 0.083 | 0.196  | 0.204   | 0.25     | 0.31        |
| remove      | 0.001 | 0.001 | 0.002  | 0.002   | 0.002    | 0.002       |
| search      | 0.001 | 0.011 | 0.097  | 1.011   | 7.28     | 19.03       |
| rangeSearch | 0.013 | 0.087 | 0.138  | 0.154   | 0.22     | 0.27        |

## B+Tree Indexing

## Extendible Hashing Indexing

Extendible hashing es un tipo de hash dínamico usado para gestionar archivos. Que
sea dinámico significa que crece y reduce su tamaño dinámicamente cuando se
realizan inserciones o eliminaciones.

> [!IMPORTANT]
> Para la liberación de buckets cuando estás no están siendo usados se realiza la estrategia de free list.
> Para la eliminación de un record en sin liberación de bucket, se usa move to last
> El tamaño del record es dinámico, tal que se crea un archivo headers que contiene el tamaño de las columnas

### Estructuras

Aparte del archivo en el que guardar el data, se requiere de un archivo adicional para guardar los índices.
Aparte, este archivo guarda el factor, la profundidad (depth) y los punteros asignados para cada llave. El factor es la
cantidad máxima de elementos que hay en un bucket, mientras que la profundidad
es la cantidad máxima de bits que pueden ser usados por la función de hashing
para calcular el valor de hashing de una llave.

Para guardar los datos, se utilizan Buckets. Estos contienen los siguientes valores.

**Bucket** (size=depth + 16 + sizeof(Record) \* factor)

| code  | size | local | pointer  | records                  |
| ----- | ---- | ----- | -------- | ------------------------ |
| depth | int  | int   | long int | sizeof(Record) \* factor |

- El código (code) es asignado al bucket para saber los punterso que se deben actualizar al hacer una partición. Su tamaño máximo es equivalente a la profundidad global.
- El tamaño (size) es la cantidad de records que el bucket tiene actualmente.
- La profundidad local (local depth) es la cantidad de bits usados por el código.
- El puntero (pointer) es usado para el caso de que el bucketse llene y este ya no se pueda partir. Cuando suceda esto, se crea un nuevo bucket el cual es apuntado por el bucket actual.
- Los registros (records) son los datos guardados. Se puede tener un número de registros no superior al factor. Cuando se supera, se debe partir o encadenar un bucket.

Inicialmente, se crea 2 buckets con los códigos de 0 y 1, por lo que tienen una profundidad de 1. Ambos no tienen registros así que el tamaño es 1 y el puntero es -1 como no se ha encadenado.

### Algoritmos

> [!NOTE]
> Se optimizó el uso de memoria para que solo se cargue un bucket a la vez para las diversas operaciones. Las unicas veces que se carga más de un bucket es cuando se ha realizado chaining, pues allí se necesita aplicar operaciones entre buckets.
> Para evitar leer del archivo constantemente, la clase posee atributos de valores leídos comúnmente de los archivos, como es el tamaño de la llave y el tamaño de un registro.

#### **Insert(string record)**

1. Encontrar bucket.
   1. Extraer llave del registro.
   2. Obtener puntero asociado con la llave.
   3. Extraer información del bucket.
2. Si es que el bucket tiene un tamaño inferior al factor, insertar en bucket.
   1. Cambiar atributos del bucket para que tenga el nuevo registro y se incremente su tamaño.
   2. Sobrescribir el bucket en su posición asociada.
3. Si es que el bucket está lleno, pero su profundidad local es inferior a la profundidad global, partir bucket.
   1. Dividir registros, incluyendo al que se va a insertar, entre los que tienen el siguiente bit como 1 y los que lo tienen como 0.
   2. Si se tiene que la cantidad de registros asociados al código actual más 0 es superior al factor
      1. Crear nuevo bucket para el código actual más 1. Este empieza vacío.
      2. Redireccionar punteros associados con el código actual más 1.
         1. Se empieza en el código actual.
         2. Se usa un paso de 2 elevado a la profundidad local hasta el final.
      3. Se modifica el bucket actual para tener el código actual más 0 y aumentar la profundidad local.
      4. Se escribe el bucket en el archivo.
      5. Se hace una llamada hacia insert con el mismo registro dado originalmente.
   3. Si se tiene que la cantidad de registros asociados al código actual más 1 es superior al factor
      1. Crear nuevo bucket para el código actual más 1. Asignarle el tamaño y los registros del bucket actual.
      2. Redireccionar punteros associados con el código actual más 1.
         1. Se empieza en el código actual .
         2. Se usa un paso de 2 elevado a la profundidad local hasta el final.
      3. Se modifica el bucket actual para tener el código actual más 0 y aumentar la profundidad local. También se quita los registros para dejarlo como vacio
      4. Se escribe el bucket en el archivo.
      5. Se hace una llamada hacia insert con el mismo registro dado originalmente.
   4. No se tiene overflow de bucket
      1. Crear nuevo bucket para el código actual más 1. Asignarle el tamaño y los registros cuyo siguiente bit del código es 1.
      2. Redireccionar punteros associados con el código actual más 1.
         1. Se empieza en el código actual .
         2. Se usa un paso de 2 elevado a la profundidad local hasta el final.
      3. Modificar el bucket actual para tener el código actual más 0 y aumentar la profundidad local. También se reemplazan los registros por cuyos tienen el siguiente bit como 1.
4. No se pueder partir así que se empieza a encadenar.
   1. Mientras no se llegue al último bucket de la cadena, iterar.
      1. El bucket actual tiene espacio.
      2. Cambiar atributos del bucket para que tenga el nuevo registro y se incremente su tamaño.
      3. Sobrescribir el bucket en su posición asociada.
      4. Se sale del loop.
   2. Si se está lleno y el puntero no apunta a ningún lugar.
      1. Crear nuevo bucket con el registro dado. La profundidad y el código son los mismos que el bucket actual.
      2. Cambiar puntero del bucket actual para apuntar al nuevo.
      3. Sobrescribir el bucket en su posición asociada.
      4. Se sale del loop.
   3. Si el bucket está lleno y se tiene un bucket disponible en el puntero.
      1. Moverse a nuevo bucket y empezar de nuevo.

#### **Remove(T key)**

1.  Encontrar bucket.
    1.  Extraer llave del registro.
    2.  Obtener puntero asociado con la llave.
    3.  Extraer información del bucket.
2.  Iterar por la cadena de buckets hasta que se encuentre
    1. Iterar por cada registro del bucket. Guardar todos los registros en una variable. Si se encuentra la llave, no guardar ese registro.
    2. Si se encontró, dejar de iterar por los buckets.
    3. Si no se encontró y se tiene puntero hacia otro bucket, cambiar bucket actual por el siguiente en la cadena.
    4. Si no hay puntero y no se encontró, salir.
3.  Si no se encontró, regresar falso
4.  Si no hay puntero hacia el siguiente bucket.

    1. Si solo se tiene un registro y el bucket actual es el primero de la cadena de buckets. 2. Conseguir bucket hermano. O sea que el bucket cuyo primer dígito del códgigo sea opuesto al actual.
       1. Si el bucket hermano está lleno a menos de la mitad de su capacidad.
          1. Cambiar hermano para que no tenga el primer dígito de su código actual y reducir su profundidad por uno..
          2. Liberar bucket actual y cambiar el free list de buckets.
          3. Redireccionar los punteros del índice empezando en el código del hermano.
       2. Caso contrario, ignorar.
    2. Si solo se tiene un registro.
       1. Quitar el puntero del bucket anterior.
       2. Liberar bucket actual y cambiar el free list de buckets.
    3. Si se tiene más de un registro en el bucket actual
       1. Añadir registro a bucket actual.
       2. Escribir cambios del bucket actual en el archivo.

5.  Si hay puntero hacia el siguiente bucket.
    1.  Iterar hasta llegar al bucket que está al final de la cadena.
    2.  Añadir último registro del último bucket a los registros del bucket actual. Guardar cambios en el archivo
    3.  Si el tamaño del último bucket is igual a 1.
        1.  Quitar punero del bucket anterior.
        2.  Liberar último bucket.
    4.  Si el tamaño del último bucket is superior a 1.
        1. Quitar registro del bucket.
        2. Escribir cambios en el archivo.

#### **Search (int key)**

1. Encontrar bucket.
   1. Extraer llave del registro.
   2. Obtener puntero asociado con la llave.
   3. Extraer información del bucket.
2. Mientras que haya buckets por recorrer y no se haya encontrado el record, iterar
   1. Iterar por todos los registros del bucket actual.
      1. Si se encuentra un registro cuya llave encaje con la dada, regresar registro.
   2. Si se tiene un puntero hacia otro bucket.
      1. Moverse hacia el siguiente bucket y empezar de nuevo

#### **RangeSearch**

1. Crear lista de encontrados.
2. Iterar desde el límite izquierdo hasta el derecho.
   1. Realizar find sobre la llave actual.
   2. Añadir resutlado a lista de encontrados.
3. Regresar lista de encontrados

### Evolución de tiempos de ejecución

Los siguientes tiempos están en segúndos. Se tiene un factor de 8 y una profundidad global de 16

| seg         | N=10  | N=100 | N=1K  | N=10K | N=100K | N=1M(aprox) |
| ----------- | ----- | ----- | ----- | ----- | ------ | ----------- |
| buildTable  | 0.001 | 0.389 | 0.776 | 1.438 | 3.456  | 20.910      |
| add         | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.000       |
| remove      | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.000       |
| search      | 0.000 | 0.000 | 0.000 | 0.000 | 0.003  | 0.051       |
| rangeSearch | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.000       |

# Compilador y GUI

# Resultados experimentales

Para el ánalisis de los rendimientos entre técnicas, se utilizaron las tablas de tiempo de ejecución.
A partir de ello, se generaron los diferentes gráficos que fueron importantes para visualizar las complejidades de cada
algoritmo.

## Análisis de Rendimientos

### Construir tabla desde file

#### Tiempos de ejecución

| buildFromFile      | N=10  | N=100 | N=1K   | N=10K   | N=100K   | N=1M(aprox) |
| ------------------ | ----- | ----- | ------ | ------- | -------- | ----------- |
| AVLFile            | 0.383 | 7.989 | 122.22 | 1556.27 | 17735.63 | 201655.4    |
| B+Tree             |       |       |        |         |          |             |
| Extendible Hashing | 0.001 | 0.389 | 0.776  | 1.438   | 3.456    | 20.910      |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis

- [x] asdsa
- [ ] asdsa
- [ ] asdsa

### Insertar registro

#### Tiempos de ejecución

| add                | N=10  | N=100 | N=1K  | N=10K | N=100K | N=1M  |
| ------------------ | ----- | ----- | ----- | ----- | ------ | ----- |
| AVLFile            | 0.023 | 0.083 | 0.196 | 0.204 | 0.25   | 0.31  |
| B+Tree             |       |       |       |       |        |       |
| Extendible Hashing | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.000 |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis

- [x] asdsa
- [ ] asdsa
- [ ] asdsa
-

### Eliminar registro

#### Tiempos de ejecución

| remove             | N=10  | N=100 | N=1K  | N=10K | N=100K | N=1M(aprox) |
| ------------------ | ----- | ----- | ----- | ----- | ------ | ----------- |
| AVLFile            | 0.001 | 0.001 | 0.002 | 0.002 | 0.002  | 0.002       |
| B+Tree             |       |       |       |       |        |             |
| Extendible Hashing | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.0000.000  |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis

- [x] asdsa
- [ ] asdsa
- [ ] asdsa
-

### Búsqueda de registro

#### Tiempos de ejecución

| search             | N=10  | N=100 | N=1K  | N=10K | N=100K | N=1M(aprox) |
| ------------------ | ----- | ----- | ----- | ----- | ------ | ----------- |
| AVLFile            | 0.001 | 0.011 | 0.097 | 1.011 | 7.28   | 19.03       |
| B+Tree             |       |       |       |       |        |             |
| Extendible Hashing | 0.000 | 0.000 | 0.000 | 0.000 | 0.003  | 0.051       |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis

- [x] asdsa
- [ ] asdsa
- [ ] asdsa

### Búsqueda por rango

#### Tiempos de ejecución

| rangeSearch        | N=10  | N=100 | N=1K  | N=10K | N=100K | N=1M(aprox |
| ------------------ | ----- | ----- | ----- | ----- | ------ | ---------- |
| AVLFile            | 0.013 | 0.087 | 0.138 | 0.154 | 0.22   | 0.27       |
| B+Tree             |       |       |       |       |        |            |
| Extendible Hashing | 0.000 | 0.000 | 0.000 | 0.000 | 0.000  | 0.000      |

#### Gráfica de complejidades

<img alt="Gráfico de evolución de tiempo ejecución vs. data" height="200" src="https://costaricamakers.com/wp-content/uploads/2022/03/image-5.png" width="300"/>

#### Análisis

- [x] asdsa
- [ ] asdsa
- [ ] asdsa

# Execute
