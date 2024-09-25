#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <chrono>

#define HEAD_FILE 8;
#define BYTES_TO_NEXTDEL 0
#define BYTES_TO_LEFT  4
#define BYTES_TO_RIGHT  8
#define BYTES_TO_HEIGHT  12
#define BYTES_TO_DATA 16

using namespace std;

template<class T>
struct Record {
    T key;
    char nombre[15]{};
    char apellidos[15]{};
    char puesto[2]{};
    int sueldo=0;

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
    }

    string getData() {
        string output = to_string(key) + "," + nombre + "," + apellidos + "," + puesto + "," + to_string(sueldo)+"\n";
        return output;
    }
};

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
    string result = "output.csv";

    int getBalance(long pos) {
        ifstream file(filename, ios::in | ios::binary);
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
        if(pos==-1)
            return -1;
        ifstream file(filename, ios::in | ios::binary);
        int height=0;
        file.seekg(getPosFisical<T>(pos)+BYTES_TO_HEIGHT, ios::beg);
        file.read((char*)&height, sizeof(int));
        file.close();
        return height;
    }

    template<class K>
    K read(long pos) {
        K data;
        ifstream file(filename, ios::in | ios::binary);
        file.seekg(pos, ios::beg);
        int pos_ = file.tellg();
        file.read((char*)&data, sizeof(K));
        file.close();
        return data;
    }

    template<class M>
    void write(long pos, M data) {
        ofstream file(filename, ios::in |ios::out| ios::ate | ios::binary);
        file.seekp(pos, ios::beg);
        file.write((char*)&data, sizeof(M));
        file.flush();
        file.close();
    }

    long rotateRight(long pos) {
        long x ,t, rightPos;
        x = read<long>(getPosFisical<T>(pos)+BYTES_TO_LEFT);
        rightPos = read<long>(getPosFisical<T>(pos)+BYTES_TO_RIGHT);
        t = read<long>(getPosFisical<T>(x)+BYTES_TO_RIGHT);
        write<long>(getPosFisical<T>(x)+BYTES_TO_RIGHT,pos);
        write<long>(getPosFisical<T>(pos)+BYTES_TO_LEFT,t);

        int height= max(getHeight(t), getHeight(rightPos))+1;
        write<int>(getPosFisical<T>(pos)+BYTES_TO_HEIGHT,height);
        int heightX = max(getHeight(pos),getHeight(read<long>(getPosFisical<T>(x)+BYTES_TO_RIGHT)))+1;
        write<int>(getPosFisical<T>(x)+BYTES_TO_HEIGHT, heightX);
        return x;
    };

    long rotateLeft(long pos){
        long y ,t, leftPos;
        leftPos = read<long>(getPosFisical<T>(pos)+BYTES_TO_LEFT);
        y = read<long>(getPosFisical<T>(pos)+BYTES_TO_RIGHT);
        t = read<long>(getPosFisical<T>(y)+BYTES_TO_LEFT);
        write<long>(getPosFisical<T>(y)+BYTES_TO_LEFT,pos);
        write<long>(getPosFisical<T>(pos)+BYTES_TO_RIGHT,t);
        int height= max(getHeight(t), getHeight(leftPos))+1;
        write<int>(getPosFisical<T>(pos)+BYTES_TO_HEIGHT,height);
        int heightY = max(getHeight(pos),getHeight(read<long>(getPosFisical<T>(y)+BYTES_TO_RIGHT)))+1;
        write<int>(getPosFisical<T>(y)+BYTES_TO_HEIGHT, heightY);
        return y;
    };

    void rewriteHead() {
        ofstream file(filename, ios::in | ios::out | ios::ate | ios::binary);
        file.seekp(0, ios::beg);
        file.write((char*)&head, sizeof(HeadAVL));
        file.flush();
        file.close();
    }

    pair<bool, long> insert(long posRoot, Record<T> record) {
        bool isAdded = true;
        fstream file;
        if(posRoot == -1) {
            NodeAVL<T> node(record);
            long pos;
            if (head.nextDel == -1 ) {
                file.open(filename, ios::in | ios::out | ios::app | ios::binary);
                file.seekp(0, ios::end);
                pos = file.tellp();
                file.write((char*)&node, sizeof(NodeAVL<T>));
                file.flush();

            }
            else {
                file.open(filename, ios::in | ios::out | ios::ate | ios::binary);
                file.seekp(getPosFisical<T>(head.nextDel), ios::beg);
                pos = file.tellp();
                NodeAVL<T> nodeDeleted;
                file.read((char*)&nodeDeleted, sizeof(NodeAVL<T>));
                write<NodeAVL<T>>(getPosFisical<T>(head.nextDel),node);
                posRoot = head.nextDel;
                head.nextDel = nodeDeleted.nextDel;
            }
            file.close();
            long sizeHeadAVL = sizeof(HeadAVL);
            long sizeNodeAVL = sizeof(NodeAVL<T>);
            long newpos = (pos-sizeHeadAVL)/sizeNodeAVL;
            return make_pair(true, newpos);
        }
        NodeAVL<T> nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));
        if (record.key < nodeRoot.getKey()) {
            auto result = insert(nodeRoot.left,record);
            write<long>(getPosFisical<T>(posRoot)+BYTES_TO_LEFT,result.second);
            isAdded = result.first;
        }
        else if (record.key > nodeRoot.getKey()) {
            auto result = insert(nodeRoot.right,record);
            write<long>(getPosFisical<T>(posRoot)+BYTES_TO_RIGHT,result.second);
            isAdded = result.first;
        }
        else
            return make_pair(false, posRoot);

        nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));
        int height = max(getHeight(nodeRoot.left),getHeight(nodeRoot.right))+1;
        write<int>(getPosFisical<T>(posRoot)+BYTES_TO_HEIGHT,height);

        int balance = getBalance(posRoot);

        nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));

        NodeAVL<T>* leaveLeft = (nodeRoot.left>-1)? new NodeAVL<T>(read<NodeAVL<T>>(getPosFisical<T>(nodeRoot.left))):nullptr;
        NodeAVL<T>* leaveRight = (nodeRoot.right>-1)?new NodeAVL<T>(read<NodeAVL<T>>(getPosFisical<T>(nodeRoot.right))):nullptr;

        if (balance > 1 && record.key < leaveLeft->getKey() && nodeRoot.left >-1) {
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.key > leaveRight->getKey() && nodeRoot.right >-1) {
            return make_pair(true, rotateLeft(posRoot));
        }
        if (balance > 1 && record.key > leaveLeft->getKey() && nodeRoot.left >-1) {
            long leftRoot = rotateLeft(nodeRoot.left);
            write<long>(getPosFisical<T>(posRoot)+BYTES_TO_LEFT,leftRoot);
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.key < leaveRight->getKey() && nodeRoot.right > -1) {
            long rightRoot = rotateRight(nodeRoot.right);
            write<long>(getPosFisical<T>(posRoot)+BYTES_TO_RIGHT,rightRoot);
            return make_pair(true, rotateLeft(posRoot));
        }
        file.close();
        return make_pair(isAdded, posRoot);
    }

    long mininValue(long pos) {
        auto* leaves = new LeavesAVL (read<LeavesAVL>(getPosFisical<T>(pos)+BYTES_TO_LEFT));
        while(leaves->left>-1) {
            pos = leaves->left;
            leaves = new LeavesAVL(read<LeavesAVL>(getPosFisical<T>(leaves->left)+BYTES_TO_LEFT));
        }
        delete leaves;
        return pos;
    }

    pair<bool, long> deleteFL(long posRoot, T key){
        if (posRoot == -1)
            return make_pair(false, posRoot);

        NodeAVL<T> nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));

        if (key < nodeRoot.getKey()) {
            auto result = deleteFL(nodeRoot.left, key);
            write<long>(getPosFisical<T>(posRoot) + BYTES_TO_LEFT, result.second);
        }
        else if (key > nodeRoot.getKey()) {
            auto result = deleteFL(nodeRoot.right, key);
            write<long>(getPosFisical<T>(posRoot) + BYTES_TO_RIGHT, result.second);
        }
        else {
            if (nodeRoot.left == -1 && nodeRoot.right == -1) {
                nodeRoot.nextDel = head.nextDel;
                head.nextDel = posRoot;
                write<NodeAVL<T>>(getPosFisical<T>(posRoot), nodeRoot);
                return make_pair(true, -1);
            }
            if (nodeRoot.left == -1) {
                head.nextDel = posRoot;
                return make_pair(true, nodeRoot.right);
            }
            if (nodeRoot.right == -1) {
                head.nextDel = posRoot;
                return make_pair(true, nodeRoot.left);
            }
            long posMinRight = mininValue(nodeRoot.right);
            NodeAVL<T> minRightNode = read<NodeAVL<T>>(getPosFisical<T>(posMinRight));

            nodeRoot.data = minRightNode.data;
            //write<NodeAVL<T>>(getPosFisical<T>(posRoot), nodeRoot);
            write<Record<T>>(getPosFisical<T>(posRoot)+BYTES_TO_DATA, nodeRoot.data);

            auto result = deleteFL(nodeRoot.right, minRightNode.getKey());
            write<long>(getPosFisical<T>(posRoot) + BYTES_TO_RIGHT, result.second);
        }

        int height = max(getHeight(nodeRoot.left), getHeight(nodeRoot.right)) + 1;
        write<int>(getPosFisical<T>(posRoot) + BYTES_TO_HEIGHT, height);

        int balance = getBalance(posRoot);

        if (balance > 1 && getBalance(nodeRoot.left) >= 0) {
            return make_pair(true, rotateRight(posRoot));
        }

        if (balance > 1 && getBalance(nodeRoot.left) < 0) {
            write<long>(getPosFisical<T>(nodeRoot.left) + BYTES_TO_LEFT, rotateLeft(nodeRoot.left));
            return make_pair(true, rotateRight(posRoot));
        }

        if (balance < -1 && getBalance(nodeRoot.right) <= 0) {
            return make_pair(true, rotateLeft(posRoot));
        }

        if (balance < -1 && getBalance(nodeRoot.right) > 0) {
            write<long>(getPosFisical<T>(nodeRoot.right) + BYTES_TO_RIGHT, rotateRight(nodeRoot.right));
            return make_pair(true, rotateLeft(posRoot));
        }

        return make_pair(true, posRoot);
    }

    pair<bool, Record<T>> search(long posRoot, T key) {
        if (posRoot == -1)
            return make_pair(false, Record<T>());

        NodeAVL<T> nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));

        if (key < nodeRoot.getKey())
            return search(nodeRoot.left, key);

        if (key > nodeRoot.getKey())
            return search(nodeRoot.right, key);

        return make_pair(true, nodeRoot.data);
    }

    void rangeSearch(long posRoot, T keyMin, T keyMax, vector<Record<T>>& result) {
        if (posRoot == -1) return;
        NodeAVL<T> nodeRoot = read<NodeAVL<T>>(getPosFisical<T>(posRoot));
        if (keyMin < nodeRoot.getKey())
            rangeSearch(nodeRoot.left, keyMin, keyMax, result);

        if (keyMin <= nodeRoot.getKey() && nodeRoot.getKey() <= keyMax)
            result.push_back(nodeRoot.data);

        if (keyMax > nodeRoot.getKey())
            rangeSearch(nodeRoot.right, keyMin, keyMax, result);
    }

    void writeOutputFile(Record<T>& record, const int& duration, const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(state) {
            outputFile<<record.getData();
        }
        string outputString = "Execution time: "+to_string(duration)+"ms.\n";
        outputFile<<outputString;
    }
    void writeOutputFile(vector<Record<T>>& vector, const int& duration, const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(state) {
            for(int i = 0; i < vector.size(); i++) {
                outputFile<<vector[i].getData();
            }
        }
        string outputString = "Execution time: "+to_string(duration)+"ms.\n";
        outputFile<<outputString;
    }
    void writeOutputFile(const int& duration, const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(!state) {
            string outputString = "Process failed in "+to_string(duration)+"ms.\n";
            outputFile<<outputString;
            return;
        }
        string outputString = "Execution time: "+to_string(duration)+"ms.\n";
        outputFile<<outputString;
    }


public:
    AVLFile(string filename): filename(filename) {
        fstream file(filename);
        if (!file.good()) {
            file.close();
            file.open(filename, ios::out | ios::binary);
            file.write((char*)&head, sizeof(HeadAVL));
            file.flush();
        }
        else {
            file.read((char*)&head, sizeof(HeadAVL));
            cout<<"Root: "<<head.root<<endl;
            cout<<"NextDel: "<<head.nextDel<<endl;
        }
        file.close();
    }

    AVLFile(string filename, string output): filename(filename), result(output) {
        fstream file(filename);
        if (!file.good()) {
            file.close();
            file.open(filename, ios::out | ios::binary);
            file.write((char*)&head, sizeof(HeadAVL));
            file.flush();
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
        int count = 0;
        while(file.peek() != EOF) {
            NodeAVL<T> node;
            file.read((char*)&node, sizeof(NodeAVL<T>));
            cout<<count<<" : ";
            cout<<"Left("<<node.left<<") - Right("<<node.right;
            cout<<") - Height("<<node.height<<") - nextDel("<<node.nextDel<<")\t";
            node.data.show();
            cout<<endl;
            count++;
        }
    }

    bool add(Record<T> record) {
        auto start = chrono::high_resolution_clock::now();
        auto root = insert(head.root, record);
        auto end = chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if(root.first) {
            HeadAVL nodeRoot = read<HeadAVL>(0);
            if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
                if (head.root != root.second)
                    head.root = root.second;
                rewriteHead();
            }
        }
        writeOutputFile(duration.count(),root.first);
        cout<<(root.first?"Insercion correcta del ":"Ya existe el ")<<"registro con key("<<record.key<<")\n";
        return root.first;
    }

    bool remove(T key) {
        auto start = chrono::high_resolution_clock::now();
        auto root = deleteFL(head.root, key);
        auto end = chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if(root.first) {
            HeadAVL nodeRoot = read<HeadAVL>(0);
            if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
                if (head.root != root.second)
                    head.root = root.second;
                rewriteHead();
            }
        }
        writeOutputFile(duration.count(),root.first);
        cout<<(root.first?"Eliminacion correcta del ":"No se encontro el ")<<"registro con key("<<key<<")\n";
        return root.first;
    }

    Record<T> search(T key) {
        auto start = chrono::high_resolution_clock::now();
        auto result = search(head.root, key);
        auto end = chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        writeOutputFile(result.second,duration.count(),result.first);
        cout<<((result.first)?"Registro encontrado":"No se encontró el registro")<<"con key(" << key << "):\n";
        result.second.show();
        cout<<endl;

        return result.second;
    }

    vector<Record<T>> rangeSearch(T keyMin, T keyMax) {
        vector<Record<T>> result;
        auto start = chrono::high_resolution_clock::now();
        rangeSearch(head.root, keyMin, keyMax, result);
        auto end = chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

        bool state = (!result.empty())?true:false;
        writeOutputFile(result,duration.count(),state);

        if (state) {
            cout << "Registros en el rango [" << keyMin << ", " << keyMax << "]:\n";
            for (const auto& record : result) {
                record.show();
                cout << endl;
            }
        }
        else
            cout << "No se encontraron registros en el rango [" << keyMin << ", " << keyMax << "].\n";

        return result;
    }




};

template<class T>
void test_insert1(AVLFile<T> &avl) {
    avl.add(Record<int>(10008, "Claudio", "Echarre","" ,1500));
    avl.add(Record<int>(10009, "Aaron", "Navaro","" ,1000));
    avl.add(Record<int>(10010, "Marcelino", "Vargas","" ,1300));
    avl.add(Record<int>(10020, "Piero", "Guerrero","" ,1000));
    avl.add(Record<int>(10015, "Juan", "Aquino","" ,2000));
    avl.add(Record<int>(10025, "Jef", "Farfan","" ,1000));
    avl.viewFile();
}
template<class T>
void test_remove(AVLFile<T> &avl) {
    avl.remove(10015);
    avl.viewFile();

    avl.remove(10009);
    avl.viewFile();
}
template<class T>
void test_insert2(AVLFile<T> &avl) {
    avl.viewFile();
    avl.add(Record<int>(10009, "Aaron", "Navaro","" ,1000));
    avl.viewFile();
}
template<class T>
void test_insert3(AVLFile<T> &avl) {
    avl.viewFile();
    avl.add(Record<int>(10015, "Juan", "Aquino","" ,2000));
    avl.viewFile();
}

template<class T>
void test_search(AVLFile<T> &avl) {
    avl.search(10025);
    avl.viewFile();
}

template<class T>
void test_rangeSearch(AVLFile<T> &avl) {
    vector<Record<T>> vec =  avl.rangeSearch(10012,10025);
}

/*
int main() {
    AVLFile<int> avl("avlfile.dat");
    test_insert1(avl);
    test_remove(avl);
    test_insert2(avl);
    test_insert3(avl);
    test_search(avl);
    test_rangeSearch(avl);
    return 0;
}*/