#ifndef ENTRY_H_
#define ENTRY_H_

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Entry {
private:
  char *key;
  char *record;
  long int pointer;

public:
  Entry() {}
  Entry(char *key, char *record, long int pointer) {
    this->key = key;
    this->record = record;
    this->pointer = pointer;
  }
  ~Entry() {
    delete record;
    delete key;
  }
  string get_key() {
    string temp(key);
    return temp;
  }
  long int get_pointer() { return this->pointer; }
  string get_record() {
    string temp(record);
    return temp;
  }
  void change_pointer(long int new_p) { this->pointer = new_p; }
  void change_record(string new_r) {
    delete this->record;
    this->record = new char[new_r.size()];
    strncpy(this->record, new_r.c_str(), new_r.size());
  }
  void change_key(string new_k) {
    delete key;
    key = new char[new_k.size()];
    strncpy(this->key, new_k.c_str(), new_k.size());
  }
};

#endif // ENTRY_H_
