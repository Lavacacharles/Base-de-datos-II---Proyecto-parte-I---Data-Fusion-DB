# import info
import prueba 
import time
import sys
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QLabel, QTabWidget, QTableWidget, \
    QTableWidgetItem, QCheckBox, QTextEdit, QPushButton

class MiSGDB(QWidget):
    def __init__(self):
        super().__init__()

        self.setWindowTitle('Mi SGDB')

        # Layout principal
        layout_principal = QVBoxLayout()

        # Layout superior con barra lateral y área de texto
        layout_superior = QHBoxLayout()

        # Panel lateral
        panel_lateral = QVBoxLayout()
        panel_label = QLabel('Tables')
        panel_lateral.addWidget(panel_label)
        # Añadir botones para Customer y Order
        btn_customer = QPushButton('Customer')
        btn_order = QPushButton('Order')
        panel_lateral.addWidget(btn_customer)
        panel_lateral.addWidget(btn_order)

        # Área de texto
        area_texto = QTextEdit()
        area_texto.setText(
            "select * from")
        print(area_texto)
        
        # Botón Ejecutar
        ejecutar = QPushButton('Ejecutar')
        

            
        
        # Añadir al layout superior

        layout_superior.addLayout(panel_lateral)
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
        headers_vector = ["Nombre", "Edad", "Años", "Ciclo"]
        ## R: Para la parte de los registros espero q me den un vector del registro
        ## -> Vector<Records>
        FixedFILE = prueba.FixedFILE("data.dat")
        records = FixedFILE.readAll()
        # Crear tabla con el tamaño adecuado
        num_rows = len(records)
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


        # Rellenar con los datos
        for row, record in enumerate(records):
            recordAtributos = get_Lista_From_Char_Vector(record)
            for i in range(len(recordAtributos)): # Estamos en el record
                # print(get_Lista_From_Char_Vector(record)[i])
                self.result_table.setItem(row, i, QTableWidgetItem(str(recordAtributos[i])))

        # Opciones para ejecutar
        self.total = 0
        def ejecutar_comando():
            texto = area_texto.toPlainText() # Inserta lo que debe hacer 
            match texto:
                case "select * from":
                    start_time = time.time()
                    records = FixedFILE.readAll() 
                    self.result_table.clearContents()
                    num_rows = len(records)
                    self.result_table.setRowCount(num_rows)
                    for row, record in enumerate(records):
                        recordAtributos = get_Lista_From_Char_Vector(record)
                        for i in range(len(recordAtributos)):
                            self.result_table.setItem(row, i, QTableWidgetItem(str(recordAtributos[i])))
                    end_time = time.time()
                    self.total = (end_time - start_time) * 1000
                case "select ONE":
                    record = FixedFILE.readAll()[0]
                    self.result_table.clearContents()
                    self.result_table.setRowCount(1)
                    recordAtributos = get_Lista_From_Char_Vector(record)
                    for i in range(len(recordAtributos)):
                        self.result_table.setItem(0, i, QTableWidgetItem(str(recordAtributos[i])))

            self.footer_label2.setText(f"{self.total:.2f} milisec")
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
