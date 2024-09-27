#include "bucket.h"
#include "hash.h"

using namespace std;

void test_general() {
  ExtendibleHashingFile hash("students", 3, 3, "datos_small.csv");
  cout << "-----------------------------------------------------------" << endl;
  // 1 == 1, 22 == 10110, 32 == 100000
  cout << "Read from csv" << endl;
  // Only base case was tested
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search" << endl;
  string record_found = hash.find(22);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Remove" << endl;
  hash.remove(22);
  cout << "-----------------------------------------------------------" << endl;
  // cout << "Insertion to fill bucket" << endl;
  // // 10 == 1010, 10 == 1001
  // hash.insert("10,Luisa,Martín,6");
  // hash.insert("9,Manuela,Hernández,1");
  // hash.insert("9,Manuela,Hernández,1");
  // cout << "-----------------------------------------------------------" <<
  // endl; cout << "Insertion to divide bucket " << endl;
  // // 8 == 1000, 14 == 1110
  // hash.insert("8,Rosario,Jiménez,4");
  // hash.insert("14,Sandra,Ruiz,6");
  // cout << "-----------------------------------------------------------" <<
  // endl;
}

void test_m() {
  ExtendibleHashingFile hash("studentsM", 3, 3, "datos_m.csv");
  cout << "in main" << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Read from moderate csv to have chaining" << endl;
}

void test_chain() {
  ExtendibleHashingFile hash("studentsM", 2, 3, "datos_c.csv");
  cout << "in main" << endl;
  cout << "Insertion to chain bucket " << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search in chained" << endl;
  string record_found = hash.find(1);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Remove to chained with space" << endl;
  hash.remove(1);
  cout << "-----------------------------------------------------------" << endl;
  cout << "Remove done so we search" << endl;
  record_found = hash.find(1);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Remove to chained with 1 element" << endl;
  hash.remove(25);
  cout << "-----------------------------------------------------------" << endl;
  cout << "Remove done so we search" << endl;
  record_found = hash.find(25);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Insertion to use freed bucket" << endl;
  hash.add("25,Javier,Cortés,1");
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search to check it works" << endl;
  record_found = hash.find(25);
  cout << record_found << endl;
}

int main() {
  test_chain();
  return 0;
}
