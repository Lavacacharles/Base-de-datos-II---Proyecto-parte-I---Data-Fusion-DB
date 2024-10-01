#include "bucket.h"
#include "hash.h"
#include <chrono>
#include <cmath>

using namespace std;

float to_seconds(int milseconds) {
  float temp = milseconds / pow(10, 6);
  return temp;
}

void test_general() {
  ExtendibleHashingFile hash("students");
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

void test_small() {
  ExtendibleHashingFile hash("small", 3, 3, "datos_small.csv");
  cout << "in main" << endl;
  cout << "-----------------------------------------------------------" << endl;
  cout << "Read from small csv" << endl;
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

void test_created(string file_name) {
  ExtendibleHashingFile hash(file_name);
  cout << "-----------------------------------------------------------" << endl;
  cout << "Removing to existing hash" << endl;
  hash.remove(1);
  cout << "-----------------------------------------------------------" << endl;
  cout << "Insertion to existing hash" << endl;
  hash.add("1,http://atualizacao.novoatendimentocaixa.ml/"
           "sinbc,Doesn't exists,phishing,48");
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search" << endl;
  string record_found = hash.find(1);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
}

void test_csv(string csv_file, int factor, int depth) {
  std::chrono::steady_clock::time_point begin =
      std::chrono::steady_clock::now();

  ExtendibleHashingFile hash("output", factor, depth, csv_file);
  std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

  auto diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
                  .count();

  cout << "Build table" << endl;
  std::cout << "Time difference microseconds = " << diff << std::endl;
  std::cout << "Time difference seconds = " << to_seconds(diff) << std::endl;

  begin = std::chrono::steady_clock::now();
  // hash.find(8) ;
  cout << hash.find(8) << endl;
  end = std::chrono::steady_clock::now();
  diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
             .count();
  cout << "Search" << endl;
  std::cout << "Time difference microseconds = " << diff << std::endl;
  std::cout << "Time difference seconds = " << to_seconds(diff) << std::endl;

  begin = std::chrono::steady_clock::now();
  hash.remove(7);
  end = std::chrono::steady_clock::now();

  diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
             .count();
  cout << "Remove" << endl;
  std::cout << "Time difference microseconds = " << diff << std::endl;
  std::cout << "Time difference seconds = " << to_seconds(diff) << std::endl;

  begin = std::chrono::steady_clock::now();

  hash.add("7,http://atualizacao.novoatendimentocaixa.ml/"
           "sinbc,Phishing.Database,phishing,48");

  end = std::chrono::steady_clock::now();

  diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
             .count();
  cout << "Insert" << endl;
  std::cout << "Time difference microseconds = " << diff << std::endl;
  std::cout << "Time difference seconds = " << to_seconds(diff) << std::endl;

  begin = std::chrono::steady_clock::now();
  hash.range_search(2, 9232);
  end = std::chrono::steady_clock::now();

  diff = std::chrono::duration_cast<std::chrono::microseconds>(end - begin)
             .count();
  cout << "Range search" << endl;
  std::cout << "Time difference microseconds = " << diff << std::endl;
  std::cout << "Time difference seconds = " << to_seconds(diff) << std::endl;
}

void test_times(int factor, int depth) {
  cout << "Factor: " << factor << endl;
  cout << "Depth: " << depth << endl;
  cout << "10 records" << endl;
  test_csv("datasets/df_10.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "100 records" << endl;
  test_csv("datasets/df_100.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "1k records" << endl;
  test_csv("datasets/df_1k.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "10k records" << endl;
  test_csv("datasets/df_10k.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "50k records" << endl;
  test_csv("datasets/df_50k.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "100k records" << endl;
  test_csv("datasets/df_100k.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;

  cout << "1m records" << endl;
  test_csv("datasets/df_1m.csv", factor, depth);
  cout << "-----------------------------------------------------------" << endl;
}

void test_general_created() {
  test_general();
  ExtendibleHashingFile hash("students");
  cout << "-----------------------------------------------------------" << endl;
  cout << "Removing to existing hash" << endl;
  hash.remove(22);
  cout << "-----------------------------------------------------------" << endl;
  cout << "Insertion to existing hash" << endl;
  hash.add("18,Jose,Alonso,6");
  cout << "-----------------------------------------------------------" << endl;
  cout << "Search" << endl;
  string record_found = hash.find(18);
  cout << record_found << endl;
  cout << "-----------------------------------------------------------" << endl;
}

int main() {
  // test_m();
  // test_chain();
  test_times(8, 16);

  // test_csv("df_10.csv", 8, 16);
  return 0;
}
