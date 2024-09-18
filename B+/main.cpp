#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#define ORDER_TREE 5

using namespace std;

//data.dat
struct Node{
    vector<vector<char>> data;
};

struct Bucket{
    // Record registro[];
};

// index.dat
struct Index{
    int hijos[ORDER_TREE + 1];
    int posRegistros[ORDER_TREE];
    int order;
    bool isLeaf;
};

struct BPlusTree{
    long pos_root;
};

struct Record{
    int ciclo;
    char nombre[20];
    char apellido[20];

    void showData(){
        cout << ciclo << " " << nombre << " " << apellido << endl;
    }
};

int otherFunc(int argc, char *argv[]){
    if(argc < 2){
        cerr << "Faltan parametros";
        exit(0);
    }
    string filename = string(argv[1]); string Indexfilename;
    ofstream Page; Page.open(filename, ios::binary);

    if(!Page.is_open()){
        cerr << "Falla iniciado b+";
        exit(0);
    }

    int ciclo = 4   ;
    char nombre[20] = "Roni";
    char apellido[20] = "Pacori";
 

    Page.write(reinterpret_cast<char*>(&ciclo), sizeof(ciclo));
    Page.write(nombre, sizeof(nombre));
    Page.write(apellido, sizeof(apellido));
    Page.close();

    ifstream checkPage; checkPage.open(filename, ios::binary);

    Record r;
    checkPage.read(reinterpret_cast<char*>(&r),sizeof(Record));

    checkPage.close();

    r.showData();
    return 0;
}


int main(){
    cout << sizeof(string);
    return 0;
}

