#include "implementation.h"

int main(){
    string Filename = "data.dat"; string IndexFilaname = "index.dat";
    CrearArchivo(Filename, IndexFilaname);
    BPlusTree arbol;
    string csvFile = "prueba.csv";
    arbol.ReadCSV(csvFile, 5);
    cout << "acabo de scanear el csv" << endl;
    PageRecord b = arbol.ReadBucket(0);

    cout << "Leer prueba " << endl;
    cout << b.nRegistros << endl;
    for(int i = 0; b.nRegistros; i++){
        cout << endl;
        b.registros[i].showData();
        cout << endl;
    }
    cout << "leera valores" << endl;
    arbol.ReadValues();
    return 0;
}