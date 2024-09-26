#include "implementation.h"

int main(){
    string Filename = "data.dat"; string IndexFilaname = "index.dat";
    CrearArchivo(Filename, IndexFilaname);
    BPlusTree arbol;
    string csvFile = "pruebas.csv";
    arbol.ReadCSV(csvFile, 5);
    arbol.ReadValues();
    return 0;
}