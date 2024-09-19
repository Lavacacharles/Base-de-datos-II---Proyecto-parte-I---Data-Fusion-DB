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
    vector<int> freeList;
    BPlusTree(){
        pos_root = 0;
        filename = "data.dat";
        Indexfilename = "index.dat";
        WriteRoot(pos_root);
        freeList.push_back(sizeof(Index));
    }
    BPlusTree(string filename_, string Indexfilename_): filename(filename_), Indexfilename(Indexfilename_), pos_root(0){}
    
    void AjustarFreeList();

    void WriteRoot(int);
    void WriteIndex(int, Index);
    Index ReadIndex(int);
    
    void WriteRecord(int, Record);
    Record ReadRecord(int);

    void WriteBucket(int,Bucket);
    Bucket ReadBucket(int);
    Record* ReadListRecord(int,int[]);

    void SplitChild(int,Index,int);
    void InsertNonFullNode(int, Index, Record);
    void Insert(Record NuevoRegistro);
    void Search(){}
    void Eliminate(){}
    void ReadCSV(string filename, int nelements, BPlusTree tree);
};