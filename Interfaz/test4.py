# import info
# import prueba
import returnCompiler 
import hcito 
import time
import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QTabWidget, QTableWidget, \
    QTableWidgetItem, QCheckBox, QTextEdit, QPushButton, QMessageBox

def getRecordSelect(record):
    lista = record.split(',')
    return lista

def getRecordSelectRange(record):
    record = record.replace('\r', '')
    lista_de_listas = [registro.strip() for registro in record.split(';') if registro.strip()]
    lista_de_listas = [item.split(',') for item in lista_de_listas]
    lista_de_listas = [[item.strip() for item in registro] for registro in lista_de_listas]
    return lista_de_listas

class MiSGDB(QWidget):
    def __init__(self):
        super().__init__()
        compiler = returnCompiler.SQLCompiler()
        self.setWindowTitle('Mi SGDB')

        # Layout principal
        layout_principal = QVBoxLayout()

        # Layout superior con barra lateral y área de texto
        layout_superior = QHBoxLayout()

        # Panel lateral
        self.panel_lateral = QVBoxLayout()
        self.panel_label = QLabel('Tables')
        self.panel_lateral.addWidget(self.panel_label)
        # Añadir botones para Customer y Order
        # btn_customer = QPushButton('Customer')
        # btn_order = QPushButton('Order')
        # panel_lateral.addWidget(btn_customer)
        # panel_lateral.addWidget(btn_order)

        # Área de texto
        area_texto = QTextEdit()
        area_texto.setText(
            "create table CustomerAVL from file \"../datos_small.csv\" using index avl(\"Codigo\");"
                        )
        print(area_texto)
        #Manejo de las tablas: 
        tablas_l = []
        try:
            with open('tablas.txt', 'r') as file:
                tablas = file.readlines()
                for tabla in tablas:
                    tabla = tabla.strip()
                    if tabla:
                        tablas_l.append(tabla)
                        boton = QPushButton(tabla)
                        self.panel_lateral.addWidget(boton)
        except FileNotFoundError:
            print("Errooooor")
            # Si el archivo no existe, no hacemos nada
            pass


        # Botón Ejecutar
        ejecutar = QPushButton('Ejecutar')
        # Añadir al layout superior

        layout_superior.addLayout(self.panel_lateral)
        layout_superior.addWidget(area_texto)
        layout_superior.addWidget(ejecutar)

        # Pestañas de resultados
        tabs = QTabWidget()
        result_tab = QWidget()
        explain_tab = QWidget()
        transx_tab = QWidget()

        # Añadir tablas a la pestaña Result
        result_layout = QVBoxLayout()


        ## Para la parte de los headers
        ## R: Espero q me otorguen un vector de string de headers: 
        headers_vector = ["Bienvenido"]
        ## R: Para la parte de los registros espero q me den un vector del registro
        ## -> Vector<Records>
        
        # Crear tabla con el tamaño adecuado
        num_rows = 0
        num_columns = len(headers_vector)  

        # Atributos para el cambio
        # Table
        self.result_table = QTableWidget(num_rows, num_columns)

        # Añadir encabezados
        self.result_table.setHorizontalHeaderLabels(headers_vector)

        
        def get_Lista_From_Char_Vector(record): 
            #Retorna los atributos en una lista
            elements= []
            for atributos in record.record:
                atr = ""
                for j in atributos:
                    atr += j
                elements.append(atr)
            return elements

        def agregar_boton_tabla(self, nombre_tabla):
            # Crear un nuevo botón con el nombre de la tabla
            boton = QPushButton(nombre_tabla)
            # Agregar el botón al panel lateral
            self.panel_lateral.addWidget(boton)
        
        self.total = 0
        def ejecutar_comando():
            texto = area_texto.toPlainText() 
            queris = compiler.processQuery(texto)
            badQuery = False
            for query in queris:
                print("La query es", query)
                if badQuery:
                    badQuery = False
                    break

                if any("Unknown command" in q or "Invalid" in q for q in query):
                    # print("En bad queri")
                    msg = QMessageBox()
                    msg.setIcon(QMessageBox.Warning)
                    msg.setWindowTitle(query[0])
                    msg.setText("Comando desconocido")
                    msg.setInformativeText("Por favor, revise su entrada y vuelva a intentarlo.")
                    msg.exec_()
                    badQuery = True
                elif any("CREATE" in q for q in query) and not any("Invalid" in q for q in query):
                    # print("En create")
                    nombre_tabla = query[1]
                    if nombre_tabla not in tablas_l:
                        boton = QPushButton(nombre_tabla)
                        tablas_l.append(nombre_tabla)
                        self.panel_lateral.addWidget(boton)
                        with open('tablas.txt', 'a') as file:
                            file.write(nombre_tabla + '\n')
                    else: 
                        msg = QMessageBox()
                        msg.setIcon(QMessageBox.Warning)
                        msg.setWindowTitle(query[0])
                        msg.setText("Tabla ya creada")
                        msg.setInformativeText("Por favor, cree otra tabla")
                        msg.exec_()
                elif any("SEARCH" in q for q in query) and not any("Invalid" in q for q in query):
                    if any("Key not found" in q for q in query):
                        msg = QMessageBox()
                        msg.setIcon(QMessageBox.Warning)
                        msg.setWindowTitle("Llave no encontrada")
                        msg.setText("No hay registro con esa llave")
                        msg.setInformativeText("Por favor, intente con otro key")
                        msg.exec_()
                        badQuery = True
                        break
                    if any("RANGE" in q for q in query): #Busqueda por rango
                        print("Entro eeees")
                        resultado = getRecordSelectRange(query[1])
                        headers_vector = ["atr" + str(i) for i in range(len(resultado[0]))]
                        self.result_table.setColumnCount(len(headers_vector))
                        self.result_table.setHorizontalHeaderLabels(headers_vector)
                        self.result_table.setRowCount(len(resultado))
                        for row_index, registro in enumerate(resultado):
                            for col_index, value in enumerate(registro):
                                self.result_table.setItem(row_index, col_index, QTableWidgetItem(str(value)))
                        
                    else: #Busqueda normalica
                        print("Busqueda normalica")
                        reultado = getRecordSelect(query[1])
                        headers_vector = ["atr"+str(i) for i in range( len(reultado))]
                        self.result_table.setColumnCount(len(headers_vector))
                        self.result_table.setHorizontalHeaderLabels(headers_vector)
                        self.result_table.setRowCount(1)
                        for i, value in enumerate(reultado):
                            self.result_table.setItem(0, i, QTableWidgetItem(str(value)))
                elif any("INSERT" in q for q in query) and not any("Invalid" in q for q in query):
                    print("iNSERTA?")
                    if any("repite" in q for q in query):
                        msg = QMessageBox()
                        msg.setIcon(QMessageBox.Warning)
                        msg.setWindowTitle("El key se repite o hubo un fallo interno")
                        msg.setText("Erro")
                        msg.setInformativeText("Por favor, intente con otro key")
                        msg.exec_()
                    if any("correctamente" in q for q in query):
                        print("correctamente")
                        msg = QMessageBox()
                        msg.setIcon(QMessageBox.Information)
                        msg.setWindowTitle("Insertion bien ")
                        msg.setText("Exito")
                        msg.setInformativeText("Registro inserttado")
                        msg.exec_()         

                else: 
                    print("La quieru es: ",query)
                    headers_vector = ["atr"+str(i) for i in range(len(query))]
                    print("El len de query es", len(query))
                    # Ajustar el número de columnas
                    self.result_table.setColumnCount(len(headers_vector))
                    # Establecer encabezados
                    self.result_table.setHorizontalHeaderLabels(headers_vector)
                    # Ajustar el número de filas de la tabla
                    self.result_table.setRowCount(1)
                    for i, value in enumerate(query):
                        # Insertar cada valor en la columna correspondiente
                        self.result_table.setItem(0, i, QTableWidgetItem(str(value)))
                    
                
                
            print(queris)
            print("Texto en el área de texto:", texto)

        # Conectar el botón a la función ejecutar_comando
        ejecutar.clicked.connect(ejecutar_comando)

        result_layout.addWidget(self.result_table)
        result_tab.setLayout(result_layout)

        # Añadir las pestañas
        tabs.addTab(result_tab, "Result")
        tabs.addTab(explain_tab, "Explain")
        tabs.addTab(transx_tab, "Transx")

        # Footer
        footer_layout = QHBoxLayout()
        footer_label = QLabel('Time execution:')
        self.footer_label2 = QLabel(str(self.total)+ " milisec")
        self.footer_label2.setStyleSheet("""
            QLabel {
                background-color: lightblue;  /* Fondo azul claro */
                border-radius: 10px;           /* Bordes redondeados */
                padding: 5px 10px;            /* Espaciado interno */
                color: black;                 /* Color del texto */
            }
        """)

        # Añadir widgets al footer
        footer_layout.addWidget(footer_label)
        footer_layout.addWidget(self.footer_label2)

        # Añadir todos los layouts al layout principal
        layout_principal.addLayout(layout_superior)
        layout_principal.addWidget(tabs)
        layout_principal.addLayout(footer_layout)

        # Establecer el layout de la ventana principal
        self.setLayout(layout_principal)
        


if __name__ == '__main__':
    
    
    app = QApplication(sys.argv)
    ventana = MiSGDB()
    ventana.show()
    sys.exit(app.exec_())
