#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
namespace py = pybind11;
using namespace std;

struct Record {
    vector<vector<char>> record;
    Record(vector<vector<char>> atributos) : record(atributos) {}
    Record() = default;

};

class FixedFILE {
private:
    string filename;

public:
    FixedFILE() = default;
    FixedFILE(string name): filename(name) {
        ifstream infile(name, ios::binary | ios::in);
        if (infile.is_open()) {
            infile.close();
            cerr << "Archivo ya creado" << endl;
        } else {
            cerr << "!!Creando el archivo" << endl;
            infile.close();
            ofstream outfile(name, ios::binary | ios::out);
            outfile.close();
        }


    };
    void writeRecords(Record rec) {
        ofstream datafile(filename, ios::binary |ios::out|ios::app);
        if (!datafile.is_open()) {
            cerr << "Error al abrir el archivo para escritura" << endl;
            return;
        }
        long numAtributos = rec.record.size();
        datafile.write((char*)(&numAtributos), sizeof(numAtributos));
        for (auto atributo : rec.record) {
            long length = atributo.size();
            datafile.write((char*)(&length), sizeof(length));
            datafile.write((char*)(atributo.data()), length);
        }
    }
    vector<Record> readAll() {
        vector<Record> res;
        ifstream datafile(filename, ios::binary | ios::in);
        if (!datafile.is_open()) {
            cerr << "Error al abrir el archivo para lectura" << endl;
            return res;
        }
        datafile.seekg(0, ios::beg);
        while (!datafile.eof()) {
            long numAtributos;
            datafile.read((char*)(&numAtributos), sizeof(numAtributos));
            if (datafile.eof()) break;
            vector<vector<char>> atributos(numAtributos);
            for (long i = 0; i < numAtributos; ++i) {
                long length;
                datafile.read((char*)(&length), sizeof(length));
                atributos[i].resize(length);
                datafile.read((char*)(atributos[i].data()), length);
            }
            res.push_back(Record(atributos));
        }
        datafile.close();
        return res;
    }
};



PYBIND11_MODULE(prueba, handle){
    py::class_<Record>(handle, "Record")
        .def(py::init<>())
        .def(py::init<vector<vector<char>>>())
        .def_readwrite("record", &Record::record);
    py::class_<FixedFILE>(handle, "FixedFILE")
        .def(py::init<>())
        .def(py::init<string>())
        .def("writeRecords", &FixedFILE::writeRecords)
        .def("readAll", &FixedFILE::readAll);

}


// int main() { Para probar en clion
//     FixedFILE file("data.dat");
//     vector<vector<char>> atributos = {
//         {'J', 'u', 'a', 'n'},  // Nombre
//         {'2', '5'},            // Edad
//         {'3'},                 // Años
//         {'1', '0'}             // Ciclo
//     };
//     Record rec(atributos);
//     file.writeRecords(rec);
//     auto registros = file.readAll();
//     for (const auto& record : registros) {
//         cout << "Registro:" << endl;
//         for (const auto& atributo : record.record) {
//             for (const char& c : atributo) {
//                 cout << c;
//             }
//             cout << endl;
//         }
//         cout << "-------------------" << endl;  // Separador entre registros
//     }
//
//
//
//
//
//     return 0;
// }
