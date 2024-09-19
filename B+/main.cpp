#include "B+Tree.h"

void BPlusTree::WriteRoot(int posRoot){
    fstream Page; Page.open(Indexfilename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posRoot;
        exit(0);
    }
    Page.seekp(posRoot);
    Index NuevoIndex;
    Page.write(reinterpret_cast<char *>(&NuevoIndex), sizeof(Index));
    Page.close();
};

void BPlusTree::AjustarFreeList(){
    if(freeList.size() != 1){
        freeList.pop_back();
        return;
    }
    ifstream IndexPage; IndexPage.open(Indexfilename, ios::binary | ios::ate);
    if (!IndexPage.is_open()) {
        cerr << "Error  ajustando la free list" << endl;
        exit(0);
    }
    streampos LastPos = IndexPage.tellg();
    IndexPage.close();
    freeList[0] = LastPos;
}

Record* BPlusTree::ReadListRecord(int nElements, int posBuckets[]){
    Record registros[ORDER];
    for(int i = 0; i < nElements; i++){
        registros[i] = ReadRecord(posBuckets[i]);
    }
    return registros;
};

void BPlusTree::WriteBucket(int posBucket, Bucket NuevoBucket){
    ofstream Page(filename); Page.open(filename, ios::binary | ios::in | ios::out);
    if (!Page.is_open()) {
        cerr << "Fallo escribiendo bucket\n";
        exit(0);
    }
    Page.seekp(posBucket);
    Page.write(reinterpret_cast<char*>(&NuevoBucket), sizeof(Bucket));
    Page.close();
};

Bucket BPlusTree::ReadBucket(int posBucket){
    ifstream Page(filename); Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "No se pudo abrir el archivo csv\n";
        exit(0);
    }
    Bucket buffer;
    Page.seekg(posBucket);
    Page.read(reinterpret_cast<char*>(&buffer), sizeof(Bucket));
    Page.close();

    return buffer;
};

Record BPlusTree::ReadRecord(int posRecord){
    ifstream Page(filename); Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "No se pudo abrir el archivo csv\n";
        exit(0);
    }
    Record registro;
    Page.seekg(posRecord);
    Page.read(reinterpret_cast<char*>(&registro), sizeof(Record));
    Page.close();

    return registro;
};

void BPlusTree::WriteRecord(int posNuevoRecord, Record NuevoRecord){
    fstream Page; Page.open(filename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la escritur de record " << posNuevoRecord;
        exit(0);
    }
    Page.seekp(posNuevoRecord);
    Page.write(reinterpret_cast<char *>(&NuevoRecord), sizeof(Record));
    Page.close();
};

void BPlusTree::WriteIndex(int posIndex, Index NuevoIndex){
    fstream Page; Page.open(Indexfilename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posIndex;
        exit(0);
    }
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

void BPlusTree:: ReadCSV(string filename, int nelements, BPlusTree tree){
    ifstream file(filename);
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
};

void BPlusTree::SplitChild(int posIndex, Index node, int indexHijo) {
    Index tempChild = ReadIndex(node.posHijos[indexHijo]);
    Index tempHalf;
    tempHalf.isLeaf = tempChild.isLeaf;
    int mid = ORDER / 2;
    for (int i = mid; i < tempChild.nRegistros; i++) {
        tempHalf.posBuckets[i - mid] = tempChild.posBuckets[i];
    }

    if (!tempChild.isLeaf) {
        for (int i = mid; i < tempChild.nRegistros + 1; i++) {
            tempHalf.posHijos[i - mid] = tempChild.posHijos[i];
        }
    }
    for(int k = mid; k < ORDER; k++){
        tempChild.posBuckets[k] = -1;
    }
    for(int k = mid + 1; k < ORDER + 1; k++){
        tempChild.posHijos[k] = -1;
    }

    node.posBuckets[indexHijo] = tempChild.posBuckets[mid];
    int posTempHalf = freeList[freeList.size() - 1];
    node.posHijos[indexHijo + 1] = posTempHalf;
    WriteIndex(posIndex, node);
    WriteIndex(node.posHijos[indexHijo], tempChild);
    WriteIndex(posTempHalf, tempHalf);
    AjustarFreeList();
};


void BPlusTree::InsertNonFullNode(int posNode, Index node, Record NuevoRegistro) {
    int i = node.nRegistros - 1;
    string value = string(NuevoRegistro.data[KEY_INDEX]);
    
    if (node.isLeaf) {
        Bucket keys = ReadBucket(node.posBuckets[0]);
        while (i >= 0 && value < string(keys.registros[i].data[KEY_INDEX])) {
            keys.registros[i + 1] = keys.registros[i];
            node.posBuckets[i + 1] = node.posBuckets[i] + sizeof(Record); 
            i--;
        }
        node.posBuckets[i + 1] = node.posBuckets[i] + sizeof(Record);
        keys.registros[i + 1] = NuevoRegistro;
        WriteIndex(posNode, node);
        WriteBucket(node.posBuckets[0], keys);
    } else {
        Record *keys = ReadListRecord(node.nRegistros, node.posBuckets);
        while (i >= 0 && value < string(keys[i].data[KEY_INDEX])) {
            i--;
        }
        i++;
        Index hijoIndex = ReadIndex(node.posHijos[i]);
        int posHijoIndex = node.posHijos[i];
        if (hijoIndex.nRegistros == ORDER) {
            SplitChild(posNode, node, i);
            if (value > keys[i].data[KEY_INDEX]) {
                i++;
            }
        }
        InsertNonFullNode(posHijoIndex, hijoIndex, NuevoRegistro);
    }
};

pair<int,Index> BPlusTree::SplitRoot(int posNode, Index node) {
    Index temp;
    temp.isLeaf = false;
    temp.posHijos[temp.nRegistros] = posNode;
    node = temp;
    int posTemp = freeList[freeList.size() - 1];
    WriteIndex(posTemp, temp);
    AjustarFreeList();
    SplitChild(posTemp, temp, 0);
    return {posTemp, temp};
}
void BPlusTree::Insert(Record NuevoRegistro) {
    Index node = ReadIndex(pos_root);
    string value = string(NuevoRegistro.data[KEY_INDEX]);

    if (ORDER == node.nRegistros) {

        Index temp; int posTemp;
        pair<int, Index> InfoTemp = SplitRoot(0, node);
        posTemp = InfoTemp.first; temp = InfoTemp.second;
        InsertNonFullNode(posTemp, temp, NuevoRegistro);
    } else {
        InsertNonFullNode(0, node, NuevoRegistro);
    }
};


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