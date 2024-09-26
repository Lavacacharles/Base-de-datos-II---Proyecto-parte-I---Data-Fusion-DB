#ifndef AVL_H
#define AVL_H
#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <string>
#include <chrono>

#define HEAD_FILE 8;
#define BYTES_TO_NEXTDEL 0
#define BYTES_TO_LEFT  4
#define BYTES_TO_RIGHT  8
#define BYTES_TO_HEIGHT  12
#define BYTES_TO_DATA 16

using namespace std;

inline string operator+(const string& str, const char* arr_char) {
    return str + string(arr_char);
}
inline string operator+(const char* arr_char, const string& str) {
    return string(arr_char) + str;
}
template<class T>
string concatenate(T char1, const char* arr_char2) {
    if constexpr (is_same<T, char*>::value) {
        return char1 + string(arr_char2);
    }
    else if constexpr (is_same<T, int>::value) {
        return to_string(char1) + string(arr_char2);
    }
    else if constexpr (is_same<T, float>::value) {
        return to_string(char1) + string(arr_char2);
    }

    else if constexpr (is_same<T, string>::value) {
        return char1+string(arr_char2);
    }
    else {
        throw std::invalid_argument("Tipo no soportado");
    }
}

template<class T>
string convertToString (T input) {
    if constexpr (is_same<T, int>::value) {
        return to_string(input);
    }
    else if constexpr (is_same<T, string>::value) {
        return input;
    }
    else {
        throw std::invalid_argument("Tipo no soportado");
    }
}

template<class T>
T convert (string input) {
    if constexpr (is_same<T, int>::value) {
        return atoi(input.c_str());
    }
    else if constexpr (is_same<T, string>::value) {
        return string(input);
    }
    else {
        throw std::invalid_argument("Tipo no soportado");
    }
}

bool operator>(const string& str, const char* charArray) {
    return str > string(charArray);
}
bool operator<(const string& str, const char* charArray) {
    return str < string(charArray);
}

template<class R,class T>
struct NodeAVL {
    long nextDel;
    long left;
    long right;
    int height;
    R data;
    NodeAVL(R record): data(record), left(-1), right(-1), height(0), nextDel(-1) {}
    NodeAVL(){}
    T getKey(){return data.getKey();}
};

struct HeadAVL {
    long root=-1;
    long nextDel=-1;
};

struct LeavesAVL {
    long left;
    long right;
};

template<class R, class T>
long getPosFisical(long pos) {
    return pos*sizeof(NodeAVL<R,T>)+HEAD_FILE;
}

template<class R, class T>
class AVLFile {
    HeadAVL head;
    string filename;
    string result = "output.csv";

    int getBalance(long pos) {
        ifstream file(filename, ios::in | ios::binary);
        LeavesAVL leaves;
        file.seekg(getPosFisical<R,T>(pos)+BYTES_TO_LEFT, ios::beg);
        file.read((char*)&leaves, sizeof(LeavesAVL));

        if(leaves.left ==-1 && leaves.right==-1) {
            return 0;
        }
        int hLeft=-1 , hRight=-1;
        if(leaves.left > -1) {
            file.seekg(getPosFisical<R,T>(leaves.left)+BYTES_TO_HEIGHT, ios::beg);
            file.read((char*)&hLeft, sizeof(int));
        }

        if(leaves.right > -1) {
            file.seekg(getPosFisical<R,T>(leaves.right)+BYTES_TO_HEIGHT, ios::beg);
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
        file.seekg(getPosFisical<R,T>(pos)+BYTES_TO_HEIGHT, ios::beg);
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
        x = read<long>(getPosFisical<R,T>(pos)+BYTES_TO_LEFT);
        rightPos = read<long>(getPosFisical<R,T>(pos)+BYTES_TO_RIGHT);
        t = read<long>(getPosFisical<R,T>(x)+BYTES_TO_RIGHT);
        write<long>(getPosFisical<R,T>(x)+BYTES_TO_RIGHT,pos);
        write<long>(getPosFisical<R,T>(pos)+BYTES_TO_LEFT,t);

        int height= max(getHeight(t), getHeight(rightPos))+1;
        write<int>(getPosFisical<R,T>(pos)+BYTES_TO_HEIGHT,height);
        int heightX = max(getHeight(pos),getHeight(read<long>(getPosFisical<R,T>(x)+BYTES_TO_RIGHT)))+1;
        write<int>(getPosFisical<R,T>(x)+BYTES_TO_HEIGHT, heightX);
        return x;
    };

    long rotateLeft(long pos){
        long y ,t, leftPos;
        leftPos = read<long>(getPosFisical<R,T>(pos)+BYTES_TO_LEFT);
        y = read<long>(getPosFisical<R,T>(pos)+BYTES_TO_RIGHT);
        t = read<long>(getPosFisical<R,T>(y)+BYTES_TO_LEFT);
        write<long>(getPosFisical<R,T>(y)+BYTES_TO_LEFT,pos);
        write<long>(getPosFisical<R,T>(pos)+BYTES_TO_RIGHT,t);
        int height= max(getHeight(t), getHeight(leftPos))+1;
        write<int>(getPosFisical<R,T>(pos)+BYTES_TO_HEIGHT,height);
        int heightY = max(getHeight(pos),getHeight(read<long>(getPosFisical<R,T>(y)+BYTES_TO_RIGHT)))+1;
        write<int>(getPosFisical<R,T>(y)+BYTES_TO_HEIGHT, heightY);
        return y;
    };

    void rewriteHead() {
        ofstream file(filename, ios::in | ios::out | ios::ate | ios::binary);
        file.seekp(0, ios::beg);
        file.write((char*)&head, sizeof(HeadAVL));
        file.flush();
        file.close();
    }

    pair<bool, long> insert(long posRoot, R record) {
        bool isAdded = true;
        fstream file;
        if(posRoot == -1) {
            NodeAVL<R,T> node(record);
            long pos;
            if (head.nextDel == -1 ) {
                file.open(filename, ios::in | ios::out | ios::app | ios::binary);
                file.seekp(0, ios::end);
                pos = file.tellp();
                file.write((char*)&node, sizeof(NodeAVL<R,T>));
                file.flush();

            }
            else {
                file.open(filename, ios::in | ios::out | ios::ate | ios::binary);
                file.seekp(getPosFisical<R,T>(head.nextDel), ios::beg);
                pos = file.tellp();
                NodeAVL<R,T> nodeDeleted;
                file.read((char*)&nodeDeleted, sizeof(NodeAVL<R,T>));
                write<NodeAVL<R,T>>(getPosFisical<R,T>(head.nextDel),node);
                posRoot = head.nextDel;
                head.nextDel = nodeDeleted.nextDel;
            }
            file.close();
            long sizeHeadAVL = sizeof(HeadAVL);
            long sizeNodeAVL = sizeof(NodeAVL<R,T>);
            long newpos = (pos-sizeHeadAVL)/sizeNodeAVL;
            return make_pair(true, newpos);
        }
        NodeAVL<R,T> nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));
        if (record.getKey() < nodeRoot.getKey()) {
            auto result = insert(nodeRoot.left,record);
            write<long>(getPosFisical<R,T>(posRoot)+BYTES_TO_LEFT,result.second);
            isAdded = result.first;
        }
        else if (record.getKey() > nodeRoot.getKey()) {
            auto result = insert(nodeRoot.right,record);
            write<long>(getPosFisical<R,T>(posRoot)+BYTES_TO_RIGHT,result.second);
            isAdded = result.first;
        }
        else
            return make_pair(false, posRoot);

        nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));
        int height = max(getHeight(nodeRoot.left),getHeight(nodeRoot.right))+1;
        write<int>(getPosFisical<R,T>(posRoot)+BYTES_TO_HEIGHT,height);

        int balance = getBalance(posRoot);

        nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));

        NodeAVL<R,T>* leaveLeft = (nodeRoot.left>-1)? new NodeAVL<R,T>(read<NodeAVL<R,T>>(getPosFisical<R,T>(nodeRoot.left))):nullptr;
        NodeAVL<R,T>* leaveRight = (nodeRoot.right>-1)?new NodeAVL<R,T>(read<NodeAVL<R,T>>(getPosFisical<R,T>(nodeRoot.right))):nullptr;

        if (balance > 1 && record.getKey() < leaveLeft->getKey() && nodeRoot.left >-1) {
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.getKey() > leaveRight->getKey() && nodeRoot.right >-1) {
            return make_pair(true, rotateLeft(posRoot));
        }
        if (balance > 1 && record.getKey() > leaveLeft->getKey() && nodeRoot.left >-1) {
            long leftRoot = rotateLeft(nodeRoot.left);
            write<long>(getPosFisical<R,T>(posRoot)+BYTES_TO_LEFT,leftRoot);
            return make_pair(true, rotateRight(posRoot));
        }
        if (balance < -1 && record.getKey() < leaveRight->getKey() && nodeRoot.right > -1) {
            long rightRoot = rotateRight(nodeRoot.right);
            write<long>(getPosFisical<R,T>(posRoot)+BYTES_TO_RIGHT,rightRoot);
            return make_pair(true, rotateLeft(posRoot));
        }
        file.close();
        return make_pair(isAdded, posRoot);
    }

    long mininValue(long pos) {
        auto* leaves = new LeavesAVL (read<LeavesAVL>(getPosFisical<R,T>(pos)+BYTES_TO_LEFT));
        while(leaves->left>-1) {
            pos = leaves->left;
            leaves = new LeavesAVL(read<LeavesAVL>(getPosFisical<R,T>(leaves->left)+BYTES_TO_LEFT));
        }
        delete leaves;
        return pos;
    }

    pair<bool, long> deleteFL(long posRoot, T key){
        bool wasDeleted = true;
        if (posRoot == -1)
            return make_pair(false, posRoot);

        NodeAVL<R,T> nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));

        if (key < nodeRoot.getKey()) {
            auto result = deleteFL(nodeRoot.left, key);
            wasDeleted = result.first;
            write<long>(getPosFisical<R,T>(posRoot) + BYTES_TO_LEFT, result.second);
        }
        else if (key > nodeRoot.getKey()) {
            auto result = deleteFL(nodeRoot.right, key);
            wasDeleted = result.first;
            write<long>(getPosFisical<R,T>(posRoot) + BYTES_TO_RIGHT, result.second);
        }
        else {
            if (nodeRoot.left == -1 && nodeRoot.right == -1) {
                nodeRoot.nextDel = head.nextDel;
                head.nextDel = posRoot;
                write<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot), nodeRoot);
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
            NodeAVL<R,T> minRightNode = read<NodeAVL<R,T>>(getPosFisical<R,T>(posMinRight));

            nodeRoot.data = minRightNode.data;
            //write<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot), nodeRoot);
            write<R>(getPosFisical<R,T>(posRoot)+BYTES_TO_DATA, nodeRoot.data);

            auto result = deleteFL(nodeRoot.right, minRightNode.getKey());
            write<long>(getPosFisical<R,T>(posRoot) + BYTES_TO_RIGHT, result.second);
        }

        int height = max(getHeight(nodeRoot.left), getHeight(nodeRoot.right)) + 1;
        write<int>(getPosFisical<R,T>(posRoot) + BYTES_TO_HEIGHT, height);

        int balance = getBalance(posRoot);

        if (balance > 1 && getBalance(nodeRoot.left) >= 0) {
            return make_pair(true, rotateRight(posRoot));
        }

        if (balance > 1 && getBalance(nodeRoot.left) < 0) {
            write<long>(getPosFisical<R,T>(nodeRoot.left) + BYTES_TO_LEFT, rotateLeft(nodeRoot.left));
            return make_pair(true, rotateRight(posRoot));
        }

        if (balance < -1 && getBalance(nodeRoot.right) <= 0) {
            return make_pair(true, rotateLeft(posRoot));
        }

        if (balance < -1 && getBalance(nodeRoot.right) > 0) {
            write<long>(getPosFisical<R,T>(nodeRoot.right) + BYTES_TO_RIGHT, rotateRight(nodeRoot.right));
            return make_pair(true, rotateLeft(posRoot));
        }

        return make_pair(wasDeleted, posRoot);
    }

    pair<bool, R> search(long posRoot, T key) {
        if (posRoot == -1)
            return make_pair(false, R());

        NodeAVL<R,T> nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));

        if (key < nodeRoot.getKey())
            return search(nodeRoot.left, key);

        if (key > nodeRoot.getKey())
            return search(nodeRoot.right, key);

        return make_pair(true, nodeRoot.data);
    }

    void rangeSearch(long posRoot, T keyMin, T keyMax, vector<R>& result) {
        if (posRoot == -1) return;
        NodeAVL<R,T> nodeRoot = read<NodeAVL<R,T>>(getPosFisical<R,T>(posRoot));
        if (keyMin < nodeRoot.getKey())
            rangeSearch(nodeRoot.left, keyMin, keyMax, result);

        if (keyMin <= nodeRoot.getKey() && nodeRoot.getKey() <= keyMax)
            result.push_back(nodeRoot.data);

        if (keyMax > nodeRoot.getKey())
            rangeSearch(nodeRoot.right, keyMin, keyMax, result);
    }

    void writeOutputFile(R& record, const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(state) {
            outputFile<<record.getData();
        }
    }
    void writeOutputFile(vector<R>& vector, const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(state) {
            for(int i = 0; i < vector.size(); i++) {
                outputFile<<vector[i].getData();
            }
        }
    }
    void writeOutputFile(const bool state) {
        ofstream outputFile;
        outputFile.open(result,ios::out|ios::trunc);
        if(!state) {
            string outputString = "Process failed\n";
            outputFile<<outputString;
            return;
        }
        string outputString = "Execution successful\n";
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
            NodeAVL<R,T> node;
            file.read((char*)&node, sizeof(NodeAVL<R,T>));
            cout<<count<<" : ";
            cout<<"Left("<<node.left<<") - Right("<<node.right;
            cout<<") - Height("<<node.height<<") - nextDel("<<node.nextDel<<")\t";
            node.data.show();
            cout<<endl;
            count++;
        }
    }

    bool add(R record) {
        auto root = insert(head.root, record);
        if(root.first) {
            HeadAVL nodeRoot = read<HeadAVL>(0);
            if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
                if (head.root != root.second)
                    head.root = root.second;
                rewriteHead();
            }
        }
        writeOutputFile(root.first);
        cout<<(root.first?"Insercion correcta del ":"Ya existe el ")<<"registro con key("<<record.getKey()<<")\n";
        return root.first;
    }

    bool remove(T key) {
        auto root = deleteFL(head.root, key);
        if(root.first) {
            HeadAVL nodeRoot = read<HeadAVL>(0);
            if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
                if (head.root != root.second)
                    head.root = root.second;
                rewriteHead();
            }
        }
        writeOutputFile(root.first);
        cout<<(root.first?"Eliminacion correcta del ":"No se encontro el ")<<"registro con key("<<key<<")\n";
        return root.first;
    }

    R search(T key) {
        auto result = search(head.root, key);
        writeOutputFile(result.second,result.first);
        cout<<((result.first)?"Registro encontrado":"No se encontró el registro")<<"con key(" << key << "):\n";
        result.second.show();
        cout<<endl;

        return result.second;
    }

    vector<R> rangeSearch(T keyMin, T keyMax) {
        vector<R> result;
        rangeSearch(head.root, keyMin, keyMax, result);

        bool state = (!result.empty())?true:false;
        writeOutputFile(result,state);

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


#endif