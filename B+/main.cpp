#include "B+Tree.h"

void BPlusTree:: ReadCSV(string filename, int nelements, BPlusTree tree){
    ifstream file(filename);
    vector<Record> records;
    string line;

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo csv\n";
        exit(0);
    }
    bool start;
    getline(file, line);
    string *l = new string[nelements];
    while (getline(file, line)) {
        stringstream ss(line);
        string token;
        Record record;
        start = false;
        for(int i = 0; i < nelements; i++){
            if(i < nelements - 1) getline(ss, token, ',');
            else getline(ss, token, '\n');
            l[i] = token;
        }
        tree.Insert(Record(nelements, l));

    }
    delete l;
    l = nullptr;

    file.close();
}
void BPlusTree::WriteIndex(int posIndex){
    int posBuckets[ORDER];
    int posHijos[ORDER + 1];
    int nRegistros;
    bool isLeaf;
    int next;
    fstream Page; Page.open(Indexfilename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posIndex;
        exit(0);
    }
    Index NuevoIndex;
    Page.seekp(posIndex);
    Page.write(reinterpret_cast<char *>(&NuevoIndex), sizeof(Index));
    Page.close();
}
Index BPlusTree::ReadIndex(int posRecord){
    ifstream Page; Page.open(Indexfilename, ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posRecord;
        exit(0);
    }
    Page.seekg(posRecord);
    Index output; Page.read(reinterpret_cast<char *>(&output), sizeof(Index));
    Page.close();
    return output;
}


void BPlusTree::InsertNonFullNode(Index node, int value) {
    int i = node.nRegistros - 1;
    if (node.isLeaf) {
        while (i >= 0 && value < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = value;
    } else {
        while (i >= 0 && value < node->keys[i]) {
            i--;
        }
        i++;
        if (node->children[i]->keys.size() == node->M) {
            splitChild(node, i);
            if (value > node->keys[i]) {
                i++;
            }
        }
        insertNonFullNode(node->children[i], value);
    }
}

void BPlusTree::Insert(Record NuevoRegistro) {
    Index node = ReadIndex(pos_root);
    string value = string(NuevoRegistro.data[KEY_INDEX]);

    if (ORDER == node.nRegistros) {

        // Node* temp = splitRoot(root);
        // insertNonFullNode(temp, value);
    } else {
        // insertNonFullNode(node, value);
    }
}


void CrearArchivo(string filename, string Indexfilename){
    ofstream Page; Page.open(filename, ios::binary);
    Page.close();
    ofstream IndexPage; IndexPage.open(Indexfilename, ios::binary);
    IndexPage.close();
};

int TestFunction(int argc, char *argv[]){
    string filename; string Indexfilename;
    filename = "data.dat"; Indexfilename = "index.dat";
    // CrearArchivo(filename, Indexfilename);
    ifstream Page; Page.open(filename, ios::binary);
    if(!Page.is_open()){
        cerr << "Falla abriendo";
        exit(0);
    }

    char buffer[50];
    string line; getline(Page, line);
    strncpy(buffer, line.c_str(), sizeof(buffer));


    Page.close();

    cout << buffer << endl;


    return 0;
};



int main(){
    int nelements = 4;
    char **word = new char*[10];
    string l[] = {"hola", "como", "estas", "chicho"};
    for(int i = 0; i < nelements; i++){
        word[i] = new char[20];
        strncpy(word[i], l[i].c_str(), sizeof(word[i]));   
    }
    for(int i = 0; i < nelements; i++){
        cout << word[i] << endl;
    }
    
    
    for(int i = 0; i < nelements; i++){
        delete word[i];
    }
    delete []word;
    return 0;
}