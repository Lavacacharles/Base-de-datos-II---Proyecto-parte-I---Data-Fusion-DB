#include "bucket.h"
#include "hash.h"

using namespace std;

void test_general() {
  ExtendibleHashingFile hash("students", 3, 3);
  cout << "-----------------------------------------------------------" << endl;
  // 1 == 1, 22 == 10110, 32 == 100000
  hash.create_from_csv("datos_small.csv");
  cout << "Read from csv" << endl;
  // Only base case was tested
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search" << endl;
  string record_found = hash.search(22);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Insertion to fill bucket" << endl;
  // 10 == 1010, 10 == 1001
  hash.insert("10,Luisa,Martín,6");
  hash.insert("9,Manuela,Hernández,1");
  cout << "-----------------------------------------------------------" << endl;
  cout << "Insertion to divide bucket " << endl;
  // 8 == 1000, 14 == 1110
  hash.insert("8,Rosario,Jiménez,4");
  hash.insert("14,Sandra,Ruiz,6");
  cout << "-----------------------------------------------------------" << endl;
}

void test_m() {
  ExtendibleHashingFile hash("studentsM", 3, 3);
  cout << "in main" << endl;
  cout << "-----------------------------------------------------------" << endl;
  hash.create_from_csv("datos_m.csv");
  cout << "Read from moderate csv to have chaining" << endl;
}

int main() {
  ExtendibleHashingFile hash("studentsM", 2, 3);
  cout << "in main" << endl;
  hash.create_from_csv("datos_c.csv");
  cout << "Insertion to chain bucket " << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search in chained" << endl;
  string record_found = hash.search(1);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  return 0;
}
