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
    ifstream Page(filename); Page.open(filename, ios::binary);

    if (!Page.is_open()) {
        cerr << "Lee mal el registro\n";
        exit(0);
    }
    Record registro;
    Page.seekg(posRecord);
    char buff[255] = {0};
    for(int i = 0; i < NCOLS; i++){
        Page.read((char*)&buff, 255);
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
        cout << "line: "<< line << endl;
        Insert(nuevo);

    }
    file.close();
};
pair<int,Index> BPlusTree::SplitRoot(int posNode, Index node) {
    Index temp;
    temp.isLeaf = false;
    temp.nRegistros += 1;
    // node.isLeaf = true;
    // node = temp;
    int posTemp = IndexFreeList[IndexFreeList.size() - 1];

    int medio = (ORDER + 1)/2;
    for(int i = medio; i < node.nRegistros;i++){
        ;
    }

    temp.posHijos[0] = posTemp;
    // pos_root = posTemp;




    WriteIndex(posTemp, node);
    WriteIndex(pos_root, temp);
    AjustarIndexFreeList();
    
    // SplitChild(pos_root, temp, 0);
    cout << "pos_root: " << pos_root << endl;
    return {posTemp, temp};
}
void BPlusTree::SplitChild(int posIndex, Index node, int indexHijo) {
    Index tempChild = ReadIndex(node.posHijos[indexHijo]);
    Index tempHalf;
    tempHalf.isLeaf = tempChild.isLeaf;
    int mid = (ORDER + 1) / 2;
    int LlaveHijoDerecho = -1;

    // int LlaveDeLaParticion = tempChild.posHijos[mid];
    tempChild.posHijos[mid] = -1;
    for (int i = mid + 1; i < tempChild.nRegistros; i++) {
        tempHalf.posBuckets[i - (mid + 1)] = tempChild.posBuckets[i];
        tempChild.posBuckets[i] = -1; //borrar llaves del hijo izquierdo
    }
    for (int i = mid + 1; i < tempChild.nRegistros + 1; i++) {
        tempHalf.posHijos[i - (mid + 1)] = tempChild.posHijos[i];
        tempChild.posHijos[i] = -1;
    }
    //end
    int posTempHalf = IndexFreeList[IndexFreeList.size() - 1];
    // node.posBuckets[indexHijo] = tempHalf.posBuckets[0]; 
    for(int i = node.nRegistros; i > indexHijo; i--){
        node.posBuckets[i] = node.posBuckets[i - 1];
    }
    node.posBuckets[indexHijo + 1] = tempHalf.posBuckets[0];
    for(int i = node.nRegistros + 1; i > indexHijo; i--){
        node.posHijos[i] = node.posHijos[i - 1];
    }
    node.posHijos[indexHijo + 1] = tempHalf.posHijos[0];
    tempChild.nRegistros = mid; tempHalf.nRegistros = ORDER - mid - 1;
    node.nRegistros += 1;
    WriteIndex(posIndex, node);
    WriteIndex(node.posHijos[indexHijo], tempChild);
    WriteIndex(posTempHalf, tempHalf);
    AjustarIndexFreeList();
};

void BPlusTree::InsertNonFullNode(int posNode, Index node, Record NuevoRegistro) {
    int i = node.nRegistros - 1;
    string value = string(NuevoRegistro.data[KEY_INDEX]);
    // cout << "node.nRegistros: " << node.nRegistros << endl;
    
    if (node.isLeaf) {
        cout << "inserta en hoja "<< endl;
        // cout << "Chapo la lista de llaves del index" << posNode << endl;
        Record *RecordKey = ReadListRecord(node.nRegistros, node.posBuckets);
        int IndexPosKeyRecord = 0;

        string LlaveEnElIndex;
        LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);
        // if(node.nRegistros != 0){
        // }
        // for(int j = 0; j < node.nRegistros + 1; j++){
        //     cout << node.posHijos[j] << " ";
        // }
        // cout << endl;
        while (i >= 0 && value < LlaveEnElIndex) {
            IndexPosKeyRecord = i;
            i--;
            LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);
        }
        i++;
        int IndexPosBucket = i; 
        int PosKey = node.posBuckets[IndexPosKeyRecord];
        int PosBucket = node.posHijos[IndexPosBucket];
        // cout << "Bucket index insertar: " << IndexPosBucket << endl;
        // cout << "Key referencia insertar: " << IndexPosKeyRecord << endl;
        // if(node.nRegistros != 0){
        //     // cout << "asignando PosKey: " << node.posBuckets[i] << endl;
        //     if(node.posHijos[i] == -1){
        //         int PosNewBucket = freeList[freeList.size() - 1];
        //         // cout << "habilitando rama: " << PosNewBucket << endl;
        //         node.posHijos[i] = PosNewBucket;
        //         WriteBucket(PosNewBucket, PageRecord());
        //         AjustarFreeList();
        //     }
        //     PosKey = node.posHijos[i];
        // }else{
        //     PosKey = 0;
        // }
        // cout << "node.nRegistros: " << node.nRegistros << endl;
        // cout << "PosKey: " << PosKey << endl;
        PageRecord Bucket;
        if(node.nRegistros != 0){
            // cout << "Lectura para node.nRegistros: " << node.nRegistros << endl;
            Bucket = ReadBucket(PosBucket);
            // cout << "Bucket = ReadBucket(PosKey): " << Bucket.nRegistros << endl;
        }
        
        // cout << "se prepara para insertar en orden" << endl;
        Record NewRecords[PAGE_SIZE + 1];
        // cout << "se prepara para insertar en orden" << endl;
        int PosNewRecord;
        // cout << "valor a insertar: " << value << endl;
        string KeyCompare;

        bool ConditionInsert = false; bool Inserted = false;
        // cout << "Revisando valor en la lista antes de los nuevos registros" << endl;
        // for(int k = 0; k < Bucket.nRegistros;k++){
        //     cout << k<< "\n"; Bucket.registros[k].showData(); cout << endl;
        // }

        for(int j = Bucket.nRegistros; (j > 0) || (j>=0 & Inserted); j--){
            if(Inserted) {
                // cout << "Bucket.registros[j].showData(): "<< j << "----";Bucket.registros[j].showData(); cout << endl;
                NewRecords[j] = Bucket.registros[j];
                continue;
            }
            KeyCompare = string(Bucket.registros[j - 1].data[KEY_INDEX]);
            if(DATATYPE == 1){
                ConditionInsert = (stoi(value) >= stoi(KeyCompare));
            }
            else{
                ConditionInsert = (value >= KeyCompare);
            }

            if(ConditionInsert){
                Inserted = true;
                // cout << "NewRecords[j] = NuevoRegistro: "<< j << "----"; NuevoRegistro.showData(); cout << endl;
                NewRecords[j] = NuevoRegistro;
            }else {
                // cout << "Bucket.registros[j-1].showData(): "<< j << "----";Bucket.registros[j-1].showData(); cout << endl;
                NewRecords[j] = Bucket.registros[j - 1];
            }
        }
        // cout << "Inserted: " << Inserted << endl;  
        if(!Inserted){
            NewRecords[0] = NuevoRegistro;
        }
        // cout << "Revisando valor en la lista de registros nuevos" << endl;
        // for(int k = 0; k < Bucket.nRegistros + 1;k++){
        //     cout << k<< "\n"; NewRecords[k].showData(); cout << endl;
        // }

        // cout << "lleno el nuevo valor en una pagina" << endl;
        
        if(Bucket.nRegistros == PAGE_SIZE){ // dividir el bucket
            
            PageRecord NewBucket;
            int mid = (PAGE_SIZE + 1)/2;

            for(int i = mid; i < PAGE_SIZE + 1; i++){
                NewBucket.registros[i - mid] = NewRecords[i];
                if(i < PAGE_SIZE) Bucket.registros[i] = Record();
            }
            for(int j = 0; j < mid; j++){
                Bucket.registros[j] = NewRecords[j];
            }
            int PosNewBucket = freeList[freeList.size() - 1];
            
            Bucket.nRegistros = mid; 
            NewBucket.nRegistros = PAGE_SIZE - Bucket.nRegistros;

            if(Bucket.next != -1) NewBucket.next = Bucket.next;
            Bucket.next = PosNewBucket;
            NewBucket.prev = PosBucket;

            int PosNewKey = PosNewBucket + 12;

            for(i = node.nRegistros; i > IndexPosKeyRecord; i--){
                node.posBuckets[i] = node.posBuckets[i - 1];
                node.posHijos[i + 1] = node.posHijos[i];
            }
            node.posBuckets[IndexPosKeyRecord + 1] = PosNewKey;
            node.posHijos[IndexPosBucket + 1] = PosNewBucket;
            // WriteIndex(posNode, node);
            // cout  << "aca?" << endl;
            // WriteBucket(PosKey, Bucket);
            node.nRegistros += 1;
            // cout << "Bucket.prev: " << Bucket.prev << endl;
            // cout << "Bucket.next: " << Bucket.next << endl;
            // cout << "NewBucket.prev: " << NewBucket.prev << endl;
            // cout << "NewBucket.next: " << NewBucket.next << endl;
            // cout << "PosNewKey: " << PosNewKey << endl;
            WriteBucket(PosNewBucket, NewBucket);
            AjustarFreeList();
        }
        else{
            // cout << "ajustando keys del indice" << endl;
            // cout << "Bucket.nRegistros " << Bucket.nRegistros << endl;
            // cout << "IndexPosKeyRecord " << IndexPosKeyRecord << endl;
            // int j = Bucket.nRegistros;
            // if(node.nRegistros == 0){
            //     node.posBuckets[0] = PosKey + 12;
            //     node.posHijos[1] = PosKey;
            //     node.nRegistros += 1;
            // }
            // node.posBuckets[IndexPosKeyRecord] = PosKey + 12 + j*PosNewRecord;
            Bucket.nRegistros += 1;

            for(int j = 0; j < Bucket.nRegistros ; j++){
                Bucket.registros[j] = NewRecords[j];
            }
        }

        // cout << "aca?"<< endl;
        WriteIndex(posNode, node);
        // cout << "se escribira el bucket, miramos los registros: "<< Bucket.nRegistros << endl;
        WriteBucket(PosBucket, Bucket);

        // cout << "PosKey: " << PosKey << endl;
        // cout << "node.nRegistros: " << node.nRegistros << endl; 
        // cout << "Bucket escrito: " << Bucket.nRegistros << endl;
        // cout << "Registro prueba: "; Bucket.registros[0].showData(); cout << endl;
        // PageRecord otherBucket = ReadBucket(0); cout << "otherBucket: " << otherBucket.nRegistros << endl;

    
    } else {
        cout << "inserta indice "<< endl;
        Record *keys = ReadListRecord(node.nRegistros, node.posBuckets);
        string KeyCompare = string(keys[i].data[KEY_INDEX]);
        cout << "node.nRegistros: " <<node.nRegistros << endl;
        for(int p = 0; p < node.nRegistros; p++){
            cout << "node.posBuckets[i]: " << node.posBuckets[i] << " ";
        }
        cout << endl;
        cout << "keys[i].data[KEY_INDEX]: " << keys[i].data[KEY_INDEX] << endl;
        bool ConditionInsert = false;
        if(DATATYPE == 1){
            ConditionInsert = (stoi(value) < stoi(KeyCompare));
        }
        else{
            ConditionInsert = (value < KeyCompare);
        }
        while (i >= 0 && ConditionInsert) {
            i--;
            KeyCompare = string(keys[i].data[KEY_INDEX]);
            if(DATATYPE == 1){
                ConditionInsert = (stoi(value) < stoi(KeyCompare));
            }
            else{
                ConditionInsert = (value < KeyCompare);
            }
        }
        i++;
        int posHijoIndex = node.posHijos[i];
        Index hijoIndex = ReadIndex(posHijoIndex);
        if (hijoIndex.nRegistros == ORDER) {
            SplitChild(posNode, node, i);

            KeyCompare = string(keys[i].data[KEY_INDEX]);
            ConditionInsert = false;
            if(DATATYPE == 1){
                ConditionInsert = (stoi(value) > stoi(KeyCompare));
            }
            else{
                ConditionInsert = (value > KeyCompare);
            }
            if (ConditionInsert) {
                i++;
            }
        }
        InsertNonFullNode(posHijoIndex, hijoIndex, NuevoRegistro);
    }
};

void BPlusTree::Insert(Record NuevoRegistro) {
    if(IsPage){
        PageRecord Bucket = ReadBucket(pos_root); 
        Record NuevosRegistros[PAGE_SIZE + 1];
        bool Inserted = false; bool SeDebeInsertar = false;
        string LlaveRegistroNuevo = string(NuevoRegistro.data[KEY_INDEX]);
        string LlaveRegistroViejo;
        
        for(int i = Bucket.nRegistros; (i > 0) || (i>=0 & Inserted); i--){
            if(Inserted) {
                // cout << "Bucket.registros[j].showData(): "<< j << "----";Bucket.registros[j].showData(); cout << endl;
                NuevosRegistros[i] = Bucket.registros[i];
                continue;
            }
            LlaveRegistroViejo = string(Bucket.registros[i - 1].data[KEY_INDEX]);
            if(DATATYPE == 1){
                SeDebeInsertar = (stoi(LlaveRegistroNuevo) >= stoi(LlaveRegistroViejo));
            }
            else{
                SeDebeInsertar = (LlaveRegistroNuevo >= LlaveRegistroViejo);
            }

            if(SeDebeInsertar){
                Inserted = true;
                // cout << "NewRecords[j] = NuevoRegistro: "<< j << "----"; NuevoRegistro.showData(); cout << endl;
                NuevosRegistros[i] = NuevoRegistro;
            }else {
                // cout << "Bucket.registros[j-1].showData(): "<< j << "----";Bucket.registros[j-1].showData(); cout << endl;
                NuevosRegistros[i] = Bucket.registros[i - 1];
            }
        }
        if(!Inserted){
            NuevosRegistros[0] = NuevoRegistro;
        }

        if(Bucket.nRegistros == PAGE_SIZE){
            Index Raiz;
            Raiz.isLeaf = true;
            Raiz.nRegistros = 1;
            Raiz.posHijos[0] = 0;
            Raiz.posHijos[1] = 12 + NCOLS*255*PAGE_SIZE;
            Raiz.posBuckets[0] = 12;

            PageRecord NuevoBucket;
            int IndiceMedio = (PAGE_SIZE + 1)/2;
            for(int i = IndiceMedio; i < PAGE_SIZE + 1; i++){
                NuevoBucket.registros[i - IndiceMedio] = NuevosRegistros[i];
                if(i < PAGE_SIZE) Bucket.registros[i] = Record();
            }
            for(int i = 0; i < IndiceMedio; i ++){
                Bucket.registros[i] = NuevosRegistros[i];
            }
            int IndicePaginaNuevoBucket = freeList[freeList.size() - 1];
            Bucket.nRegistros = IndiceMedio; NuevoBucket.nRegistros = PAGE_SIZE - IndiceMedio;
            Bucket.next = IndicePaginaNuevoBucket; NuevoBucket.prev = 0;
            
            // cout << "IndicePaginaNuevoBucket: "<< IndicePaginaNuevoBucket << endl;
            WriteBucket(pos_root, Bucket);
            WriteBucket(IndicePaginaNuevoBucket, NuevoBucket);
            WriteIndex(pos_root, Raiz);
            AjustarIndexFreeList();
            AjustarFreeList();
            IsPage = false;
        }else {
            for(int i = 0; i < Bucket.nRegistros + 1; i ++){
                Bucket.registros[i] = NuevosRegistros[i];
            }
            Bucket.nRegistros += 1;
            WriteBucket(pos_root,Bucket);
        }
        // cout << "Bucket.nRegistros: " << Bucket.nRegistros << endl;
        return;
    }


    Index node = ReadIndex(pos_root);
    if (ORDER == node.nRegistros) {
        cout << "divide el root---------------------------------" << endl;
        Index temp; int posTemp;
        pair<int, Index> InfoTemp = SplitRoot(pos_root, node);
        posTemp = InfoTemp.first; temp = InfoTemp.second;
        cout << "posTemp: " << posTemp << endl;
        cout << "temp.nRegistros: " << temp.nRegistros << endl;
        InsertNonFullNode(posTemp, temp, NuevoRegistro);
    } else {
        // cout << "por aca" <<endl;
        InsertNonFullNode(pos_root, node, NuevoRegistro);
    }
    cout << "nada mas" << endl;
};


void BPlusTree::ReadValues(){
    Index node = ReadIndex(pos_root);
    string value; 
    // int i;
    while(!node.isLeaf){
        for(int i = 0; i < node.nRegistros + 1; i++){
            if(node.posHijos[i] != -1){
                node = ReadIndex(node.posHijos[i]);
                break;
            }
        }

    }
    cout << "llega a las hojas" << endl;
    int FisrtBucket = -1;
    for(int i = 0; i < node.nRegistros + 1;i++){
        if(node.posHijos[i] != -1){
            FisrtBucket = node.posHijos[i];
            break;
        }
    }
    if(FisrtBucket == -1){
        cout << "No hay ni pincho " << endl;
        return ;
    }
    cout << "avanza entre hojas" << endl;
    PageRecord Buffer = ReadBucket(FisrtBucket);
    vector<Record> registros;
    
    cout << "FisrtBucket: " << FisrtBucket << endl;
    cout <<"Buffer.nRegistros: " << Buffer.nRegistros << endl;
    cout <<"Buffer.next: " << Buffer.next << endl;
    // cout << "Buffer.next: " << Buffer.next << endl;
    // ifstream Page; Page.open(filename, ios::binary);

    for(int i = 0; i < Buffer.nRegistros; i++){
        registros.push_back(Buffer.registros[i]);
    }
    int k = 0;
    while(Buffer.next != -1){
        cout << "entra-------------------------------" << endl;
        Buffer = ReadBucket(Buffer.next);
        // Page.seekg(Buffer.next, ios::beg);
        // Page.read((char *)&Buffer, 12 + NCOLS*255);
        cout << "Buffer.nRegistros: " << Buffer.nRegistros << endl;
        cout << "Buffer.next: " << Buffer.next << endl;
        for(int i = 0; i < Buffer.nRegistros; i++){
            registros.push_back(Buffer.registros[i]);
        }
        // k++; if(k > 10) return;
        cout << "siguiente-------------------------------" << endl;
    };
    cout << "registros.size(): " << registros.size() << endl;
    for(int i = 0; i < registros.size(); i ++){
        registros[i].showData(); cout << " ";
        // if(i == 30) break;
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