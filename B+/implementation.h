#include "B+Tree.h"

void BPlusTree::WriteRoot(int posRoot){
    fstream Page; Page.open(Indexfilename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posRoot;
        exit(0);
    }
    Page.seekp(posRoot);
    Index NuevoIndex;
    NuevoIndex.posHijos[0] = 0; NuevoIndex.posHijos[1] = 12 + NCOLS*255;
    WriteBucket(0, PageRecord()); WriteBucket(12 + NCOLS*255, PageRecord()); 

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
    Record *registros = new Record[ORDER];
    for(int i = 0; i < nElements; i++){
        registros[i] = ReadRecord(posBuckets[i]);
    }
    return registros;
};

void BPlusTree::WriteBucket(int posBucket, PageRecord NuevoBucket) {
    fstream Page; Page.open(filename, ios::binary | ios::in | ios::out);
    if (!Page.is_open()) {
        cerr << "Fallo escribiendo bucket\n";
        exit(0);
    }

    Page.seekp(posBucket);

    Page.write((char*)&NuevoBucket.next, sizeof(int));
    Page.write((char*)&NuevoBucket.prev, sizeof(int));
    Page.write((char*)&NuevoBucket.nRegistros, sizeof(int));

    for (int i = 0; i < NuevoBucket.nRegistros; i++) {
        for (int j = 0; j < NCOLS; j++) {
            Page.write(NuevoBucket.registros[i].data[j], 255); // Escribir cada string de 255 caracteres
        }
    }
    Record emptyRecord; 
    for (int i = NuevoBucket.nRegistros; i < PAGE_SIZE; i++) {
        for (int j = 0; j < NCOLS; j++) {
            Page.write(emptyRecord.data[j], 255);
        }
    }
    Page.close();    
    cout << "acabo1" << endl;
}

PageRecord BPlusTree::ReadBucket(int posBucket){
    ifstream Page(filename); Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "No puede leer el bucket\n";
        exit(0);
    }
    PageRecord buffer;
    Page.seekg(posBucket);
    Page.read((char *)&buffer.next,  sizeof(int));
    Page.read((char *)&buffer.prev,  sizeof(int));
    Page.read((char *)&buffer.nRegistros,  sizeof(int));
    for (int i = 0; i < PAGE_SIZE; i++) {
        for (int j = 0; j < NCOLS; j++) {
            Page.read((char*) &buffer.registros[i].data[j], 255); // Escribir cada string de 255 caracteres
        }
    }
    Page.close();
    return buffer;
};

Record BPlusTree::ReadRecord(int posRecord){
    ifstream Page(filename); Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "Lee mal el registro\n";
        exit(0);
    }
    Record registro;
    Page.seekg(posRecord);
    Page.read(reinterpret_cast<char*>(&registro), 255*NCOLS);
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
    Page.write(reinterpret_cast<char *>(&NuevoRecord), 255*NCOLS);
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

void BPlusTree:: ReadCSV(string filename_, int nelements){
    ifstream file(filename_);
    string line;

    if (!file.is_open()) {
        cerr << "No se pudo abrir el archivo csv INICIO\n";
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
        Record nuevo = Record(nelements, l);
        cout << line << endl;
        Insert(nuevo);

    }
    file.close();
};

void BPlusTree::SplitChild(int posIndex, Index node, int indexHijo) {
    Index tempChild = ReadIndex(node.posHijos[indexHijo]);
    Index tempHalf;
    tempHalf.isLeaf = tempChild.isLeaf;
    int mid = ORDER / 2;
    for (int i = mid; i < tempChild.nRegistros; i++) {
        tempHalf.posBuckets[i - mid] = tempChild.posBuckets[i];
        tempChild.posBuckets[i] = -1;
    }

    if (!tempChild.isLeaf) {
        for (int i = mid; i < tempChild.nRegistros + 1; i++) {
            tempHalf.posHijos[i - mid] = tempChild.posHijos[i];
            tempChild.posHijos[i] = -1;
        }
    }
    node.posBuckets[indexHijo] = tempChild.posBuckets[mid];
    int i = node.nRegistros;
    while(i > indexHijo){
        node.posHijos[i] = node.posHijos[i - 1];
        i--;
    }
    int posTempHalf = freeList[freeList.size() - 1];
    node.posBuckets[indexHijo] = tempHalf.posBuckets[0]; node.posHijos[indexHijo + 1] = posTempHalf;

    
    WriteIndex(posIndex, node);
    WriteIndex(node.posHijos[indexHijo], tempChild);
    WriteIndex(posTempHalf, tempHalf);
    AjustarFreeList();
};

void BPlusTree::InsertNonFullNode(int posNode, Index node, Record NuevoRegistro) {
    int i = node.nRegistros - 1;
    string value = string(NuevoRegistro.data[KEY_INDEX]);
    
    if (node.isLeaf) {
        Record *RecordKey = ReadListRecord(node.nRegistros, node.posBuckets);
        cout << node.nRegistros << endl;
        
        int IndexPosKeyRecord = 0;
        while (i >= 0 && value < string(RecordKey[i].data[KEY_INDEX])) {
            IndexPosKeyRecord = i;
            i--;
        }
        i++; ; 
        int IndexPosBucket = i; int PosKey = node.posHijos[i];
        PageRecord Bucket = ReadBucket(PosKey);
        
        Record *NewRecords = new Record[Bucket.nRegistros + 1];
        bool Inserted = false; int PosNewRecord;
        for(int j = 0; j < Bucket.nRegistros + 1;){
            if(Bucket.nRegistros == 0 || value < string(Bucket.registros[j].data[KEY_INDEX])){
                NewRecords[j] = NuevoRegistro;
                PosNewRecord = j*255;
                Inserted = true;
            }
            else {
                if(Inserted){
                    NewRecords[j] = Bucket.registros[j - 1];
                }else {
                    NewRecords[j] = Bucket.registros[j];
                }
            }
            j++;
        }


        cout << endl; NewRecords[0].showData(); cout << endl;
        
        if(Bucket.nRegistros == PAGE_SIZE){ // dividir el bucket
            
            PageRecord NewBucket;
            int mid = (PAGE_SIZE + 1)/2;

            for(int i = mid; i < PAGE_SIZE + 1; i++){
                NewBucket.registros[i - mid] = NewRecords[i];
            }
            for(int j = 0; j < mid; j++){
                Bucket.registros[j] = NewRecords[i];
            }
            int PosNewBucket = freeList[freeList.size() - 1];
            
            Bucket.nRegistros = mid; NewBucket.nRegistros = PAGE_SIZE - Bucket.nRegistros;
            NewBucket.next = Bucket.next; Bucket.next = PosNewBucket;
            NewBucket.prev = PosKey;

            int PosNewKey = 12 + PosNewBucket;
            i = node.nRegistros - 1;
            while(i > IndexPosBucket){
                node.posBuckets[i + 1] = node.posBuckets[i];
                i--;
            }
            for(int j = ORDER + 1; j > IndexPosBucket; j--){
                node.posHijos[j] = node.posHijos[j - 1];
            }
            node.posBuckets[IndexPosKeyRecord + 1] = PosNewKey;
            node.posHijos[IndexPosBucket + 1] = PosNewBucket;

            WriteIndex(posNode, node);
            WriteBucket(PosKey, Bucket);
            WriteBucket(PosNewKey, NewBucket);
            AjustarFreeList();
        }
        else{
            int j = Bucket.nRegistros;
            while(j > 0){
                node.posBuckets[j] =  node.posBuckets[j - 1];
                j--;
            }
            node.posBuckets[IndexPosKeyRecord + 1] = PosKey + 12 + PosNewRecord;

        }

        Bucket.nRegistros += 1;

        for(int j = 0; j < Bucket.nRegistros ; j++){
            Bucket.registros[j] = NewRecords[j];
        }
        WriteIndex(posNode, node);
        WriteBucket(PosKey, Bucket);
        cout << "PosKey: " << PosKey << endl;
        cout << "Bucket escrito: " << Bucket.nRegistros << endl;
        cout << "Registro prueba: "; Bucket.registros[0].showData(); cout << endl;
        PageRecord otherBucket = ReadBucket(0); cout << "otherBucket: " << otherBucket.nRegistros << endl;

    
    } else {
        Record *keys = ReadListRecord(node.nRegistros, node.posBuckets);
        while (i >= 0 && value < string(keys[i].data[KEY_INDEX])) {
            i--;
        }
        i++;
        int posHijoIndex = node.posHijos[i];
        Index hijoIndex = ReadIndex(posHijoIndex);
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
    cout << "nada mas" << endl;
};


void BPlusTree::ReadValues(){
    Index node = ReadIndex(pos_root);
    string value; 
    int i;
    while(!node.isLeaf){
        node = ReadIndex(node.posHijos[0]);
    }
    cout << "llega a las hojas" << endl;
    PageRecord Buffer = ReadBucket(node.posHijos[0]);
    vector<Record> registros;
    cout << node.posHijos[0] << endl;
    cout << Buffer.nRegistros << endl;
    
    // ifstream Page; Page.open(filename, ios::binary);
    // Page.seekg(node.posHijos[0]); Page.read((char *)&Buffer, 12 + NCOLS*255);
    // cout << node.posHijos[0] << endl;
    // Buffer.registros[0].showData();

    for(int i = 0; i < Buffer.nRegistros; i++){
        registros.push_back(Buffer.registros[i]);
    }
    cout << "avanza entre hojas" << endl;
    cout << Buffer.next << endl;
    while(Buffer.next != -1){
        cout << "entra" << endl;
        // Page.seekg(Buffer.next, ios::beg);
        // Page.read((char *)&Buffer, 12 + NCOLS*255);
        Buffer = ReadBucket(Buffer.next);
        for(int i = 0; i < Buffer.nRegistros; i++){
            registros.push_back(Buffer.registros[i]);
        }
    };
    cout << "registros.size(): " << registros.size() << endl;
    for(int i = 0; i < registros.size(); i ++){
        registros[i].showData();
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