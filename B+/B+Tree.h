#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>


#define KEY_INDEX 0 // indice de la llave de ordamiento
#define ORDER 5 // cantidad maxima de hijos por nodo
#define PAGE_SIZE 20 // cantidad maxima de registros pro pagina
#define NCOLS 1 // cantidad de columnas del csv
#define DATATYPE 1 // 1 la llave es numerica, 0 la llave es categorica

using namespace std;


struct Record {
    char** data;
    Record(){
        data = new char*[NCOLS];
        for(int i = 0; i < NCOLS; i++){
            data[i] = new char[255]();
        }
    }
    Record(int nelements, string *l){
        char **word = new char*[nelements];
        for(int i = 0; i < nelements; i++){
            word[i] = new char[255];
            strncpy(word[i], l[i].c_str(), 254);
            word[i][254] = '\0';
        }
        data = word;
        // showData();
    }
    ~Record(){
        // for(int i = 0; data[i] != nullptr; i++){

        //     delete data[i];
        // }
        // delete []data;
    }
    void showData(){
        bool start = false;
        for(int i = 0; i < NCOLS; i++){
            if(start) cout << ",";
            cout << string(data[i]);
            start = true;
        }
    }
};

struct PageRecord{
    int next;
    int prev;
    int nRegistros;
    Record registros[PAGE_SIZE];
    PageRecord(){
        next = -1;
        prev = -1;
        nRegistros = 0;
    }
};

struct Index {
    int posBuckets[ORDER];
    int posHijos[ORDER + 1]; // punteros
    int nRegistros;
    bool isLeaf;
    
    Index(){
        nRegistros = 0;
        isLeaf = true;
        for(int i = 0; i < ORDER; i++){
            posBuckets[i] = -1;
        }
        for(int i = 0; i < ORDER + 1; i++){
            posHijos[i] = -1;
        }
    }
};

struct BPlusTree{
    long pos_root;
    bool IsPage;
    string filename;
    string Indexfilename;
    vector<int> freeList;
    vector<int> IndexFreeList;
    // BPlusTree(){
    //     pos_root = 0;
    //     filename = "data.dat";
    //     Indexfilename = "index.dat";
    //     WriteRoot(pos_root);
    //     freeList.push_back(0);
    //     AjustarFreeList();
    //     IndexFreeList.push_back(sizeof(Index));
    // }
    BPlusTree(){
        IsPage = true;
        PageRecord Bucket;
        // Bucket.nRegistros += 1;
        pos_root = 0;
        filename = "data.dat";
        Indexfilename = "index.dat";
        // WriteRoot(pos_root);
        freeList.push_back(12 + 255*NCOLS*PAGE_SIZE);
        IndexFreeList.push_back(0);
        WriteBucket(pos_root, Bucket);

        // AjustarFreeList();
        
    }
    BPlusTree(bool IsPage_){
        if(IsPage_){
            IsPage = true;
            PageRecord Bucket;
            Bucket.nRegistros += 1;
            pos_root = 0;
            filename = "data.dat";
            Indexfilename = "index.dat";
            // WriteRoot(pos_root);
            freeList.push_back(12 + 255*NCOLS);
            IndexFreeList.push_back(0);
            WriteBucket(pos_root, Bucket);
            // AjustarFreeList();
        }
        else {
            IsPage = false;
            // pos_root = 0;
            filename = "data.dat";
            Indexfilename = "index.dat";
            pos_root = 0;
            // WriteRoot(pos_root);
            // freeList.push_back(12 + 255*NCOLS);
            // IndexFreeList.push_back(0);
            // WriteBucket(pos_root, Bucket);
            // AjustarFreeList();
        }
    }
    BPlusTree(string filename_, string Indexfilename_): filename(filename_), Indexfilename(Indexfilename_), pos_root(0){}
    
    void AjustarFreeList();
    void AjustarIndexFreeList();

    void WriteRoot(int);
    void WriteIndex(int, Index);
    Index ReadIndex(int);
    
    void WriteRecord(int, Record);
    Record ReadRecord(int);

    void WriteBucket(int, PageRecord);
    PageRecord ReadBucket(int);
    Record* ReadListRecord(int,int[]);

    void SplitChild(int,Index,int);
    pair<int,Index> SplitRoot(int,Index);
    
    void InsertNonFullNode(int, Index, Record);

    
    void Insert(Record);
    pair <int, Record> Search(string); // El string lo manejamos como cadena o numero segun el tipo de dato de la llave de ordamiento
    vector<Record> RangeSearch(string, string);
    void Eliminate(){}
    void ReadCSV(string filename, int nelements);
    void ReadValues(); // In order
};