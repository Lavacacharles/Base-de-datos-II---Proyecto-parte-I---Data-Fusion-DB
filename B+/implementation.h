// #include "B+Tree.h"
#include "Operaciones_basicas.h"
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
    Index HijoDerecho;
    temp.isLeaf = false;
    temp.nRegistros += 1;
    // node.isLeaf = true;
    // node = temp;
    
    int medio = (ORDER + 1)/2;
    temp.posBuckets[0] = node.posBuckets[medio];
    for(int i = 0; i < node.nRegistros; i++){
        cout << "ReadRecord(node.posBuckets[i]).data[KEY_INDEX]: " << string(ReadRecord(node.posBuckets[i]).data[KEY_INDEX]) << endl;
    }
    node.posBuckets[medio] = -1;

    
    for(int i = medio + 1; i < ORDER;i++){
        HijoDerecho.posBuckets[i - (medio + 1)] = node.posBuckets[i];
        node.posBuckets[i] = -1;
    }

    for(int i = medio + 1; i < ORDER + 1;i++){
        node.posHijos[i - (medio + 1)] = node.posHijos[i];
        node.posHijos[i] = -1;
    }
    node.nRegistros = medio;
    HijoDerecho.nRegistros = ORDER - (medio + 1); 
    HijoDerecho.isLeaf = node.isLeaf;

    int posHijoDerecho = IndexFreeList[IndexFreeList.size() - 1];
    WriteIndex(posHijoDerecho, HijoDerecho);
    AjustarIndexFreeList();

    int posTemp = IndexFreeList[IndexFreeList.size() - 1];
    temp.posHijos[0] = posTemp;
    temp.posHijos[1] = posHijoDerecho;
    WriteIndex(pos_root, temp);

    WriteIndex(posTemp, node);
    AjustarIndexFreeList();
    
    // SplitChild(pos_root, temp, 0);
    cout << "pos_root: " << pos_root << endl;
    return {pos_root, temp};
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

        cout << "Estos son los hijos" << endl;
        for(int j = 0; j < node.nRegistros + 1; j++){
            cout << node.posHijos[j] << " ";
        }
        cout << endl;
        cout << "Estas son las llaves" << endl;
        for(int j = 0; j < node.nRegistros; j++){
            cout << node.posBuckets[j] << " ";
        }

        cout << endl;
        while (i >= 0 && value < LlaveEnElIndex) {
            IndexPosKeyRecord = i;
            i--;
            LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);
        }
        i++;
        int IndexPosBucket = i; 
        int PosKey = node.posBuckets[IndexPosKeyRecord];
        int PosBucket = node.posHijos[IndexPosBucket];
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
        cout << "Ahora hay node.nRegistros: "<< node.nRegistros << endl;
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
        for (int i = 0; i < node.nRegistros; i++){
            cout << "keys[i]: " << string(keys[i].data[KEY_INDEX]) << endl;
        }
        cout << "KeyCompare: "  << KeyCompare << endl;
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
    cout << "Insert: "; NuevoRegistro.showData(); cout << endl;
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
            // cout << "Bucket.registros["<< i - 1<< "]: ";Bucket.registros[i - 1].showData(); cout << endl;
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
            // Raiz.posBuckets[0] = 12;

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
            Raiz.posBuckets[0] = IndicePaginaNuevoBucket + 12;
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
            // cout << "Bucket: " << Bucket.nRegistros << endl;
            WriteBucket(pos_root,Bucket);
        }
        // cout << "Bucket.nRegistros: " << Bucket.nRegistros << endl;
        return;
    }
    

    Index node = ReadIndex(pos_root);
    // cout << "Insertando: " << endl;
    for(int i = 0; i < node.nRegistros ;i++){
        cout << "node.posBuckets[" << i << "]: " << node.posBuckets[i] << "| ";
        string vals = string(ReadRecord(node.posBuckets[i]).data[KEY_INDEX]);
        cout << "ReadRecord(node.posBuckets["<< i << "]).data[KEY_INDEX]: " << vals << endl;
    }
    if (ORDER == node.nRegistros) {
        cout << "divide el root---------------------------------" << endl;
        Index temp; int posTemp;
        pair<int, Index> InfoTemp = SplitRoot(pos_root, node);
        posTemp = InfoTemp.first; temp = InfoTemp.second;
        cout << "posTemp: " << posTemp << endl;
        cout << "temp.nRegistros: " << temp.nRegistros << endl;
        cout << "temp.posBuckets[0]: " << temp.posBuckets[0] << endl;
        cout << "ReadRecord(temp.posBuckets[0]): " << string(ReadRecord(temp.posBuckets[0]).data[KEY_INDEX]) << endl;
        InsertNonFullNode(posTemp, temp, NuevoRegistro);
    } else {
        // cout << "por aca" <<endl;
        InsertNonFullNode(pos_root, node, NuevoRegistro);
    }
    cout << "nada mas" << endl;
};


void BPlusTree::ReadValues(){
    PageRecord Buffer;
    if(!IsPage){
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
        cout << "FisrtBucket: " << FisrtBucket << endl;
        Buffer = ReadBucket(FisrtBucket);
    }
    else {
        Buffer = ReadBucket(pos_root);
    }
    cout << "avanza entre hojas" << endl;
    vector<Record> registros;
    
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