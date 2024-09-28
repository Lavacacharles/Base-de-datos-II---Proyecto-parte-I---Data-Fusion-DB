#include <iostream>
#include <chrono>
#include "SequentialFile.h"

using namespace std;
using namespace std::chrono;

template<typename T>
void measureTime(SequentialFile<Record, T>& seqFile, vector<Record>& data, const string& operation) {
    auto start = high_resolution_clock::now();
    if (operation == "add") {
        for (const auto& record : data) {
            seqFile.add(record);
        }
    } else if (operation == "search") {
        for (const auto& record : data) {
            seqFile.search(record.key);
        }
    }  else if (operation == "rangeSearch") {
        seqFile.rangeSearch(data[0].key, data[data.size()/2].key);
    } else if (operation == "remove") {
        for (const auto& record : data) {
            seqFile.remove(record.key);
        }
    } 


    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    
    cout << "Tiempo de ejecución para " << operation << ": " << duration.count() << " ms" << endl;
};

int main() {
    SequentialFile<Record, int> seqFile;
    
    // Generar datos de prueba (puedes ajustar el tamaño para hacer diferentes pruebas)
    vector<Record> testData;

    for (int i = 1; i <= 100000 ; i++) {
        string s1 = "Nombre" + to_string(i);
        string s2 = "Producto" + to_string(i);
        string s3 = "Apellido" + to_string(i);
        testData.push_back(Record(i, (char *)&s1, (char *)&s2, (char *)&s3, i * 2, i*5));
    }

    // Medir el tiempo de ejecución para la operación 'add'
    measureTime(seqFile, testData, "add");
    // Medir el tiempo de ejecución para la operación 'search'
    measureTime(seqFile, testData, "search");

    measureTime(seqFile, testData, "rangeSearch");


    // Medir el tiempo de ejecución para la operación 'remove'
    measureTime(seqFile, testData, "remove");

    return 0;
}
