#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>

#define HEAD_FILE 8;


using namespace std;

template<class T>
struct Record {
    T key;
    char nombre[100]{};
    char apellidos[100]{};
    char puesto[20]{};
    int sueldo{};

    Record()= default;
    Record(T key, const string& nombre_, const string& apellidos_, const string& puesto_, int sueldo_) {
        this->key = static_cast<T>(key);
        strcpy(this->nombre, nombre_.c_str());
        strcpy(this->apellidos, apellidos_.c_str());
        strcpy(this->puesto, puesto_.c_str());
        this->sueldo = sueldo_;
    }
    void show() const {
        cout << this->key;
        cout << "\t" << this->nombre;
        cout << "\t" << this->apellidos;
        cout << "\t" << this->puesto;
        cout << "\t" << this->sueldo;
    }
};

#define BYTES_TO_NEXTDEL 0
#define BYTES_TO_LEFT  4
#define BYTES_TO_RIGHT  8
#define BYTES_TO_HEIGHT  12

template<class T>
struct NodeAVL {
    long nextDel;
    long left;
    long right;
    int height;
    Record<T> data;
    NodeAVL(Record<T> record): data(record), left(-1), right(-1), height(0), nextDel(-1) {}
    NodeAVL(){}
    int getKey(){return data.key;}
};

struct HeadAVL {
    long root=-1;
    long nextDel=-1;
};

struct LeavesAVL {
    long left;
    long right;
};

template<class T>
long getPosFisical(long pos) {
    return pos*sizeof(NodeAVL<T>)+HEAD_FILE;
}

template<class T>
class AVLFile {
    HeadAVL head;
    string filename;

    int getBalance(long pos) {
        fstream file(filename, ios::in | ios::binary);
        LeavesAVL leaves;
        file.seekg(getPosFisical<T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.read((char*)&leaves, sizeof(LeavesAVL));

        if(leaves.left ==-1 && leaves.right==-1) {
            return 0;
        }
        int hLeft=-1 , hRight=-1;
        if(leaves.left > -1) {
            file.seekg(getPosFisical<T>(leaves.left)+BYTES_TO_HEIGHT, ios::beg);
            file.read((char*)&hLeft, sizeof(int));
        }

        if(leaves.right > -1) {
            file.seekg(getPosFisical<T>(leaves.right)+BYTES_TO_HEIGHT, ios::beg);
            file.read((char*)&hRight, sizeof(int));
        }
        file.close();
        return hLeft - hRight;
    }
    int getHeight(long pos) {
        if(pos<-1)
            return -1;
        fstream file(filename, ios::in | ios::binary);
        int height=0;
        file.seekg(getPosFisical<T>(pos)+BYTES_TO_HEIGHT, ios::beg);
        file.read((char*)&height, sizeof(int));
        file.close();
        return height;
    }
    template<class K>
    K read(fstream& file, long pos) {
        K data;
        file.seekg(pos, ios::beg);
        file.read((char*)&data, sizeof(K));
        return data;
    }
    template<class M>
    void write(fstream& file, long pos, M data) {
        file.seekp(pos, ios::beg);
        file.write((char*)&data, sizeof(M));
    }
    long rotateRight(long pos) {
        long x ,t, rightPos;
        fstream file(filename, ios::in | ios::out | ios::ate | ios::binary);
        /*file.seekg(getPosFisical<T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.read((char*)&x, sizeof(long));
        file.seekg(getPosFisical<T>(x)+BYTES_TO_RIGHT, ios::beg);
        file.read((char*)&t, sizeof(long));*/
        x = read<long>(file,getPosFisical<T>(pos)+BYTES_TO_LEFT);
        file.read((char*)&rightPos, sizeof(long));//
        t = read<long>(file,getPosFisical<T>(x)+BYTES_TO_RIGHT);

        write(file,getPosFisical<T>(x)+BYTES_TO_RIGHT,pos);
        write(file,getPosFisical<T>(pos)+BYTES_TO_LEFT,t);/*
        file.seekp(getPosFisical<T>(x)+BYTES_TO_RIGHT, ios::beg);
        file.write((char*)&pos, sizeof(long));
        file.seekp(getPosFisical<T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.write((char*)&t, sizeof(long));*/

        int height= max(getHeight(x), getHeight(rightPos))+1;
        write(file,getPosFisical<T>(pos)+BYTES_TO_HEIGHT,height);
        int heightX = max(getHeight(read<long>(file,getPosFisical<T>(x)+BYTES_TO_LEFT)),getHeight(read<long>(file,getPosFisical<T>(x)+BYTES_TO_RIGHT)))+1;
        write(file, getPosFisical<T>(x)+BYTES_TO_HEIGHT, heightX);
        file.close();
        return x;
    };
    long rotateLeft(long pos){
        long y ,t, leftPos;
        fstream file(filename, ios::in | ios::out | ios::ate | ios::binary);
        /*file.seekg(getPosFisical<T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.read((char*)&x, sizeof(long));
        file.seekg(getPosFisical<T>(x)+BYTES_TO_RIGHT, ios::beg);
        file.read((char*)&t, sizeof(long));*/
        leftPos = read<long>(file,getPosFisical<T>(pos)+BYTES_TO_LEFT);
        file.read((char*)&y, sizeof(long));//
        t = read<long>(file,getPosFisical<T>(y)+BYTES_TO_LEFT);

        write(file,getPosFisical<T>(y)+BYTES_TO_LEFT,pos);
        write(file,getPosFisical<T>(pos)+BYTES_TO_RIGHT,t);/*
        file.seekp(getPosFisical<T>(x)+BYTES_TO_RIGHT, ios::beg);
        file.write((char*)&pos, sizeof(long));
        file.seekp(getPosFisical<T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.write((char*)&t, sizeof(long));*/

        int height= max(getHeight(y), getHeight(leftPos))+1;
        write(file,getPosFisical<T>(pos)+BYTES_TO_HEIGHT,height);
        int heightY = max(getHeight(read<long>(file,getPosFisical<T>(y)+BYTES_TO_LEFT)),getHeight(read<long>(file,getPosFisical<T>(y)+BYTES_TO_RIGHT)))+1;
        write(file, getPosFisical<T>(y)+BYTES_TO_HEIGHT, heightY);
        file.close();
        return y;
    };

    void rewriteHead() {
        fstream file(filename, ios::in | ios::out | ios::ate | ios::binary);
        file.seekp(0, ios::beg);
        file.write((char*)&head, sizeof(HeadAVL));
        file.close();
    }

    pair<bool, long> insert(long posRoot, Record<T> record) {
        bool isAdded = true;
        fstream file;
        if(posRoot == -1) {
            NodeAVL<T> node(record);
            long pos;
            if (head.nextDel == -1 ) {
                file.open(filename, ios::out | ios::app | ios::binary);
                file.seekp(0, ios::end);
                pos = file.tellp();
                file.write((char*)&node, sizeof(NodeAVL<T>));

            }
            else {
                file.open(filename, ios::in | ios::out | ios::ate | ios::binary);
                file.seekp(getPosFisical<T>(head.nextDel), ios::beg);
                pos = file.tellp();
                NodeAVL<T> nodeDeleted;
                file.read((char*)&nodeDeleted, sizeof(NodeAVL<T>));
                write(file,getPosFisical<T>(nodeDeleted.nextDel),node);
                head.nextDel = nodeDeleted.nextDel;
            }
            file.close();
            long sizeHeadAVL = sizeof(HeadAVL);
            long sizeNodeAVL = sizeof(NodeAVL<T>);
            long newpos = (pos-sizeHeadAVL)/sizeNodeAVL;
            return make_pair(true, newpos);
        }
        file.open(filename, ios::in | ios::out | ios::ate | ios::binary);
        NodeAVL<T> nodeRoot = read<NodeAVL<T>>(file,getPosFisical<T>(posRoot));
        if (record.key < nodeRoot.getKey()) {
            auto result = insert(nodeRoot.left,record);
            write(file,getPosFisical<T>(posRoot)+BYTES_TO_LEFT,result.second);
            isAdded = result.first;
        }
        else if (record.key > nodeRoot.getKey()) {
            auto result = insert(nodeRoot.right,record);
            write(file,getPosFisical<T>(posRoot)+BYTES_TO_RIGHT,result.second);
            isAdded = result.first;
        }
        else
            return make_pair(false, posRoot);


        int height = max(getHeight(nodeRoot.left),getHeight(nodeRoot.right))+1;
        write(file,getPosFisical<T>(posRoot)+BYTES_TO_HEIGHT,height);

        int balance = getBalance(posRoot);

        NodeAVL<T> leaveLeft = read<NodeAVL<T>>(file,getPosFisical<T>(nodeRoot.left));
        NodeAVL<T> leaveRight = read<NodeAVL<T>>(file,getPosFisical<T>(nodeRoot.right));

        if (balance > 1 && record.key < leaveLeft.getKey()) {
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.key > leaveRight.getKey()) {
            return make_pair(true, rotateLeft(posRoot));
        }
        if (balance > 1 && record.key > leaveLeft.getKey()) {
            long leftRoot = rotateLeft(nodeRoot.left);
            write(file, getPosFisical<T>(posRoot)+BYTES_TO_LEFT,leftRoot);
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.key < leaveRight.getKey()) {
            long rightRoot = rotateRight(nodeRoot.right);
            write(file, getPosFisical<T>(posRoot)+BYTES_TO_RIGHT,rightRoot);
            return make_pair(true, rotateLeft(posRoot));
        }
        return make_pair(isAdded, posRoot);
    }

public:
    AVLFile(string filename): filename(filename) {
        fstream file(filename);
        if (!file.good()) {
            file.close();
            file.open(filename, ios::out | ios::binary);
            file.write((char*)&head, sizeof(HeadAVL));
        }
        else {
            file.read((char*)&head, sizeof(HeadAVL));
            cout<<"Root: "<<head.root<<endl;
            cout<<"NextDel: "<<head.nextDel<<endl;
        }
        file.close();
    }

    void viewFile() {
        fstream file(filename, ios::in | ios::binary);
        file.seekg(0, ios::beg);
        file.read((char*)&head, sizeof(HeadAVL));
        cout<<"Root: "<<head.root<<"\tNextDel: "<<head.nextDel<<endl;
        while(file.peek() != EOF) {
            NodeAVL<T> node;
            file.read((char*)&node, sizeof(NodeAVL<T>));
            cout<<"Left: "<<node.left<<"\tRight: "<<node.right;
            cout<<"\tHeight: "<<node.height<<"\tnextDel: "<<node.nextDel<<"\t";
            node.data.show();
            cout<<endl;
        }
    }

    bool add(Record<T> record) {
        auto root = insert(head.root, record);
        head.root = root.second;
        rewriteHead();
        return true;
    }






};


int main() {
    AVLFile<int> avl("avlfile.dat");

    avl.add(Record<int>(10008, "Claudio", "Echarre","" ,1500));
    avl.add(Record<int>(10009, "Aaron", "Navaro","" ,1000));
    avl.viewFile();
    avl.add(Record<int>(10010, "Marcelino", "Vargas","" ,1300));
    avl.viewFile();
    avl.add(Record<int>(10020, "Piero", "Guerrero","" ,1000));
    avl.viewFile();
    avl.add(Record<int>(10015, "Juan", "Aquino","" ,2000));
    /*
    cout << "Recorrido Inorder:" << endl;
    avl.inorder();

    cout << "\nBusqueda por rango:" << endl;
    vector<Record<int>> result = avl.rangeSearch(10010, 10015);
    for (const auto& rec : result) {
        cout << rec.key << ": " << rec.nombre<< "->" << rec.apellido<< "->" << rec.sueldo << endl;
    }
    */
    avl.viewFile();
    /*avl.add(Record<int>(10010, "Claudio", "Echarre","CS" ,1500));
    avl.viewFile();*/
    //avl.add(Record<int>(10011, "Juan", "Navaro","" ,1000));
    //avl.viewFile();
    return 0;
}
