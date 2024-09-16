#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#define BUCKET_SIZE = 50

using namespace std;


struct Record {
    vector<vector<char>>data;
    Record(){}
    Record(string line, int nparameters){
        stringstream ss(line);
        string token;
        Record record;
        data = vector<vector<char>>(nparameters);

        for(int i = 0; i < nparameters; i++){
            getline(ss, token, ',');
            data[i] = vector<char>(token.length());
            char *buffer = new char[token.length()];
            strncpy(buffer, token.c_str(), token.length());
            for(int j = 0; j < token.length(); j++){
                data[i][j] = buffer[j];
            }
            // data[i][data[i].size() - 1] = '\0';  // Asegurar el null-termination
            
        }
    }
    int size(){
        int size = 0;
        for(vector<char> word: data){
            size += word.size();
        }
        return size + data.size();
    }
};

struct Index {
    int recordSize;
    int recordPos;
    int next;
    Index(){}
    Index(int recordSize_, int recordPos_, int next_):recordSize(recordSize_), recordPos(recordPos_), next(next_){}
    Index(string line){
        cout << line << endl;
        recordPos = 0;

        stringstream ss(line);
        string token;
        Record record;

        getline(ss, token, ',');
        recordSize = stoi(token);

        getline(ss, token, ',');
        recordPos = stoi(token);

        getline(ss, token, ',');
        next = stoi(token);

        // getline(ss, token, ',');
        // strncpy(record.nombre, token.c_str(), sizeof(record.nombre));
        // record.nombre[sizeof(record.nombre) - 1] = '\0';  // Asegurar el null-termination
        // strncpy(reinterpret_cast<char *>(&recordSize), line.substr(recordPos, recordPos + sizeof(recordSize)).c_str(), sizeof(recordSize));
        // strncpy(reinterpret_cast<char *>(&recordPos), line.substr(recordPos, recordPos + sizeof(recordPos)).c_str(), sizeof(recordPos));
        // strncpy(reinterpret_cast<char *>(&next), line.substr(recordPos, recordPos + sizeof(next)).c_str(), sizeof(next));
    }
};

Record readRecord(string filename, string Indexfilename, int index, int nparameters){
    ifstream IndexPage; IndexPage.open(Indexfilename, ios::binary);
    if(!IndexPage.is_open()){
        cerr << "Fallo leyendo index";
        exit(0);
    }

    string line;
    IndexPage.seekg(index * sizeof(Index), ios::beg);
    // getline(IndexPage, line);
    int readRecordSize, readRecordPos, readNext;
    IndexPage.read(reinterpret_cast<char*>(&readRecordSize), sizeof(readRecordSize));
    IndexPage.read(reinterpret_cast<char*>(&readRecordPos), sizeof(readRecordPos));
    IndexPage.read(reinterpret_cast<char*>(&readNext), sizeof(readNext));
    Index node(readRecordSize, readRecordPos, readNext);

    IndexPage.close();
    cout << "b1" << endl;

    ifstream Page; Page.open(filename, ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo leyendo data";
        exit(0);
    }

    string line2;
    Page.seekg(node.recordPos * sizeof(Index));
    getline(IndexPage, line2);
    cout << line << endl;
    Record registro(line2, nparameters);
    Page.close();
    cout << "b2" << endl;

    return registro;
}

Index writeIndex(string filename, string Indexfilename, int recordSize, int recordPos, int next, int index, bool replace){
    ofstream IndexPage; IndexPage.open(Indexfilename, ios::binary | ios::in | ios::out);
    if(!IndexPage.is_open()){
        cerr << "Fallo escribiendo index";
        exit(0);
    }
    IndexPage.seekp(index * sizeof(Index), ios::beg);
    
    char salto = '\n';
    char des[1];
    des[0] = 'a';
    
    cout << recordSize << " " << recordPos << " " << next << endl;
    cout << "writting" << endl;
    IndexPage.write(reinterpret_cast<char *>(&recordSize), sizeof(recordSize));
    IndexPage.write(reinterpret_cast<char *>(&recordPos), sizeof(recordPos));
    IndexPage.write(reinterpret_cast<char *>(&next), sizeof(next));
    if(!replace) {
        IndexPage.write(&salto, sizeof(salto));
    }
    cout << "end writting" << endl;
    IndexPage.close();

    Index node(recordSize, recordPos, next);
    return node;
}

void writeRecord(string filename, string Indexfilename, Record &registro, int index, bool replace){
    ofstream Page; Page.open(filename, ios::binary | ios::in | ios::out);
    if(!Page.is_open()){
        cerr << "Fallo leyendo data";
        exit(0);
    }
    string line;
    Page.seekp(index * sizeof(Index));
    
    char salto[1];
    salto[0] = '\n';
    
    char delimitador[1];
    delimitador[0] = ',';
    for(int i = 0; i < registro.data.size(); i++){
        for(int j = 0; j < registro.data[i].size(); j++){
            Page.write(reinterpret_cast<char * >(&registro.data[i][j]), sizeof(registro.data[i][j]));
        }
        Page.write(reinterpret_cast<char * >(&delimitador), sizeof(delimitador));
    }
    cout << "a1" << endl;
    if(!replace){
        Page.write(reinterpret_cast<char * >(&salto), sizeof(salto));
    }
    cout << "a2" << endl;
    Page.close();
    cout << "a3" << endl;
}


void test(){ 

    string filename = "data.dat"; string Indexfilename = "index.dat";
    bool replace = false;
    int parameters;

    string line = "50061878,Marcos,Martín,5,";
    Record registro(line, 4);

    for(vector<char> word: registro.data){
        for(const char &character: word){
            cout << character;
        }
        cout << "\n";
    }

    int recordSize = registro.size();
    int recordPos = 0;
    int next = -1;
    int index = 0;
    int nparameters = 3;
    Index newIndex = writeIndex(filename,  Indexfilename, recordSize, recordPos, next, index, replace);

    // ifstream file; file.open(Indexfilename);
    // string s;
    // getline(file,s);
    // file.close();

    // int mo = -2;
    // strncpy(reinterpret_cast<char*>(&mo), s.substr(0,4).c_str(), sizeof(mo));
    // cout << "-" << endl;
    // cout << mo << endl;
    // cout << "-" << endl;
    // cout << "h1" << endl;
    // file.close();

    writeRecord(filename, Indexfilename, registro, index, replace);
    cout << "h2" << endl;

    Record registroLeido = readRecord(filename,  Indexfilename,  index, nparameters);
    cout << "h3" << endl;

    for(int i = 0; i < nparameters; i++){
        cout << i << endl;
    }
}