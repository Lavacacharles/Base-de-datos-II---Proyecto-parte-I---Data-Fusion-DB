#ifndef BUCKET_H_
#define BUCKET_H_

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Bucket {
private:
  char *code;
  int size;
  int local;
  long int pointer;
  char *records;

public:
  Bucket() {}
  Bucket(char *code, int size, int local, long int pointer, char *records) {
    this->code = code;
    this->size = size;
    this->local = local;
    this->pointer = pointer;
    this->records = records;
  }
  ~Bucket() {
    delete records;
    delete code;
  }
  string get_code() {
    string temp(code);
    return temp;
  }
  int get_size() { return this->size; }
  long int get_pointer() { return this->pointer; }
  int get_local() { return this->local; }
  string get_record(int pos, int record_size) {
    string temp(records);
    return temp.substr(pos * record_size, record_size);
  }
  string get_all_records() {
    string temp(records);
    return temp;
  }
  void change_size(int new_sz) { this->size = new_sz; }
  void change_local(long new_ld) { this->local = new_ld; }
  void change_pointer(long int new_p) { this->pointer = new_p; }
  void change_records(string new_r, int record_size, int factor) {
    delete records;
    records = new char[record_size * factor];
    strncpy(this->records, new_r.c_str(), record_size * factor);
  }
  void change_code(string new_c, int depth) {
    delete code;
    code = new char[depth];
    strncpy(this->code, new_c.c_str(), depth);
  }
};

#endif // BUCKET_H_
