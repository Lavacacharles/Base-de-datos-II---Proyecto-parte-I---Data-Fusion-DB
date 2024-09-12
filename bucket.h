#ifndef BUCKET_H_
#define BUCKET_H_

#include <string>
#include <vector>

using namespace std;

class Bucket {
private:
  string code;
  int size;
  int local;
  int pointer;
  string records;

public:
  Bucket() {}
  Bucket(string code, int size, int local, int pointer, string records) {
    this->code = code;
    this->size = size;
    this->local = local;
    this->pointer = pointer;
    this->records = records;
  }
  int get_size() { return this->size; }
  int get_pointer() { return this->pointer; }
};

#endif // BUCKET_H_
