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
const int Default_F = 4;
// Cantidad de digitos en el hash index (Profundidad global)
const int Default_D = 32;

// structure of bucket
// code, size, local depth, pointer, values
// string, int, int, TV * size, int

// Problems:
// TODO
// - Save number of elements in header of bucket?
// - To generalize, use strings to save records inside.
// - Changing of fields of records not possible, because we don't know their
// sizes. Use separators to know.
// - Regarding key, assume it's first?

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
  void create_bucket(string code, int ld, string records) {
    ofstream file(this->data_name, ios::app | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    Bucket<this->bucketdepth, this->get_record_size(), this->factor> bucket(
        code, 0, ld, -1, records);

    file.write((char *)&bucket,
               sizeof(Bucket<this->bucketdepth, this->get_record_size(),
                             this->factor>)); // guardar en formato binario
    file.close();
    // test
    // TODO
  }
  int get_header_name_size() {
    ifstream file(header_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.read((char *)&tmp, sizeof(int));
    return tmp;
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

  void overwrite_pointer(int code, int new_pointer) {
    // TODO
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
  // Bucket get_bucket(int pos) {
  //   // TODO
  // }

public:
  ExtendibleHashingFile(string file_name, int f, int d) {
    this->index_name = file_name + "_index";
    this->data_name = file_name + "_data";
    this->header_name = file_name + "_header";
    this->factor = f;
    this->depth = d;
  }

  void create_directory() {
    ofstream file(index_name);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    // The headers
    // First the factor
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
  }

  string search(TK key) {
    // 1. Search by divide and conquer the position for the given key
    int pos = get_pointer_key(key);
    // 2. Get pointer of key
    int pointer = get_index_pointer(pos);
    // 3. Go to bucket of pointer to search for key over all the records
    // TODO
    // 4. If bucket has a next bucket, redo step 3
    // TODO
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
