#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>


#define KEY_INDEX 2
#define ORDER 5
#define PAGE_SIZE 20
#define NELEMENTS 4

using namespace std;


struct Record {
    char** data;
    Record(){}
    Record(int nelements, string l[]){
        char **word = new char*[nelements];
        for(int i = 0; i < nelements; i++){
            word[i] = new char[255];
            strncpy(word[i], l[i].c_str(), sizeof(word[i]));
        }
        showData();
    }
    ~Record(){
        for(int i = 0; data[i] != nullptr; i++){
            delete data[i];
        }
        delete []data;
    }
    void showData(){
        bool start = false;
        for(int i = 0; data[i] != nullptr; i++){
            if(start) cout << ",";
            cout << data[i];
            start = true;
        }
    }
};


struct Bucket{
    Record registros[ORDER];
};


struct Index {
    int posBuckets[ORDER];
    int posHijos[ORDER + 1]; // punteros
    int nRegistros;
    bool isLeaf;
    int next;
    
    Index(){
        nRegistros = 0;
        isLeaf = true;
        next = -1;
    }
};

struct BPlusTree{
    long pos_root;
    string filename;
    string Indexfilename;
    BPlusTree(){
        pos_root = 0;
        filename = "data.dat";
        Indexfilename = "index.dat";
        WriteIndex(pos_root);
    }
    BPlusTree(string filename_, string Indexfilename_): filename(filename_), Indexfilename(Indexfilename_), pos_root(0){}
    
    void WriteIndex(int);
    Index ReadIndex(int);

    Record ReadRecord(int);
    
    void Search(){}

    void InsertNonFullNode(Index, int);
    void Insert(Record NuevoRegistro);
    void Eliminate(){}
    void ReadCSV(string filename, int nelements, BPlusTree tree);
};

struct Record{
    int sueldo;
    int id;
};

struct PaginaRegistro{
    Record registros[ORDER];
    int cantidadRegistros;
    int siguiente;
    int anterior;
};

struct Indice{
    bool esHoja;
    int cantidadIndices;
    int Llaves[ORDER];
    int posHijos[ORDER + 1];
    int posRegistros[ORDER];
};

struct ArbolBMas{
    long pos_root;
    string IndexFilename;
    string Filename;



    vector<Record> getAllMaximus(){
        ifstream IndexFile; IndexFile.open(IndexFilename, ios::binary);
        Indice IndexPage;
        IndexFile.seekg(pos_root);IndexFile.read((char *)&IndexPage, sizeof(IndexPage));
        // IndexFile.close();
        while(!IndexPage.esHoja){
            IndexFile.seekg(IndexPage.posHijos[IndexPage.cantidadIndices - 1]);
            IndexFile.read((char *)&IndexPage, sizeof(IndexPage));
        }
        IndexFile.close();
        ifstream PageFile; PageFile.open(Filename, ios::binary);
        PageFile.seekg(IndexPage.posRegistros[IndexPage.cantidadIndices - 1]);
        PaginaRegistro Page;
        PageFile.read((char *)&Page, sizeof(Page));
        int i = Page.cantidadRegistros - 1;
        int sueldoMaximo = Page.registros[i].sueldo;
        vector<Record> gananMax;
        while(Page.registros[i].sueldo == sueldoMaximo){
            i -= 1;
            gananMax.push_back(Page.registros[i]);
            if( i == -1){
                PageFile.seekg(Page.anterior);
                PageFile.read((char *)&Page, sizeof(Page));
                i = Page.cantidadRegistros - 1;
            }
        }
        return gananMax;
    }
};

// struct RecordLog{ 
//     int idTx; //Id de la transacción
//     TYPE type;//BT, COMMIT, ABORT, WRITE, CP, 
//     //TODO: Complete los campos
// };

// void undo_tx(RecordLog record); //Ya esta implementado, solo usar 
// void redo_tx(RecordLog record); //Ya esta implementado, solo usar



// void recoveryCheckPoint(vector<RecordLog> log, vector<int> recomienzo)
// {    
//     vector <RecordLog> Undo;
//     vector <RecordLog> Redo;
//     unordered_set<int> undoI;
//     for (int i = log.size() - 1; i > -1; i--){
//         if(log[i].type == "COMMIT"){
//             Redo.push_back(log[i]);
//             if(undoI.find(i) !=undoI.end()){
//                 undoI.erase(i);
//             }
//         }else {
//             undoI.insert(i);
//         }
//     }
//     for(int i: undoI){
//         undo_tx(log[i]);
//     }
//     for(RecordLog logr: Redo){
//         redo_tx(logr);
//     }

// }

