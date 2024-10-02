// #include "B+Tree.h"
#include "Operaciones_basicas.h"
#include <unordered_set>
pair<int,Index> BPlusTree::SplitRoot(int posNode, Index node) {
    Index temp;
    Index HijoDerecho;
    temp.isLeaf = false;
    temp.nRegistros += 1;
    // node.isLeaf = true;
    // node = temp;
    
    int medio = (ORDER + 1)/2;
    temp.posBuckets[0] = node.posBuckets[medio];
    node.posBuckets[medio] = -1;

    
    for(int i = medio + 1; i < ORDER;i++){
        HijoDerecho.posBuckets[i - (medio + 1)] = node.posBuckets[i];
        node.posBuckets[i] = -1;
    }

    for(int i = medio + 1; i < ORDER + 1;i++){
        // cout << "node.posHijos[" << i << "]: " << node.posHijos[i] << endl;
        HijoDerecho.posHijos[i - (medio + 1)] = node.posHijos[i];
        node.posHijos[i] = -1;
    }
    node.nRegistros = medio;
    HijoDerecho.nRegistros = ORDER - (medio + 1); 
    HijoDerecho.isLeaf = true;

    int posHijoDerecho = IndexFreeList[IndexFreeList.size() - 1];

    WriteIndex(posHijoDerecho, HijoDerecho);
    AjustarIndexFreeList();

    int posTemp = IndexFreeList[IndexFreeList.size() - 1];
    temp.posHijos[0] = posTemp;
    temp.posHijos[1] = posHijoDerecho;
    WriteIndex(pos_root, temp);


    // cout << "Despues asi son los hijos raiz temp.posHijos[j]" << endl;
    // for(int j = 0; j < temp.nRegistros + 1; j++){
    //     Index provIndex = ReadIndex(temp.posHijos[j]);

    //     cout << string(ReadRecord(provIndex.posBuckets[0]).data[KEY_INDEX]) << " ";
    // }
    // cout << endl;

    WriteIndex(posTemp, node);
    AjustarIndexFreeList();
    
    // SplitChild(pos_root, temp, 0);
    // cout << "pos_root: " << pos_root << endl;
    return {pos_root, temp};
}
void BPlusTree::SplitChild(int posIndex, Index node, int indexHijo) {
    Index tempChild = ReadIndex(node.posHijos[indexHijo]);
    Index tempHalf;
    tempHalf.isLeaf = tempChild.isLeaf;
    int mid = (ORDER + 1) / 2;
    int LlaveHijoDerecho = -1;
    // for(int k = 0; k < ORDER; k ++){
    //     cout << "node.posBuckets["<< k<< "]: " << node.posBuckets[k] << ", ";
    // }cout << endl;

    // int LlaveDeLaParticion = tempChild.posHijos[mid];
    for (int i = mid + 1; i < tempChild.nRegistros; i++) {
        tempHalf.posBuckets[i - (mid + 1)] = tempChild.posBuckets[i];
        tempChild.posBuckets[i] = -1; //borrar llaves del hijo izquierdo
    }
    for (int i = mid + 1; i < tempChild.nRegistros + 1; i++) {
        tempHalf.posHijos[i - (mid + 1)] = tempChild.posHijos[i];
        tempChild.posHijos[i] = -1;
    }
    tempChild.posHijos[mid] = -1;
    //end
    int posTempHalf = IndexFreeList[IndexFreeList.size() - 1];
    // node.posBuckets[indexHijo] = tempHalf.posBuckets[0]; 

    for(int i = node.nRegistros; i > indexHijo; i--){
        node.posBuckets[i] = node.posBuckets[i - 1];
    }
    node.posBuckets[indexHijo] = tempHalf.posBuckets[0];
    // cout << "indexHijo: " << indexHijo << endl;

    // for(int k = 0; k < ORDER; k ++){
    //     cout << "node.posBuckets["<< k<< "]: " << node.posBuckets[k] << ", ";
    // }cout << endl;

    for(int i = node.nRegistros + 1; i > indexHijo; i--){
        node.posHijos[i] = node.posHijos[i - 1];
    }
    node.posHijos[indexHijo + 1] = posTempHalf;
    tempChild.nRegistros = mid; tempHalf.nRegistros = ORDER - mid - 1;
    node.nRegistros += 1;

    // for(int k = 0; k < node.nRegistros; k ++){
    //     cout << "node.posBuckets["<< k<< "]: " << node.posBuckets[k] << ", ";
    // }cout << endl;
    // for(int k = 0; k < tempChild.nRegistros; k ++){
    //     cout << "tempChild.posBuckets["<< k<< "]: " << tempChild.posBuckets[k] << ", ";
    // }cout << endl;
    // for(int k = 0; k < tempHalf.nRegistros; k ++){
    //     cout << "tempHalf.posBuckets["<< k<< "]: " << tempHalf.posBuckets[k] << ", ";
    // }cout << endl;
    

    WriteIndex(posIndex, node);
    WriteIndex(node.posHijos[indexHijo], tempChild);
    WriteIndex(posTempHalf, tempHalf);
    AjustarIndexFreeList();
};

void BPlusTree::InsertNonFullNode(int posNode, Index node, Record NuevoRegistro) {
    int i = node.nRegistros - 1;
    string value = string(NuevoRegistro.data[KEY_INDEX]);
    // for(int i = 0; i < node.nRegistros ;i++){
    //     cout << "node.posBuckets[" << i << "]: " << node.posBuckets[i] << "| ";
    //     string vals = string(ReadRecord(node.posBuckets[i]).data[KEY_INDEX]);
    //     cout << "ReadRecord(node.posBuckets["<< i << "]).data[KEY_INDEX]: " << vals << endl;
    // }
    // cout << "node.nRegistros: " << node.nRegistros << endl;
    
    if (node.isLeaf) {
        // cout << "inserta en hoja "<< endl;
        // cout << "Chapo la lista de llaves del index" << posNode << endl;
        // cout << "Estos son los hijos" << endl;
        // for(int j = 0; j < node.nRegistros + 1; j++){
        //     cout << node.posHijos[j] << " ";
        // }
        // cout << endl;
        // cout << "Estas son las llaves" << endl;
        // for(int j = 0; j < node.nRegistros; j++){
        //     cout << node.posBuckets[j] << " ";
        // }
        // cout << endl;
        Record *RecordKey = ReadListRecord(node.nRegistros, node.posBuckets);

        string LlaveEnElIndex;
        LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);

        int IndexPosKeyRecord = i;
        bool EsElNodoCorrecto = false;
        LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);
        if(DATATYPE == 1){
            EsElNodoCorrecto = (stoi(value) < stoi(LlaveEnElIndex));
        }
        else{
            EsElNodoCorrecto = (value < LlaveEnElIndex);
        }
        while (i >= 0 && EsElNodoCorrecto) {
            LlaveEnElIndex = string(RecordKey[i].data[KEY_INDEX]);
            // cout << "LlaveEnElIndex: " << LlaveEnElIndex << "| " << "value: " << value << endl;
            if(DATATYPE == 1){
                EsElNodoCorrecto = (stoi(value) < stoi(LlaveEnElIndex));
            }
            else{
                EsElNodoCorrecto = (value < LlaveEnElIndex);
            }
            IndexPosKeyRecord = i;
            i--;
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
        // cout << "NewRecords[k] Revisando valor en la lista de registros nuevos:" << endl;
        // for(int k = 0; k < Bucket.nRegistros + 1;k++){
        //     NewRecords[k].showData(); cout << " ";
        // }
        // cout << endl;
        if(Bucket.nRegistros == PAGE_SIZE){ // dividir el bucket
            
            PageRecord NewBucket;
            int mid = (PAGE_SIZE + 1)/2;
            // cout << "Nuevos NewBucket.registros[i - mid]: " << endl;
            for(int i = mid; i < PAGE_SIZE + 1; i++){
                NewBucket.registros[i - mid] = NewRecords[i];
                if(i < PAGE_SIZE) Bucket.registros[i] = Record();
                // cout << string(NewBucket.registros[i - mid].data[KEY_INDEX]) << " ";
            }
            // cout << endl;
            for(int j = 0; j < mid; j++){
                Bucket.registros[j] = NewRecords[j];
            }
            int PosNewBucket = freeList[freeList.size() - 1];
            
            Bucket.nRegistros = mid; 
            NewBucket.nRegistros = PAGE_SIZE - mid + 1;
        
            if(Bucket.next != -1) NewBucket.next = Bucket.next;
            Bucket.next = PosNewBucket;
            NewBucket.prev = PosBucket;

            int PosNewKey = PosNewBucket + 12;
            int PosNuevasLlaves[ORDER];
            string Llave;
            Llave = string(NuevoRegistro.data[KEY_INDEX]);
            // cout << "Antes node.posBuckets[i] : " << endl;
            // for(i = 0; i < node.nRegistros; i++){
            //     cout << node.posBuckets[i] << " "; 
            // }
            // cout << endl;
            for(i = node.nRegistros; i > IndexPosKeyRecord; i--){
                node.posBuckets[i] = node.posBuckets[i - 1];
                node.posHijos[i + 1] = node.posHijos[i];
            }
            node.posBuckets[IndexPosKeyRecord + 1] = PosNewKey;
            // cout << "Despues node.posBuckets[i] : " << endl;
            // for(i = 0; i < node.nRegistros + 1; i++){
            //     cout << node.posBuckets[i] << " "; 
            // }
            // cout << endl;
            node.posHijos[IndexPosBucket + 1] = PosNewBucket;
            // cout << "PosNewKey: " << PosNewKey << endl;
            // cout << "node.nRegistros: " << node.nRegistros << endl;
            // cout << "IndexPosKeyRecord: " << IndexPosKeyRecord << endl;
            // cout << "node.posBuckets[IndexPosKeyRecord + 1]: " << node.posBuckets[IndexPosKeyRecord] << endl;
            
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
            Bucket.nRegistros += 1;

            for(int j = 0; j < Bucket.nRegistros ; j++){
                Bucket.registros[j] = NewRecords[j];
            }
        }

        // cout << "aca?"<< endl;
        // cout << "Ahora hay node.nRegistros: "<< node.nRegistros << endl;
        WriteIndex(posNode, node);
        // cout << "se escribira el bucket, miramos los registros: "<< Bucket.nRegistros << endl;
        WriteBucket(PosBucket, Bucket);

        // cout << "PosKey: " << PosKey << endl;
        // cout << "node.nRegistros: " << node.nRegistros << endl; 
        // cout << "Bucket escrito: " << Bucket.nRegistros << endl;
        // cout << "Registro prueba: "; Bucket.registros[0].showData(); cout << endl;
        // PageRecord otherBucket = ReadBucket(0); cout << "otherBucket: " << otherBucket.nRegistros << endl;

    
    } else {
        // cout << "inserta indice "<< endl;
        Record *keys = ReadListRecord(node.nRegistros, node.posBuckets);
        string KeyCompare = string(keys[i].data[KEY_INDEX]);
        bool ConditionInsert = false;
        if(DATATYPE == 1){
            ConditionInsert = (stoi(value) < stoi(KeyCompare));
        }
        else{
            ConditionInsert = (value < KeyCompare);
        }
        
        while (i >= 0 && ConditionInsert) {
            KeyCompare = string(keys[i].data[KEY_INDEX]);
            // cout << "KeyCompare: " << KeyCompare << endl;
            if(DATATYPE == 1){
                ConditionInsert = (stoi(value) < stoi(KeyCompare));
            }
            else{
                ConditionInsert = (value < KeyCompare);
            }
            i--;
        }
        i++;
        int posHijoIndex = node.posHijos[i];
        // cout << "i - posHijoIndex, posHijoIndex: " << posHijoIndex << endl; 
        Index hijoIndex = ReadIndex(posHijoIndex);
        // cout << "hijoIndex.posBuckets[i]).data[KEY_INDEX]"<<endl;
        // for(int i = 0; i < hijoIndex.nRegistros; i++){
        //     cout << string(ReadRecord(hijoIndex.posBuckets[i]).data[KEY_INDEX]) << " ";
        // }
        // cout << endl;
        if (hijoIndex.nRegistros == ORDER) {
            // cout << "splitea: " << endl;
            SplitChild(posNode, node, i);
            node = ReadIndex(posNode);
            keys = ReadListRecord(node.nRegistros, node.posBuckets);
            // cout << "node.nRegistros: " <<node.nRegistros << endl;
            // for(int p = 0; p < node.nRegistros + 1; p++){
            //     cout << "node.posHijos["<< p << "]: " << node.posHijos[p] << " ";
            // }cout << endl;
            // for (int i = 0; i < node.nRegistros; i++){
            //     cout << "keys[i]: " << string(keys[i].data[KEY_INDEX]) << endl;
            // }
            KeyCompare = string(keys[i].data[KEY_INDEX]);
            // cout << "i: " << i << ", " << KeyCompare << endl;
            ConditionInsert = false;
            // cout << "pas aqui: " << i << ", " << KeyCompar   e << endl;
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
    // cout << "Insert: "; NuevoRegistro.showData(); cout << endl;
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
            Bucket.nRegistros = IndiceMedio; NuevoBucket.nRegistros = PAGE_SIZE - IndiceMedio + 1;
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
    // for(int i = 0; i < node.nRegistros ;i++){
    //     cout << "node.posBuckets[" << i << "]: " << node.posBuckets[i] << "| ";
    //     string vals = string(ReadRecord(node.posBuckets[i]).data[KEY_INDEX]);
    //     cout << "ReadRecord(node.posBuckets["<< i << "]).data[KEY_INDEX]: " << vals << endl;
    // }
    // for(int k = 0; k < ORDER; k ++){
    //     cout << "node.posBuckets["<< k<< "]: " << node.posBuckets[k] << ", ";
    // }cout << endl;
    if (ORDER == node.nRegistros) {
        // cout << "divide el root---------------------------------" << endl;
        Index temp; int posTemp;
        pair<int, Index> InfoTemp = SplitRoot(pos_root, node);
        posTemp = InfoTemp.first; temp = InfoTemp.second;
        // cout << "posTemp: " << posTemp << endl;
        // cout << "temp.nRegistros: " << temp.nRegistros << endl;
        // cout << "temp.posBuckets[0]: " << temp.posBuckets[0] << endl;
        // cout << "ReadRecord(temp.posBuckets[0]): " << string(ReadRecord(temp.posBuckets[0]).data[KEY_INDEX]) << endl;
        InsertNonFullNode(posTemp, temp, NuevoRegistro);
    } else {
        // cout << "por aca" <<endl;
        InsertNonFullNode(pos_root, node, NuevoRegistro);
    }
    // cout << "nada mas" << endl;
};
int binarySearch(int arr[], int n, int target) {
    int inicio = 0, fin = n - 1;

    while (inicio <= fin) {
        int medio = inicio + (fin - inicio) / 2; // Evitar desbordamiento

        // Verificar si el valor objetivo está en el medio
        if (arr[medio] == target)
            return medio;

        // Si el valor objetivo es menor, se ajusta el límite superior
        if (arr[medio] > target)
            fin = medio - 1;
        else
            inicio = medio + 1; // Ajustar el límite inferior
    }

    // Si el valor no está presente en el arreglo
    return -1;
}

pair <int, Record> BPlusTree::Search(string BuscarRegistro){
    if(IsPage){
        PageRecord Bucket = ReadBucket(pos_root);
        int inicio = 0, fin = Bucket.nRegistros - 1; 
        bool EsMayor = false; bool EsIgual = false;

        while (inicio <= fin) {
            int medio = inicio + (fin - inicio) / 2;
            if(DATATYPE == 1){
                EsIgual = (stoi(BuscarRegistro) == stoi(Bucket.registros[medio].data[KEY_INDEX]));
            }
            else{
                EsIgual = (BuscarRegistro == Bucket.registros[medio].data[KEY_INDEX]);
            }
            if (EsIgual)
                return {pos_root, Bucket.registros[medio]};
            
            if(DATATYPE == 1){
                EsMayor = (stoi(BuscarRegistro) < stoi(Bucket.registros[medio].data[KEY_INDEX]));
            }
            else{
                EsMayor = (BuscarRegistro < Bucket.registros[medio].data[KEY_INDEX]);
            }
            if (EsMayor)
                fin = medio - 1;
            else
                inicio = medio + 1; // Ajustar el límite inferior
        }
        Record vacio;
        return {-1, vacio};
    }
    Index Raiz = ReadIndex(pos_root);
    bool LlegoAHoja;
        
    int IndexPagina = -1;
    while(!(Raiz.isLeaf && LlegoAHoja)){
        if(Raiz.isLeaf){
            LlegoAHoja = true;
        }
        Record *Bucket = ReadListRecord(Raiz.nRegistros, Raiz.posBuckets);
        string Llave;
        bool AcaBusca = false;
        int index = 0;
        for(int i = 0; i < Raiz.nRegistros; i++){
            Llave = string(Bucket[i].data[KEY_INDEX]);
            if(DATATYPE == 1){
                AcaBusca = (stoi(Llave) > stoi(BuscarRegistro));
            }else {
                AcaBusca = (Llave > BuscarRegistro);
            }
            if(AcaBusca){
                if(i == Raiz.nRegistros - 1) 
                    index += 1;
                break;
            }
        }
        
        if(!LlegoAHoja)
            Raiz = ReadIndex(Raiz.posHijos[index]);
        else 
            IndexPagina = Raiz.posHijos[index];
    }
    PageRecord Pagina = ReadBucket(IndexPagina);
    string Llave;
    bool AcaEsta = false;
    for(int i = 0; i < Pagina.nRegistros; i++){
        Llave = string(Pagina.registros[i].data[KEY_INDEX]);
        if(DATATYPE == 1){
            AcaEsta = (stoi(Llave) == stoi(BuscarRegistro));
        }else {
            AcaEsta = (Llave == BuscarRegistro);
        }
        if(AcaEsta){
            return {IndexPagina, Pagina.registros[i]};
        }
    }
    // No se encontro
    Record vacio;
    return {-1, vacio};
};
vector<Record> BPlusTree::RangeSearch(string PrimerRegistro, string UltimoRegistro){
    pair<int, Record> Inicio = Search(PrimerRegistro);
    PageRecord Pagina = ReadBucket(Inicio.first);

    string Llave; int IndicePrimerRegistro;
    bool AcaEsta = false;
    for(int i = 0; i < Pagina.nRegistros; i++){
        Llave = string(Pagina.registros[i].data[KEY_INDEX]);
        if(DATATYPE == 1){
            AcaEsta = (stoi(Llave) == stoi(PrimerRegistro));
        }else {
            AcaEsta = (Llave == PrimerRegistro);
        }
        if(AcaEsta){
            IndicePrimerRegistro = i;
            break;
        }
    }

    vector<Record> Encontrados;
    bool SonMenores = true;
    while(Pagina.next != 1 && SonMenores){
        for(int i = IndicePrimerRegistro; i < Pagina.nRegistros;i ++){
            Llave = string(Pagina.registros[i].data[KEY_INDEX]);
            if(DATATYPE == 1){
                SonMenores = (stoi(Llave) > stoi(UltimoRegistro));
            }else {
                SonMenores = (Llave > PrimerRegistro);
            }
            if(AcaEsta){
                break;
            }
            Encontrados.push_back(Pagina.registros[i]);
        }
        Pagina = ReadBucket(Pagina.next);
    }

    return Encontrados;
};

void BPlusTree::ReadValues(){
    PageRecord Buffer;
    unordered_set<int> indices;
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
        // cout << "llega a las hojas" << endl;
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
        indices.insert(FisrtBucket);
    }
    else {
        Buffer = ReadBucket(pos_root);
        indices.insert(pos_root);
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
    cout << "insert.begin()" << endl;
    for(auto i = indices.begin(); i != indices.end(); i++){
        cout << *i << endl;
    }
    while(Buffer.next != -1){
        // cout << "entra-------------------------------" << endl;
        
        Buffer = ReadBucket(Buffer.next);
        cout << "Buffer.nRegistros: " << Buffer.nRegistros << endl;
        cout << "Buffer.next: " << Buffer.next << endl;
        cout << "Buffer.prev: " << Buffer.prev << endl;
        if(indices.find(Buffer.next) != indices.end()){
            cout << "esta kagao\n";
            cout << "Buffer.nregistros: "<< Buffer.nRegistros << endl;
            cout << "Buffer.next: " << Buffer.next << endl;
            cout << "Buffer.prev: " << Buffer.prev << endl;
            return;
        }
        indices.insert(Buffer.next);
        // Page.seekg(Buffer.next, ios::beg);
        // Page.read((char *)&Buffer, 12 + NCOLS*255);

        for(int i = 0; i < Buffer.nRegistros; i++){
            registros.push_back(Buffer.registros[i]);
        }
        // k++; if(k > 10) return;
        // cout << "siguiente-------------------------------" << endl;
    };
    cout << "registros.size(): " << registros.size() << endl;
    for(int i = 0; i < registros.size(); i ++){
        registros[i].showData(); cout << " ";
        if (i > 0){
            if(stoi(string(registros[i].data[KEY_INDEX])) != stoi(string(registros[i - 1].data[KEY_INDEX])) + 1) {
                cout << "se kago\n";
                break;
            }
        }
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