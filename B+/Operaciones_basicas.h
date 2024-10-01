#include "B+Tree.h"

void BPlusTree::WriteRoot(int posRoot){
    fstream Page; Page.open(Indexfilename, ios::in | ios::out | ios::binary);
    if(!Page.is_open()){
        cerr << "Fallo abriendo archivo en la lectura del index " << posRoot;
        exit(0);
    }
    // NuevoIndex.posHijos[0] = 0; NuevoIndex.posHijos[1] = 12 + NCOLS*255;
    Index NuevoIndex;
    Page.seekp(posRoot);
    Page.write(reinterpret_cast<char *>(&NuevoIndex), sizeof(Index));
    Page.close();
    int PosKey = 0;
    for(int i = 0; i < 2; i++){
        PageRecord NewBucket;
        if(PosKey != 0)NewBucket.prev = 0;
        if(PosKey == 0)NewBucket.next = PosKey + 12 + NCOLS*255;
        NuevoIndex.posHijos[i] = PosKey;
        NuevoIndex.posBuckets[i] = PosKey + 12;
        WriteBucket(PosKey, NewBucket);
        PosKey += 12 + NCOLS*255;
    }
    // WriteBucket(0, PageRecord()); WriteBucket(12 + NCOLS*255, PageRecord()); 
    // PageRecord BucketPrev = ReadBucket(0);
    // PageRecord BucketNex = ReadBucket(PosKey);

    IndexFreeList = vector({PosKey});
};

void BPlusTree::AjustarFreeList(){
    if(freeList.size() != 1){
        freeList.pop_back();
        return;
    }
    ifstream IndexPage; IndexPage.open(filename, ios::binary | ios::ate);
    if (!IndexPage.is_open()) {
        cerr << "Error  ajustando la free list" << endl;
        exit(0);
    }
    streampos LastPos = IndexPage.tellg();
    IndexPage.close();
    freeList[0] = LastPos;
}

void BPlusTree::AjustarIndexFreeList(){
    if(IndexFreeList.size() != 1){
        IndexFreeList.pop_back();
        return;
    }
    ifstream IndexPage; IndexPage.open(Indexfilename, ios::binary | ios::ate);
    if (!IndexPage.is_open()) {
        cerr << "Error  ajustando la free list" << endl;
        exit(0);
    }
    streampos LastPos = IndexPage.tellg();
    IndexPage.close();
    IndexFreeList[0] = LastPos;
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
    
    // cout << "NuevoBucket.next: " << NuevoBucket.next << endl;
    // cout << "NuevoBucket.prev: " << NuevoBucket.prev << endl;
    // cout << "NuevoBucket.nRegistros: " << NuevoBucket.nRegistros << endl;

    
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
    // cout << "posBucket: " << posBucket << endl;
    // cout << "Bucket.prev: " << NuevoBucket.prev << endl;
    // cout << "Bucket.next: " << NuevoBucket.next << endl;
    // cout << "NewBucket.prev: " << NewBucket.prev << endl;
    // cout << "NewBucket.next: " << NewBucket.next << endl;
    Page.close();    
    // cout << "acabo1" << endl;
}

PageRecord BPlusTree::ReadBucket(int posBucket){
    ifstream Page(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "No puede leer el bucket\n";
        exit(0);
    }
    PageRecord buffer;
    Page.seekg(posBucket);
    Page.read((char *)&buffer.next,  sizeof(int));
    Page.read((char *)&buffer.prev,  sizeof(int));
    Page.read((char *)&buffer.nRegistros,  sizeof(int));
    // cout << "buffer.nRegistros: " <<  buffer.nRegistros << endl;
    Page.seekg(posBucket + 12, ios::beg);
    // cout << "Pos: " << posBucket + 12 << endl;
    for (int i = 0; i < PAGE_SIZE; i++) {
        // cout << "i: " << i << endl;
        for (int j = 0; j < NCOLS; j++) {
            char buff[255] = {0};
            Page.read((char*) &buff, 255); // Escribir cada string de 255 caracteres
            strncpy(buffer.registros[i].data[j], buff, 254);
            // cout << "j: " << j << "---- valor del dato en el buffer: " <<  string(buffer.registros[i].data[j]) << endl;
        }
    }
    Page.close();
    // cout << "acabo de leer un bucket" << endl;
    return buffer;
};

Record BPlusTree::ReadRecord(int posRecord){
    ifstream Page; Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "Lee mal el registro\n";
        exit(0);
    }
    Record registro;
    Page.seekg(posRecord);
    // cout << "Leyendo un registro, posRecord: " << posRecord << endl;
    char buff[255] = {0};
    for(int i = 0; i < NCOLS; i++){
        Page.read(buff, 255);
        // cout << "buff: " << buff << endl;
        strncpy(registro.data[i], buff, 254);
        registro.data[i][254] = '\0';
    }
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
    for(int i = 0; i < NCOLS;i++){
        Page.write(NuevoRecord.data[i], 255);
    } 
    // Page.write((char *)&NuevoRecord, 255*NCOLS);
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