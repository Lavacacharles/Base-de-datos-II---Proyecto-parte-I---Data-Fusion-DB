#include <cstdio>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <map>
#include <functional>

#define MAX_B 4
const int N_BUCKETS = 10;
using namespace std;

struct Record {
     std::array<char, 10> codigo;
    std::array<char, 20> nombre;
    std::array<char, 20> apellido;
    int ciclo;
    bool operator==(const Record& other) const {
        return std::string(nombre) == other.nombre;
    }
    void setData(ifstream &file) {
        file.getline(codigo, 10, ',');
        file.getline(nombre, 20, ',');
        file.getline(apellido, 20, ',');
        file>>ciclo; file.get();
    }

    void showData() {
        cout<<setw(10)<<left<<codigo;
        cout<<setw(10)<<left<<nombre;
        cout<<setw(10)<<left<<apellido;
        cout<<setw(10)<<left<<ciclo<<endl;
    }

};


int hasFunction(string name) { //Funcion hasheshita
    size_t value =  hash<string>{}(name);
    return value%MAX_B;
}


struct Bucket{
    std::array<Record, 4> records; // Example for 4 records per bucket
    int size;
    long next_bucket;

    Bucket(){
        size = 0;
        next_bucket = -1;
    }

    void showData(){
        cout<<"["<<endl;
        for(int i=0;i<size;i++)
            records[i].showData();
        cout<<"]"<<endl;
    }
};

class StaticHashFile {
private:
    string filename;
    int N;
public:
    StaticHashFile(string _filename, int _N): filename(_filename), N(_N){
        ofstream datafile(filename, ios::binary | ios::in | ios::out);
        if (!datafile.is_open()) {
            // Si no existe el archivo, lo creamos y llenamos los buckets
            ofstream datafile_new(filename, ios::binary | ios::out);
            Bucket bnull;
            for (int i = 0; i < _N; i++) {
                datafile_new.write((char*)&bnull, sizeof(bnull));  // Inicializar buckets
            }
            datafile_new.close();
            cout << "Archivo creado e inicializado con buckets vacios" << endl;
        }

        datafile.close();
    }

    void insert(Record record){
        fstream datafile(filename, ios::binary| ios::in | ios::out);
        //1- usando una funcion hashing ubicar la pagina de datos
        int pos = hasFunction(string(record.nombre));
        //2- leer la pagina de datos
        Bucket B;
        datafile.seekg(pos*sizeof(B), ios::beg);
        datafile.read((char*)&B, sizeof(B));
        //3- verificar si size < MAX_B, si es asi, se inserta en esa pagina y se re-escribe al archivo
        if (B.size< MAX_B) {
            B.records[B.size] = record;
            B.size++;
            //Set the initial position of the bucket:
            datafile.seekp(pos*sizeof(B));
            datafile.write((char*)&B, sizeof(B));
            // B.showData();
        } else {
            //4- caso contrario, crear nuevo bucket, insertar ahi el nuevo registro, y enlazar
            Bucket NB;
            NB.records[NB.size] = record;
            NB.size++;
            datafile.seekg(0, ios::end);
            long pos_new_bucket = datafile.tellp();
            datafile.write((char*)&NB, sizeof(NB));
            long current_pos = pos * sizeof(B);
            while (B.next_bucket != -1) {
                current_pos = B.next_bucket;
                datafile.seekg(B.next_bucket, ios::beg);
                datafile.read((char*)&B, sizeof(B));
            }
            B.next_bucket = pos_new_bucket;
            datafile.seekp(current_pos, ios::beg);
            datafile.write((char*)&B, sizeof(B));
        }
        datafile.close();
        //5- puede considerar el rehuso de buckets liberados por la eliminacion
        //Me rindo
    }

    Record search(string nombre){
        Bucket B;
        ifstream datafile(filename, ios::binary| ios::in);
        //1- usando una funcion hashing ubicar la pagina de datos
        int pos = hasFunction(string(nombre));
        datafile.seekg(pos*sizeof(B));
        //2- leer la pagina de datos, ubicar el registro que coincida con el nombre
        Record record;
        datafile.read((char*)&B, sizeof(B));
        while (true) {
            for (int i = 0; i < B.size; ++i) {
                if (string(B.records[i].nombre) == nombre) {
                    cerr<<"Registro encontrado exitosamente"<<endl;
                    return B.records[i];
                }
            }
            if (B.next_bucket != -1) {
                datafile.seekg(B.next_bucket, ios::beg);
                datafile.read((char*)&B, sizeof(B));
            } else {
                break;
            }
        }
        datafile.close();

        //3- si no se encuentra el registro en esa pagina, ir a la pagina enlazada iterativamente
        cerr << "No se encontrO el registro con el nombre: " << nombre << endl;
        return Record();  // Retorna un registro vacío
    }

    bool remove(string nombre){
        fstream datafile(filename, ios::binary| ios::in|ios::out);
        //1- usando una funcion hashing ubicar la pagina de datos
        int pos = hasFunction(nombre);
        //2- leer la pagina de datos, ubicar el registro que coincida con el nombre
        Bucket B;
        datafile.seekg(pos*sizeof(B));
        Record record;
        datafile.read((char*)&B, sizeof(B));
        int finded;
        long pos2;
        while (true) {
            for (int i = 0; i < B.size; ++i) {
                if (string(B.records[i].nombre) == nombre) {
                    B.size--;
                    if (B.size == 0) {
                        //Aqui ponemos lo del free list pero time no alcanzo
                    }
                    //Escribimos:
                    datafile.seekg(pos2*sizeof(B));
                    datafile.write((char*)&B, sizeof(B));
                    finded = 7;
                    break;
                }
            }
            if (finded == 7) {
                break;
            }
            if (B.next_bucket != -1) {
                pos2 = B.next_bucket;
                datafile.seekg(B.next_bucket, ios::beg);
                datafile.read((char*)&B, sizeof(B));
            } else {
                break;
            }
        }
        datafile.close();
        return true;
        //3- si no se encuentra el registro en esa pagina, ir a la pagina enlazada iterativamente
        //4- retirar el registro del bucket, y re-escribir la pagina en el archivo
        //5- si un bucket se queda sin registros, puede considerar su rehuso en la insercion        
    }

    void scanAll() {
        // 1- abrir el archivo de datos y mostrar todos los datos
        ifstream datafile(filename, ios::binary | ios::in);
        Bucket B;
        datafile.seekg(0, ios::beg);

        for (int i = 0; i < N; i++) {
            datafile.seekg(i*sizeof(B), ios::beg);
            datafile.read((char*)&B, sizeof(B));
            cout << "Bucket " << i << ":" << endl;
            if (B.size > 0) {
                B.showData();
            } else {
                cout << "Bucket vacío." << endl;
            }
            while (B.next_bucket != -1) {
                cout << "Siguiente bucket encadenado en posición: " << B.next_bucket << endl;
                datafile.seekg(B.next_bucket, ios::beg);
                datafile.read((char*)&B, sizeof(B));
                B.showData();
            }
        }

        datafile.close();
    }
    vector<Bucket> getAll() {
        // 1- abrir el archivo de datos y mostrar todos los datos
        ifstream datafile(filename, ios::binary | ios::in);
        Bucket B;
        datafile.seekg(0, ios::beg);
        vector<Bucket> res;

        for (int i = 0; i < N; i++) {
            datafile.seekg(i*sizeof(B), ios::beg);
            datafile.read((char*)&B, sizeof(B));
            // cout << "Bucket " << i << ":" << endl;
            if (B.size > 0) {
                res.push_back(B);
            } else {
                // cout << "Bucket vacío." << endl;
            }
            while (B.next_bucket != -1) {
                // cout << "Siguiente bucket encadenado en posición: " << B.next_bucket << endl;
                datafile.seekg(B.next_bucket, ios::beg);
                datafile.read((char*)&B, sizeof(B));
                res.push_back(B);

            }
        }
        
        datafile.close();
        return res;

    }

};


