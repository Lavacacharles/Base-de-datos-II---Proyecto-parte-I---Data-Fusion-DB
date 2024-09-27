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
    char key[100];
    char data[200]{};
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
static void buildFromCSV(AVLFile<R, T> &avl,string fileName, int idPosition, int N, int Nres, int cantFields) {
    fstream file(fileName);
    if (!file.good()) {
        cerr<<"File not found: "<<fileName<<endl;
        return;
    }
    string campos;
    getline(file,campos);
    int counter = 1;
    while(!file.eof() && N--) {
        string line, id="", data="", field;
        getline(file,line);
        if (counter > Nres) {
            istringstream lineStream(line);
            int count = 0;
            int countFields = cantFields;
            while(getline(lineStream,field, ',') && countFields--) {
                if (count == idPosition) {
                    id = field;
                }
                else {
                    if (!data.empty())
                        data+=",";
                    data += field;
                }

                count++;
            }
            if (data!="" || id!="") {
                Record<T> record(id, data.c_str());
                avl.add(record);
            }
            cout<<"Registro leido:"<<counter<<endl;
        }
        else
            cout<<"Registro no leido:"<<counter<<endl;
        counter++;


    }
}

template<class R, class T>
void viewPerformance_Build(AVLFile<R,T> &avl,int N, int Nres) {\
    auto start = chrono::high_resolution_clock::now();
    buildFromCSV(avl,"out.csv",0, N,Nres,3);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout<<endl<<endl <<"BUILD TABLE : Execution time in " <<duration.count() << "ms" <<endl<< endl;
    //avl.viewFile();
}
template<class R, class T>
void viewPerformance_ADD(AVLFile<R,T> &avl) {\
    auto start = chrono::high_resolution_clock::now();
    string id = "https://verifyacoount5375.duckdns.org/invalid.html";
    string data = "Phishing.Database,phishing";
    Record<string> record(id,data.c_str());
    avl.add(record);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout <<endl<<endl<<"ADD : Execution time in " <<duration.count() << " ms" << endl<<endl;
    //avl.viewFile();
}

template<class R, class T>
void viewPerformance_SEARCH(AVLFile<R,T> &avl) {\
    auto start = chrono::high_resolution_clock::now();
    string id = "ia800209.us.archive.org";
    avl.search(id);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout <<endl<<endl<<"SEARCH : Execution time in " <<duration.count() << " ms" <<endl<< endl;
    //avl.viewFile();
}

template<class R, class T>
void viewPerformance_RANGE_SEARCH(AVLFile<R,T> &avl) {\
    auto start = chrono::high_resolution_clock::now();
    string min = "https", max = "oracle";
    avl.rangeSearch(min, max);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout<<endl <<"RANGE_SEARCH : Execution time in " <<duration.count() << " ms" <<endl<< endl;
    //avl.viewFile();
}

template<class R, class T>
void viewPerformance_REMOVE(AVLFile<R,T> &avl) {\
    auto start = chrono::high_resolution_clock::now();
    string id = "https://verifyacoount5375.duckdns.org/invalid.html";
    avl.remove(id);
    auto end = chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    cout <<endl<<endl<<"REMOVE : Execution time in " <<duration.count() << "ms"<<endl << endl;
    //avl.viewFile();
}


int main() {
    AVLFile<Record<string>,string> avl("avlfile.dat");
    viewPerformance_Build(avl, 10,0);
    viewPerformance_ADD(avl);
    viewPerformance_SEARCH(avl);
    viewPerformance_RANGE_SEARCH(avl);
    //viewPerformance_REMOVE(avl);

    return 0;
}