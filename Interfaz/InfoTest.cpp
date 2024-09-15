#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <string>
#include <cstring>

namespace py = pybind11;
using namespace std;

struct Record {
    string codigo;
    string nombre;
    string apellido;
    int ciclo;
    bool operator==(const Record& other) const {
        return std::string(nombre) == other.nombre;
    }
   
};

vector<Record> getAll(int n){

    vector<Record> res(n);
    Record record;
    for (int i =0; i <n; i++) {
        record.apellido = "ape"+to_string(i);
        record.codigo = "cod"+to_string(i);
        record.nombre = "nom"+to_string(i);
        record.ciclo = i;
        res[i] = record;

    }
    return res;
}

PYBIND11_MODULE(info, handle){

    handle.doc() = "This is the module docs"; 
     py::class_<Record>(handle, "Record")
        .def(py::init<>())  // Default constructor
        .def_readwrite("codigo", &Record::codigo)
        .def_readwrite("nombre", &Record::nombre)
        .def_readwrite("apellido", &Record::apellido)
        .def_readwrite("ciclo", &Record::ciclo);

    handle.def("get", &getAll);
}


