#ifndef HASH_H_
#define HASH_H_

#include "bucket.h"
#include <bitset>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

// Factor de bloque
const int Default_F = 3;
// Cantidad de digitos en el hash index (Profundidad global)
const int Default_D = 3;

// structure of bucket
// code, size, local depth, pointer, values
// string, int, int, TV * size, int

// Problems:
// TODO
// - Regarding key, assume it's first?
// - Parse for insertion so that data is separated with something, like a comma,
// to give the correct size to the fields in a new string

template <typename TK> class ExtendibleHashingFile {
private:
  string index_name;
  string data_name;
  string header_name;
  // The factor de bloque and profundidad global could be accesses with a field
  // in place of reading the file each time
  int factor;
  int depth;
  // helper functions
  void create_bucket(string code, int size, int ld, int pointer,
                     string records) {
    ofstream data_file(data_name, ios::app | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    //  strncpy copies the values filling from left to right. If there isn't
    //  enoug on the string to fill the char array, it's left empty. Not a
    //  problem because we have the local depth to slice what is useful
    char c_code[depth];
    strncpy(c_code, code.c_str(), depth);
    data_file.write(c_code, sizeof(char) * depth);

    // inserting size
    data_file.write((char *)&size, sizeof(int));

    // Inserting local depth
    data_file.write((char *)&ld, sizeof(int));

    // Inserting pointer
    data_file.write((char *)&pointer, sizeof(int));

    // The space not used is filled with weird things after nth value of string
    data_file.write(records.c_str(),
                    sizeof(char) * this->get_record_size() * factor);

    data_file.close();
  }

  int get_header_name_size() {
    ifstream file(header_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  int get_bucket_size() {
    return this->depth * sizeof(char) + sizeof(int) * 3 +
           this->get_record_size() * this->factor;
  }

  void overwrite_bucket(Bucket *bucket, int pos_bucket) {
    ofstream data_file(data_name, ios::in | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekp(pos_bucket * this->get_bucket_size(), ios::cur);

    data_file.write(bucket->get_code().c_str(), sizeof(char) * depth);

    // inserting size
    int temp = bucket->get_size();
    data_file.write((char *)&temp, sizeof(int));

    // Inserting local depth
    temp = bucket->get_local();
    data_file.write((char *)&temp, sizeof(int));

    // Inserting pointer
    temp = bucket->get_pointer();
    data_file.write((char *)&temp, sizeof(int));

    // // The space not used is filled with weird things after th value of
    // string
    data_file.write(bucket->get_all_records().c_str(),
                    sizeof(char) * this->get_record_size() * factor);

    data_file.close();
  }

  int get_field_size(int pos) {
    ifstream file(header_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.seekg(sizeof(int) + pos * (get_header_name_size() + sizeof(int)) +
                   get_header_name_size(),
               ios::beg); // fixed length record
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  void overwrite_pointer(int pos, int new_pointer) {
    ofstream index_file(index_name, ios::out | ios::binary);
    if (!index_file.is_open())
      throw("No se pudo abrir el archivo");

    index_file.seekp(sizeof(int) * 2 + pos * sizeof(int),
                     ios::beg); // fixed length record
    index_file.write((char *)&new_pointer, sizeof(int));
  }

  // Helper function to obtain the pointer associated to a position
  int get_index_pointer(int pos) {
    ifstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.seekg(sizeof(int) * 2 + pos * sizeof(int),
               ios::beg); // fixed length record
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  int get_pointer_key(TK key) {
    string binary_code = hash_func(key);
    int left = 0, right = pow(2, this->depth);
    for (auto iter = binary_code.begin(); iter != binary_code.end(); iter++) {
      int mid = left + (right - left) / 2;
      if (*iter == '0') {
        right = mid;
      } else {
        left = mid;
      }
    }
    return left;
  }

  int get_number_fields() {
    ifstream file(this->header_name, ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(0, ios::end);         // ubicar cursos al final del archivo
    long total_bytes = file.tellg(); // cantidad de bytes del archivo
    total_bytes -= sizeof(int);
    file.close();
    return total_bytes / (get_header_name_size() + sizeof(int));
  }

  int get_record_size() {
    int total = 0;
    for (int i = 0; i < get_number_fields(); i++) {
      total += get_field_size(i);
    }
    return total;
  }
  string hash_func(TK key) { return bitset<this->depth>(key).to_string(); }

  // Helper function to obtain bucket size
  Bucket *get_bucket(int pos) {
    ifstream data_file(this->data_name, ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekg(pos * get_bucket_size());

    char *c_code = new char[depth];
    data_file.read(c_code, sizeof(char) * depth);

    int size_bucket;
    data_file.read((char *)&size_bucket, sizeof(int));

    int local_depth;
    data_file.read((char *)&local_depth, sizeof(int));

    int pointer;
    data_file.read((char *)&pointer, sizeof(int));

    char *records = new char[this->record_size * this->factor];
    data_file.read(records, sizeof(char) * this->record_size * this->factor);

    Bucket *bucket =
        new Bucket(c_code, size_bucket, local_depth, pointer, records);

    data_file.close();
    return bucket;
  }

  void create_directory() {
    ofstream file(index_name);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    // The headers
    // First the factor, then the depth
    file.write((char *)&this->factor, sizeof(int));
    file.write((char *)&this->depth, sizeof(int));

    // Puts all codes with their associated pointers
    // We start with 2 buckets
    for (int i = 0; i < pow(2, this->depth); i++) {
      string binary_code = hash_func(i);
      int pos;
      if (*(binary_code.end() - 1) == '0') {
        pos = 0;
      } else {
        pos = 1;
      }
      // To save the binary code might not be neccessary because in the search
      // it can be done internally because it's sorted
      //
      // file.write((char *)&binary_code, binary_code.size());
      file.write((char *)&pos, sizeof(int));
    }
    create_bucket("0", 1, "");
    create_bucket("1", 1, "");
  }

  int get_factor() {
    ifstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  int get_depth() {
    ifstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    file.seekg(sizeof(int),
               ios::beg); // fixed length record

    int tmp;
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  string separate_record(string record) {
    // TODO
    // Consider the record given is separated by commas to indicate fields.
    // Return record but with the field size of the header size
  }

  int get_number_buckets() {
    ifstream file(this->data_name, ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(0, ios::end);         // ubicar cursos al final del archivo
    long total_bytes = file.tellg(); // cantidad de bytes del archivo
    file.close();
    return total_bytes / this->get_bucket_size();
  }

  void redirect_headers(int new_pointer, string code) {
    int ld = code.size();
    int start = stoi(code, nullptr, 2);
    while (start < pow(2, this->depth)) {
      this->overwrite_pointer(start, new_pointer);
      start += pow(2, ld);
    }
    // TODO
    // Test
  }

public:
  ExtendibleHashingFile(string file_name) {
    this->index_name = file_name + "_index";
    this->data_name = file_name + "_data";
    this->header_name = file_name + "_header";
    this->factor = this->get_factor();
    this->depth = this->get_depth();
  }
  ExtendibleHashingFile(string file_name, int f = Default_F,
                        int d = Default_D) {
    this->index_name = file_name + "_index";
    this->data_name = file_name + "_data";
    this->header_name = file_name + "_header";
    this->factor = f;
    this->depth = d;
    create_directory();
  }

  string search(TK key) {
    // TK is a template parameter but it is treated internally as a string.
    // Might be better to receive a string directly

    // 1. Search by divide and conquer the position for the given key
    int pos = get_pointer_key(key);
    // 2. Get pointer of key
    int pointer = get_index_pointer(pos);
    // 3. Loop over the chain of bucket until the end is reached or the key is
    // found
    Bucket *bucket = get_bucket(pointer);
    while (true) {
      string current_key = "";
      string record = "";
      // 4. Go to current bucket to search for key over all the records
      for (int i = 0; i < bucket->get_size(); i++) {
        record = bucket->get_record(i, this->get_record_size());
        // Remember, we are assuming that the key is the first field
        current_key = record.substr(0, this->get_field_size(0));
        if (current_key == key) {
          return record;
        }
      }

      if (bucket->get_pointer() == -1) {
        return "Key not found";
      } else {
        int pointer = bucket->get_pointer();
        delete bucket;
        bucket = this->get_bucket(pointer);
      }
    }

    // TODO
    // Test
  }

  vector<string> range_search(TK start_key, TK end_key) {
    vector<string> in_range;
    for (TK i = start_key; i < end_key; i++) {
      in_range.push_back(this->search(i));
    }
    return in_range;
  }

  bool insert(string record) {
    // TODO
  }
    // 1. Find bucket
    string key = record.substr(0, this->get_field_size(0));
    int pos_header = get_pointer_key(key);
    int pointer = get_index_pointer(pos_header);
    Bucket *bucket = get_bucket(pointer);
    // 2. Check if bucket has space
    if (bucket->get_size() < this->factor) {
      // 2.1 If it does, just insert
      string records = bucket->get_all_records();
      records += separate_record(record);
      bucket->change_records(records, this->get_record_size());
      this->overwrite_bucket(bucket, pointer);
    } else {
      if (bucket->get_local() < this->depth) {
        // 2.2 Divide elements of the bucket.
        string records_0 = "";
        string records_1 = "";

        if (key[this->depth - bucket->get_local() - 1] == "0") {
          records_0 += record;
        } else {
          records_1 += record;
        }

        for (int i = 0; i < bucket->get_size(); i++) {
          string current_record =
              bucket->get_record(i, this->get_record_size());
          key = current_record.substr(0, this->get_field_size(0));
          string binary_key = this->hash_func(key);
          if (binary_key[this->depth - bucket->get_local() - 1] == "0") {
            records_0 += current_record;
          } else {
            records_1 += current_record;
          }
        }

        // Check what happens if it doesn't work and the amount of records keeps
        // surpasing the factor
        // TODO
        this->create_bucket("1" + bucket->get_code(),
                            records_1.size() / this->get_record_size(),
                            bucket->get_local() + 1, -1, records_1);

        // Change pointers of headers.
        redirect_headers(this->get_number_buckets() - 1,
                         "1" + bucket->get_code());
        // Change current bucket to have "0"
        bucket->change_records(records_0, this->get_record_size());
        bucket->change_size(records_0.size() / this->get_record_size());
        bucket->change_code("0" + bucket->get_code(), this->depth);
        bucket->change_local(bucket->get_local() + 1);
        // Pointer doesn't needs to be changed
        this->overwrite_bucket(bucket, pointer);

      } else {
        // 2.3 If dividing isn't possible, change pointer of bucket to a new
        // bucket and insert there
        // Travel to end of chain
        while (true) {
          if (bucket->get_size() < this->factor) {
            string records = bucket->get_all_records();
            records += separate_record(record);
            bucket->change_records(records, this->get_record_size());
            this->overwrite_bucket(bucket, pointer);
            break;
          } else if (bucket->get_pointer() == -1) {
            this->create_bucket(bucket->get_code(), 1, bucket->get_local(), -1,
                                separate_record(record));
            this->overwrite_bucket(bucket, this->get_number_buckets() - 1);
            break;
          } else {
            int pointer = bucket->get_pointer();
            delete bucket;
            bucket = this->get_bucket(pointer);
          }
        }
      }
    }
    delete bucket;
    // TODO
    // Test
  }

  bool remove(TK key) {
    // TODO
  }

  bool create_from_csv(string csvfile) {
    ifstream file(csvfile);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    vector<vector<string>> dataframe;

    string line;
    getline(file, line);
    istringstream ss(line);
    vector<string> headers;
    int longest_header = 0;
    for (string value; getline(ss, value, ',');) {
      headers.push_back(value);
      if (longest_header < value.size()) {
        longest_header = value.size();
      }
    }
    vector<int> sizes(headers.size(), 0);

    // Theres is an extra character inserted after the first field. Research
    // why. Might be like som kind of end of something
    // 1. Read from csv file
    for (; getline(file, line);) {
      istringstream ss(line);
      vector<string> row;
      if (!dataframe.empty()) {
        row.reserve(dataframe.front().size());
      }

      int i = 0;
      for (string value; getline(ss, value, ',');) {
        row.push_back(value);
        if (sizes[i] < value.size()) {
          sizes[i] = value.size();
        }
        i += 1;
        // ss.seekg(1, ios::cur);
      }
      dataframe.push_back(row);
    }

    file.close();

    // 2. Create headers file
    ofstream header_file(header_name, ios::out | ios::binary | ios::trunc);
    if (!header_file.is_open())
      throw("No se pudo abrir el archivo");
    header_file.write((char *)&longest_header, sizeof(int));
    for (int i = 0; i < headers.size(); i++) {
      header_file.write((char *)&headers[i], sizeof(char) * longest_header);
      header_file.write((char *)&sizes[i], sizeof(int));
    }

    // 3. Create data file
    for (auto iter = dataframe.begin(); iter != dataframe.end(); iter++) {
      string record = "";
      for (auto field_iter = iter->begin(); field_iter != iter->end();
           field_iter++) {
        record += *field_iter;
      }
      this->insert(record);
    }
  }
};

#endif // HASH_H_
