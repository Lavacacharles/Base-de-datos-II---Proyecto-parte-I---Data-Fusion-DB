# import info
import prueba 
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
            "insert into table Order from file('C:\\data.csv')\nusing index hash;\n\nselect * from Order;")
        print(area_texto)
        # Botón Ejecutar

        ejecutar = QPushButton('Ejecutar')

        # Función para obtener y mostrar el texto en consola
        def ejecutar_comando():
            texto = area_texto.toPlainText()
            print("Texto en el área de texto:", texto)

        # Conectar el botón a la función ejecutar_comando
        ejecutar.clicked.connect(ejecutar_comando)
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
        result_table = QTableWidget(num_rows, num_columns)

        # Añadir encabezados
        result_table.setHorizontalHeaderLabels(headers_vector)
        
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
                result_table.setItem(row, i, QTableWidgetItem(str(recordAtributos[i])))

        result_layout.addWidget(result_table)
        result_tab.setLayout(result_layout)

        # Añadir las pestañas
        tabs.addTab(result_tab, "Result")
        tabs.addTab(explain_tab, "Explain")
        tabs.addTab(transx_tab, "Transx")

        # Añadir todos los layouts al layout principal
        layout_principal.addLayout(layout_superior)
        layout_principal.addWidget(tabs)

        # Establecer el layout de la ventana principal
        self.setLayout(layout_principal)


if __name__ == '__main__':
    app = QApplication(sys.argv)
    ventana = MiSGDB()
    ventana.show()
    sys.exit(app.exec_())
