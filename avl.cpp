#include "avl.h"

using namespace std;

template<class R,class T>
void test_insert1(AVLFile<R,T> &avl) {
    /*Comentando por cambio en struct Record
    avl.add(Record<int>(10008, "Claudio", "Echarre","" ,1500));
    avl.add(Record<int>(10009, "Aaron", "Navaro","" ,1000));
    avl.add(Record<int>(10010, "Marcelino", "Vargas","" ,1300));
    avl.add(Record<int>(10020, "Piero", "Guerrero","" ,1000));
    avl.add(Record<int>(10015, "Juan", "Aquino","" ,2000));
    avl.add(Record<int>(10025, "Jef", "Farfan","" ,1000));
    avl.viewFile();
    */
}
template<class R,class T>
void test_remove(AVLFile<R,T> &avl) {
    avl.remove(10015);
    avl.viewFile();

    avl.remove(10009);
    avl.viewFile();
}
template<class R,class T>
void test_insert2(AVLFile<R,T> &avl) {
    /*Comentando por cambio en struct Record
    avl.viewFile();
    avl.add(Record<int>(10009, "Aaron", "Navaro","" ,1000));
    avl.viewFile();
    */
}
template<class R,class T>
void test_insert3(AVLFile<R, T> &avl) {
    /*Comentando por cambio en struct Record
    avl.viewFile();
    avl.add(Record<int>(10015, "Juan", "Aquino","" ,2000));
    avl.viewFile();
    */
}

template<class R, class T>
void test_search(AVLFile<R, T> &avl) {
    avl.search(10025);
    avl.viewFile();
}

template<class R, class T>
void test_rangeSearch(AVLFile<R,T> &avl) {
    vector<R> vec =  avl.rangeSearch(10012,10025);
}


template<class T>
struct Record {
    char key[260];
    char data[80]{};
    Record()= default;
    Record(T key_, const string& data_) {
        strcpy(this->key, convertToString(key_).c_str());
        strcpy(this->data, data_.c_str());
    }
    void show() const {
        cout << this->key;
        cout << "\t" << this->data;
    }

    string getData() {
        string output = concatenate(key,",") +data+"\n";
        return output;
    }
    T getKey() const {
        return convert<T>(key);
    }

};



template<class R, class T>
static void buildFromCSV(AVLFile<R, T> &avl,string fileName, int idPosition) {
    fstream file(fileName);
    if (!file.good()) {
        cerr<<"File not found: "<<fileName<<endl;
        return;
    }
    string campos;
    getline(file,campos);
    while(!file.eof()) {
        string line, id="", field;
        getline(file,line);
        istringstream lineStream(line);
        string data="";
        int count = 0;
        while(getline(lineStream,field, ',')) {
            if (count == idPosition) {
                id = field;
            }
            else {
                if (data!="")
                    data+=",";
                data += field;
            }

            count++;
        }
        if (data!="" || id!="") {
            Record<T> record(id, data.c_str());
            avl.add(record);
        }

    }
}

int main() {
    AVLFile<Record<string>,string> avl("avlfile.dat");
    //test_insert1(avl);
    /*test_remove(avl);
    test_insert2(avl);
    test_insert3(avl);
    test_search(avl);*/
    avl.viewFile();
    buildFromCSV(avl,"space_travellers.csv",1);
    avl.remove("Pavel Popovich");
    avl.viewFile();/*
    for(int i =0; i<642; i++)
         avl.search(i);
     */

    return 0;
}