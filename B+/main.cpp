#include "implementation.h"

int main(){
    string Filename = "data.dat"; string IndexFilaname = "index.dat";
    CrearArchivo(Filename, IndexFilaname);
    BPlusTree arbol;
    string csvFile = "test.csv";
    arbol.ReadCSV(csvFile, NCOLS);
    cout << "acabo de scanear el csv" << endl;
    cout << "leera valores" << endl;
    arbol.ReadValues();
    return 0;
}
int oc(){
    string index = "index.dat";
    string data  = "data.dat";
    ifstream DataFile; DataFile.open(data, ios::binary);
    Record r;
    DataFile.seekg(15348);
    DataFile.read((char *)&r, NCOLS*255);

    cout << "r.showData(): ";r.showData(); cout << endl;
    DataFile.close();
    // ifstream IndexFile; IndexFile.open(index, ios::binary);
    // Index prueba;
    // IndexFile.seekg(52);
    // IndexFile.read((char *)&prueba, sizeof(Index));
    
    // cout << "prueba.isLeaf: " << prueba.isLeaf << endl;
    // cout << "prueba.nRegistros: " << prueba.nRegistros << endl;
    

    // IndexFile.close();
    return 0;
}
