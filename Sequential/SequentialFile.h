#pragma once
#include <iostream>
#include <fstream>
#include <cstring>
#include <sstream>
#include <vector>
#include <filesystem>
#include <cmath>
#include "Record.h"
#include "functions.h"
#include "methods.h"
#include "parent.h"

using namespace std;

template<size_t N>
std::string to_string(const char (&arr)[N]) {
    return std::string(arr, std::find(arr, arr + N, '\0'));
}


template<typename R>
struct Entry {
    R record;
    long nextPF;
    bool isMain=false;
    Entry(){}
    
    Entry(R record_){
      this->record = record_;
    }
  public:
    void setData(){
        cout<<"ID:";
        cin>>record.key;
        cout<<"Nombre:";
        cin>>record.nombre;
        cout<<"Producto:";
        cin>>record.producto;
        cout<<"Precio:";
        cin>>record.precio;
        cout<<"Cantidad:";
        cin>>record.cantidad;
    }

    void showData(){
      cout << nextPF << " " << isMain << endl;
      record.print();
    }

};

template <typename R, typename T>
class SequentialFile: public MethodSelector<R>, public FileParent<T> {
private:
    string mainFilename;
    string auxFilename;

public:
    SequentialFile() {
        this->mainFilename = "sequential_datos.dat";
        this->auxFilename = "sequential_aux.dat";

        ifstream mainFile(this->mainFilename, ios::binary);
        if (!mainFile) {
            ofstream newMainFile(this->mainFilename, ios::binary);
            int nextPF = 0;
            bool isMain = true;
            newMainFile.write((char*)&nextPF, sizeof(int));
            newMainFile.write((char*)&isMain, sizeof(bool));
            newMainFile.close();
        }
    }

    SequentialFile(string mainFilename, string auxFilename) : FileParent<T>(mainFilename, mainFilename) {
        this->mainFilename = mainFilename + "_data";
        this->auxFilename = auxFilename + "_aux";

        ifstream mainFile(mainFilename, ios::binary);
        if (!mainFile) {
            ofstream newMainFile(mainFilename, ios::binary);
            int nextPF = 0;
            bool isMain = true;
            newMainFile.write((char*)&nextPF, sizeof(int));
            newMainFile.write((char*)&isMain, sizeof(bool));
            newMainFile.close();
        }

        this->create_headers_basic(2);
    }

    bool add(string recordS) {
        istringstream lineStream(recordS);
        string line, id = "", field;
        string data = "";
        int count = 0;
        while (getline(lineStream, field, ',')) {
            if (count == 0) {
                id = field;
            } else {
                if (data != "")
                    data += ",";
                data += field;
            }

            count++;
        }
        if (data != "" || id != "") {
            R record(id, data.c_str());
            this->add(record);
        }
    }

    bool add(R Nuevorecord) override {

        Entry<R> NuevoRegistro(Nuevorecord);

        ifstream mainp(mainFilename, ios::binary| ios::app);
        if (!mainp.is_open()) return false;
        mainp.seekg(0,ios::end);
        int TMain =  mainp.tellg();
        int SizeMain=(TMain - sizeof(int) - sizeof(bool))/sizeof(Entry<R>);
        if (TMain <= sizeof(int) + sizeof(bool)) {
            NuevoRegistro.nextPF = -1;
            NuevoRegistro.isMain = true;
            ofstream mainFile(mainFilename, ios::binary | ios::app);
            mainFile.write((char*)&NuevoRegistro, sizeof(Entry<R>));
            mainFile.close();
            return true;
        }
        mainp.close();

        ifstream mainFile(mainFilename, ios::binary| ios::app);
        int HnextPF;
        bool HisMain;
        mainFile.read((char*)&HnextPF, sizeof(int));
        mainFile.read((char*)&HisMain, sizeof(bool));
        Entry<R> FirstRecord;
        mainFile.read((char*)&FirstRecord, sizeof(Entry<R>));


        ifstream auxFile(auxFilename, ios::binary | ios::app);
        auxFile.seekg(0,ios::end);
        int SizeAux=auxFile.tellg()/sizeof(Entry<R>);

        // if(FirstRecord.id>=NuevoRegistro.id){ NuevoRegistro.id <= FirstRecord.id
        if ( menor_igual(to_string(NuevoRegistro.record.key), to_string(FirstRecord.record.key)) ) {
            if(HisMain){
                NuevoRegistro.nextPF=HnextPF;
                NuevoRegistro.isMain=HisMain;
                HnextPF=SizeAux;
                HisMain=false;

                ofstream auxFile2(auxFilename, ios::binary | ios::app);
                auxFile2.write((char*)&NuevoRegistro, sizeof(Entry<R>));
                auxFile2.close();

                ofstream mainFile(mainFilename, ios::binary | ios::in | ios::out);
                mainFile.write((char*)&HnextPF, sizeof(int));
                mainFile.write((char*)&HisMain, sizeof(bool));
                mainFile.close();
            }
            else{
                Entry<R> PrevioRecord;
                ifstream auxFile(auxFilename, ios::binary | ios::app);
                auxFile.seekg(HnextPF*sizeof(Entry<R>));
                auxFile.read((char*)&PrevioRecord,sizeof(Entry<R>));
                if ( menor(to_string(NuevoRegistro.record.key), to_string(PrevioRecord.record.key)) ) {
              //if(PrevioRecord.id>NuevoRegistro.id){ // NuevoRegistro.id < PrevioRecord.id
                    NuevoRegistro.nextPF=HnextPF;
                    NuevoRegistro.isMain=false;
                    HnextPF=SizeAux;
                }
                else{
                    int posPrevio=HnextPF;
                    Entry<R> SPrevioRecord;
                    auxFile.seekg(PrevioRecord.nextPF*sizeof(Entry<R>));
                    auxFile.read((char*)&SPrevioRecord,sizeof(Entry<R>));
                    while( menor_igual(to_string(SPrevioRecord.record.key), to_string(NuevoRegistro.record.key)) and !SPrevioRecord.isMain ) {
                    // while(SPrevioRecord.id<=NuevoRegistro.id and !SPrevioRecord.isMain){
                        posPrevio=PrevioRecord.nextPF;
                        PrevioRecord=SPrevioRecord;
                        auxFile.seekg(SPrevioRecord.nextPF*sizeof(Entry<R>));
                        auxFile.read((char*)&SPrevioRecord,sizeof(Entry<R>));
                    }
                    NuevoRegistro.nextPF=PrevioRecord.nextPF;
                    NuevoRegistro.isMain=PrevioRecord.isMain;
                    PrevioRecord.nextPF=SizeAux;
                    PrevioRecord.isMain=false;
                    ofstream auxFile3(auxFilename, ios::binary | ios::in | ios::out);
                    auxFile3.seekp(posPrevio*sizeof(Entry<R>));
                    auxFile3.write((char*)&PrevioRecord, sizeof(Entry<R>));
                    auxFile3.close();
                }

                ofstream mainFile(mainFilename, ios::binary | ios::in | ios::out);
                mainFile.write((char*)&HnextPF, sizeof(int));
                mainFile.write((char*)&HisMain, sizeof(bool));
                mainFile.close();

                ofstream auxFile3(auxFilename, ios::binary | ios::app);
                auxFile3.write((char*)&NuevoRegistro, sizeof(Entry<R>));
                auxFile3.close();
            }
            auxFile.close();
            mainFile.close();
            if(SizeAux+1>int(log(SizeMain))){
                merge(mainFilename,auxFilename);
            }
          return true;
        }
        int inicio = 0;
        mainFile.seekg(0, ios::end);
        int tamMain = mainFile.tellg() / sizeof(Entry<R>);
        int fin=tamMain;
        mainFile.seekg(0, ios::beg);

        Entry<R> record;
        Entry<R> PrevRecord;
        int medio;
        int pos;
        bool lastMain=true;
        while (inicio <= fin) {
            medio = (inicio + fin) / 2;
            mainFile.seekg(medio * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
            mainFile.read((char *) &record, sizeof(Entry<R>));
            if ( menor(to_string(record.record.key), to_string(NuevoRegistro.record.key)) ) {
            //if(record.id<NuevoRegistro.id){
                inicio = medio + 1;
                pos=medio;
                PrevRecord=record;
            }
            else{
                fin = medio - 1;
                pos=medio-1;
            }
            if ( igual_igual(to_string(record.record.key), to_string(NuevoRegistro.record.key)) ) {
            //if(record.id==NuevoRegistro.id){
                PrevRecord=record;
                pos=medio;
                break;
            }
            if(pos>tamMain-1){
                pos-=1;
            }
        }
        if(PrevRecord.nextPF==-2){
            pos-=1;
            mainFile.seekg(pos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.read((char *) &PrevRecord, sizeof(Entry<R>));
        }
        while ( !PrevRecord.isMain and menor(to_string(PrevRecord.record.key), to_string(NuevoRegistro.record.key)) ) {
        //while(!PrevRecord.isMain and PrevRecord.id<NuevoRegistro.id){
            lastMain=false;
            ifstream auxFile1(auxFilename, ios::binary | ios::app);
            auxFile1.seekg(PrevRecord.nextPF*sizeof(Entry<R>));
            auxFile1.read((char*)&record, sizeof(Entry<R>));
            auxFile1.close();
            if ( menor_igual( to_string(record.record.key), to_string(NuevoRegistro.record.key) ) ) {
            //if(record.id<=NuevoRegistro.id){
                pos=PrevRecord.nextPF;
                PrevRecord=record;
            }
            else{
                break;
            }
        }
        NuevoRegistro.nextPF=PrevRecord.nextPF;
        NuevoRegistro.isMain=PrevRecord.isMain;
        PrevRecord.nextPF=SizeAux;
        PrevRecord.isMain=false;
        if(lastMain){
            ofstream mainFile2(mainFilename, ios::binary | ios::in | ios::out);
            mainFile2.seekp(pos * sizeof(Entry<R>)+sizeof(int)+sizeof(bool),ios::beg);
            mainFile2.write((char*)&PrevRecord, sizeof(Entry<R>));
            mainFile2.close();
        } else{
            ofstream auxFile2(auxFilename, ios::binary | ios::in | ios::out);
            auxFile2.seekp(pos * sizeof(Entry<R>),ios::beg);
            auxFile2.write((char*)&PrevRecord, sizeof(Entry<R>));
            auxFile2.close();
        }
        ofstream auxFile2(auxFilename, ios::binary | ios::app);
        auxFile2.write((char*)&NuevoRegistro, sizeof(Entry<R>));
        auxFile2.close();


        auxFile.close();
        mainFile.close();

        if(SizeAux+1>int(log(SizeMain))){
            merge(mainFilename,auxFilename);
        }
        return true;
    }


    void merge(string &mainFilename, string &auxFilename) {
        ofstream outputFile("merge.dat", ios::binary);
        if (!outputFile.is_open()) {
          return;
        }
        // cout << "Here " << endl;

        ifstream mainFile(mainFilename, ios::binary);
        if (!mainFile.is_open()) {
          return;
        } 
        mainFile.seekg(0, ios::end);
        int fileSize = mainFile.tellg();

        if (fileSize == sizeof(bool)+sizeof(int)) {
            cout << "No existen registros en los archivos" <<endl;
            return;
        }

        mainFile.seekg(0, ios::beg);

        ifstream auxFile(auxFilename, ios::binary);
        if (!auxFile.is_open()) {
            int HnextPF;
            bool HisMain;
            mainFile.read((char*)&HnextPF, sizeof(int));
            mainFile.read((char*)&HisMain, sizeof(bool));

            outputFile.write((char*)&HnextPF, sizeof(int));
            outputFile.write((char*)&HisMain, sizeof(bool));

            int nextPos = HnextPF;
            int contador = 0;

            while(nextPos != -1){
                Entry<R> record;
                Entry<R> temp;

                mainFile.seekg(nextPos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
                mainFile.read((char*) &record, sizeof(Entry<R>));

                if (!record.isMain){
                        temp = record;
                        temp.isMain = true;
                        contador++;
                        if (record.nextPF == -1){contador = -1;}
                        temp.nextPF = contador;
                        outputFile.write((char*) &temp, sizeof(Entry<R>));
                }
                else{
                    contador++;
                    if (record.nextPF == -1){contador = -1;}
                    int tempnext = record.nextPF;
                    record.nextPF = contador;
                    outputFile.write((char*) &record, sizeof(Entry<R>));
                    record.nextPF = tempnext;
                    }

                nextPos = record.nextPF;
            }
        }
        else{
            int HnextPF;
            bool HisMain;
            mainFile.read((char*)&HnextPF, sizeof(int));
            mainFile.read((char*)&HisMain, sizeof(bool));

            outputFile.write((char*)&HnextPF, sizeof(int));
            outputFile.write((char*)&HisMain, sizeof(bool));

            int nextPos = HnextPF;
            bool Whcfile = HisMain;
            int contador = 0;
            while(nextPos != -1){
                Entry<R> record;
                Entry<R> temp;
                if (Whcfile){
                    mainFile.seekg(nextPos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
                    mainFile.read((char*) &record, sizeof(Entry<R>));

                    if (!record.isMain){
                        temp = record;
                        temp.isMain = true;
                        contador++;
                        if (record.nextPF == -1){contador = -1;}
                        temp.nextPF = contador;
                        outputFile.write((char*) &temp, sizeof(Entry<R>));
                    }
                    else{
                        contador++;
                        if (record.nextPF == -1){contador = -1;}
                        int tempnext = record.nextPF;
                        record.nextPF = contador;
                        outputFile.write((char*) &record, sizeof(Entry<R>));
                        record.nextPF = tempnext;
                    }
                }
                else {
                    auxFile.seekg(nextPos * sizeof(Entry<R>));
                    auxFile.read((char*) &record, sizeof(Entry<R>));

                    if (!record.isMain){
                        temp = record;
                        temp.isMain = true;
                        contador++;
                        if (record.nextPF == -1){contador = -1;}
                        temp.nextPF = contador;
                        outputFile.write((char*) &temp, sizeof(Entry<R>));
                    }
                    else {
                        contador++;
                        if (record.nextPF == -1){contador = -1;}
                        int tempnext = record.nextPF;
                        record.nextPF = contador;
                        outputFile.write((char*) &record, sizeof(Entry<R>));
                        record.nextPF = tempnext;
                    }
                }
                nextPos = record.nextPF;
                Whcfile = record.isMain;
            }
        }

        mainFile.close();
        auxFile.close();
        outputFile.close();

        std::remove(mainFilename.c_str());
        std::remove(auxFilename.c_str());
        std::rename("merge.dat", mainFilename.c_str());

        fstream main(mainFilename, ios::binary | ios::in | ios::out);
        if (!main.is_open()) {
          return;
        }

        int HnextPF = 0;
        int HisMain = 1;
        main.write((char*)&HnextPF, sizeof(int));
        main.write((char*)&HisMain, sizeof(bool));

        main.close();
    }

    bool remove(T key) override {
        merge(mainFilename,auxFilename);

        fstream mainFile(mainFilename, ios::binary| ios::in | ios::out);
        if (!mainFile.is_open()) {
          return false;
        }

        int inicio = 0;
        mainFile.seekg(0, ios::end);
        int fin = ((int)mainFile.tellg() - inicio) / sizeof(Entry<R>) - 1;
        mainFile.seekg(0, ios::beg);

        Entry<R> record;
        Entry<R> PrevRecord;
        int pos;
        while (inicio <= fin) {
            int medio = (inicio + fin) / 2;
            mainFile.seekg(medio * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
            mainFile.read((char*)&record, sizeof(Entry<R>));

            if ( igual_igual(record.record.key, key) ) {
            // if (record.record.key == key) {
                pos = medio;
                break;
            }
            else if ( menor(to_string(record.record.key), key) ) {
            // else if (record.record.key < key) {
                inicio = medio + 1;
                if ( menor(to_string(PrevRecord.record.key), to_string(record.record.key)) ) { //
                // if (key- record.record.key < key - PrevRecord.record.key) { //  record.key  > prevredc
                    PrevRecord = record;
                }
            } else {
                fin = medio - 1;
            }

            pos = -4;
        }

        if (pos == -4){
            cout<<"No existe el ID"<<endl;
            return false;
        }

        if (pos == 0){
            int HnextPF;
            HnextPF = 1;
            mainFile.seekp(0, ios::beg);
            mainFile.write((char*)&HnextPF, sizeof(int));

            Entry<R> rmrecord;
            mainFile.seekg(pos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.read((char*) &rmrecord, sizeof(Entry<R>));

            rmrecord.nextPF = -2;

            mainFile.seekp(pos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.write((char*) &rmrecord, sizeof(Entry<R>));
        }

        else{
            Entry<R> prevrecord;
            mainFile.seekg((pos-1) * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.read((char*) &prevrecord, sizeof(Entry<R>));

            Entry<R> rmrecord;
            mainFile.seekg(pos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.read((char*) &rmrecord, sizeof(Entry<R>));

            prevrecord.nextPF = rmrecord.nextPF;
            rmrecord.nextPF = -2;

            mainFile.seekp((pos-1) * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.write((char*) &prevrecord, sizeof(Entry<R>));

            mainFile.seekp(pos * sizeof(Entry<R>) + sizeof(int) + sizeof(bool), ios::beg);
            mainFile.write((char*) &rmrecord, sizeof(Entry<R>));
        }
        mainFile.close();
        return true;
    }

    string find(T key) {
        // TODO
        // read from output file
    }

    string search(T key) {
        vector<Entry<R>> result;
        ifstream mainFile(mainFilename, ios::binary);
        if (!mainFile.is_open()) R();

        int inicio = 0;
        mainFile.seekg(0, ios::end);
        int fin = ((int)mainFile.tellg() - inicio) / sizeof(Entry<R>) - 1;
        mainFile.seekg(0, ios::beg);

        Entry<R> record;
        Entry<R> PrevRecord;

        while (inicio <= fin) {
            int medio = (inicio + fin) / 2;
            mainFile.seekg(medio * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
            mainFile.read((char*)&record, sizeof(Entry<R>));

            if ( igual_igual(record.record.key, key) ) {
            // if (record.record.key == key) {
                if (record.nextPF == -2){
                    // cout << "El Entry se encuentra eliminado" <<endl;
                    return "Key not found";
                }
                else{
                    result.push_back(record);
                    return result[0].record.getData();
                }
            } 
            else if ( menor(to_string(record.record.key), key) ) {
            // else if (record.record.key < key) {
                inicio = medio + 1;
                if ( menor(to_string(PrevRecord.record.key), to_string(record.record.key)) ) {
                // if (key - record.record.key < key - PrevRecord.record.key) { // -recr < -prev  rec > prev
                    PrevRecord = record;
                }
            } else {
                fin = medio - 1;
            }
        }

        ifstream auxFile(auxFilename, ios::binary);
        if (!auxFile.is_open()) return "Key not found";

        while(auxFile.read((char*)&record, sizeof(Entry<R>))){
            if ( igual_igual(PrevRecord.record.key, key) ) {
            // if (PrevRecord.record.key == key) {
                result.push_back(PrevRecord);
                return result[0].record.getData();
            }
        }

        // cout << "No se encontro el ID" << endl;

        mainFile.close();
        auxFile.close();
        return "Key not found";
    }

    vector<string> range_search(T keyMin, T keyMax) {
        // TODO
        // Read from file
    }

    vector<string> rangeSearch(T beginkey, T endkey) {
        vector<string> result;
        ifstream mainFile(mainFilename, ios::binary);
        if (!mainFile.is_open()) throw runtime_error("No se pudo abrir el archivo main");

        ifstream auxFile(auxFilename, ios::binary);

        Entry<R> record;
        int inicio = 0;
        mainFile.seekg(0, ios::end);
        int fin = ((int)mainFile.tellg() - inicio) / sizeof(Entry<R>) - 1;
        mainFile.seekg(0, ios::beg);

        while (inicio <= fin) {
            int medio = (inicio + fin) / 2;
            mainFile.seekg(medio * sizeof(Entry<R>) + sizeof(int) + sizeof(bool));
            mainFile.read((char*)&record, sizeof(Entry<R>));
            
            if ( menor_igual(beginkey, record.record.key) ) {
            // if (record.record.key >= beginkey) { // beginkey =< record
                fin = medio - 1;
            } else {
                inicio = medio + 1;
            }
        }

        bool found = false;

        while ( menor_igual(record.record.key, endkey) ) {
        // while (record.record.key <= endkey) {
            if ( found && menor(record.record.key, beginkey) ){
            // if (found && record.record.key < beginkey) {
                break;
            }
            if ( menor_igual(beginkey, record.record.key) ) {
            // if (record.record.key >= beginkey) {
                result.push_back(record.record.getData());
                found = true;
            }
            if(record.nextPF==-1)break;
            if (record.isMain) {
                mainFile.seekg(record.nextPF * sizeof(Entry<R>)  + sizeof(int) + sizeof(bool));
                mainFile.read((char*)&record, sizeof(Entry<R>));
            } else {
                auxFile.seekg(record.nextPF * sizeof(Entry<R>));
                auxFile.read((char*)&record, sizeof(Entry<R>));

            }
        }

        mainFile.close();
        auxFile.close();
        return result;
    }

    vector<R> load() override {
        merge(mainFilename,auxFilename);

        ifstream file(mainFilename, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");
        vector<R> result;
        Entry<R> record;

        file.seekg(sizeof(int) + sizeof(bool), ios::beg);
        while(file.peek() != EOF){
            record = Entry<R>();
            file.read((char*) &record, sizeof(Entry<R>));
            if (record.nextPF != -2) {
                result.push_back(record.record);
            }
        }
        file.close();
        return result;
    }


    void display_all() override {
      vector<R>vec = load();
      for (int i = 0; i < vec.size(); i++) {
        //vec[i].showData();
          cout << vec[i].getData() << endl;
      }
      cout << endl;
      return;
    };
};
