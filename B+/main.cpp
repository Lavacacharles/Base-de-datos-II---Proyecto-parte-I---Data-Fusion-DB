#include "implementation.h"

int main(){
    string Filename = "data.dat"; string IndexFilaname = "index.dat";
    CrearArchivo(Filename, IndexFilaname);
    BPlusTree arbol;
    string csvFile = "test.csv";
    arbol.ReadCSV(csvFile, NCOLS);
    cout << "acabo de scanear el csv" << endl;
    // PageRecord b = arbol.ReadBucket(0);

    // cout << "Leer prueba " << endl;
    // cout << b.nRegistros << endl;
    // for(int i = 0; i < b.nRegistros; i++){
    //     cout << endl;
    //     b.registros[i].showData();
    //     cout << endl;
    // }
    cout << "leera valores" << endl;
    arbol.ReadValues();
    return 0;
}
int oc(){
    string filename = "data.dat";
    ifstream Page; Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "Lee mal el registro\n";
        exit(0);
    }
    Record registro;
    int posRecord = 0;
    Page.seekg(posRecord);
    cout << "Leyendo un registro, posRecord: " << posRecord << endl;
    char buff[255] = {0};
    for(int i = 0; i < NCOLS; i++){
        Page.read(buff, 255);
        cout << "buff: " << buff << endl;
        strncpy(registro.data[i], buff, 254);
        registro.data[i][254] = '\0';
    }
    Page.close();

    return 0; 

}