#ifndef HASH_H_
#define HASH_H_

#include "bucket.h"
#include "parent.h"
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
// - Parse for insertion so that data is separated with something, like a comma,
// to give the correct size to the fields in a new string

// The key needs to be a int because of the hashing function. Before bitset was
// used and it works with string, but it needs a constant as limit. External
// package booster can be used to fix this, but is not implemetned
class ExtendibleHashingFile : public FileParent<int> {
private:
  string index_name;
  // The factor de bloque and profundidad global could be accesses with a field
  // in place of reading the file each time
  int factor;
  int depth;

  // helper functions
  int create_bucket(string code, int size, int ld, long int pointer,
                    string records) {
    // there is a free bucket, so lets use it
    long int pos_bucket = 0;
    if (this->get_top_free_list() == -1) {
      ofstream data_file(this->data_name, ios::app | ios::binary);
      if (!data_file.is_open())
        throw("No se pudo abrir el archivo");

      //  strncpy copies the values filling from left to right. If there isn't
      //  enoug on the string to fill the char array, it's left empty. Not a
      //  problem because we have the local depth to slice what is useful
      // char c_code[depth];
      // strncpy(c_code, code.c_str(), depth);
      // data_file.write(c_code, sizeof(char) * depth);
      data_file.write(code.c_str(), sizeof(char) * depth);

      // inserting size
      data_file.write(reinterpret_cast<const char *>(&size), sizeof(int));

      // Inserting local depth
      data_file.write(reinterpret_cast<const char *>(&ld), sizeof(int));

      // Inserting pointer
      data_file.write(reinterpret_cast<const char *>(&pointer),
                      sizeof(long int));

      // The space not used is filled with weird things after nth value of
      // string
      data_file.write(records.c_str(),
                      sizeof(char) * this->record_size * factor);

      data_file.close();

      pos_bucket = this->get_number_buckets() - 1;
    } else {
      // 1. Get pointer that is at the top of the free list
      long int top = this->get_top_free_list();
      // 2. Get pointer of aimed bucket to pass it into the top
      int new_top = this->get_pointer_free_list(top);
      // 3. Overwrite in free space
      // Could be optimized with overwrite at the expense of generalizing
      // somethings
      ofstream data_file(data_name, ios::in | ios::binary);
      if (!data_file.is_open())
        throw("No se pudo abrir el archivo");

      data_file.seekp(top * this->get_bucket_size(), ios::cur);

      data_file.write(code.c_str(), sizeof(char) * depth);

      // inserting size
      data_file.write(reinterpret_cast<const char *>(&size), sizeof(int));

      // Inserting local depth
      data_file.write(reinterpret_cast<const char *>(&ld), sizeof(int));

      // Inserting pointer
      data_file.write(reinterpret_cast<const char *>(&pointer),
                      sizeof(long int));

      // The space not used is filled with weird things after nth value of
      // string
      data_file.write(records.c_str(),
                      sizeof(char) * this->record_size * factor);

      data_file.close();

      // 4. Redirect top
      this->change_top_free_list(new_top);

      pos_bucket = top;
      // TODO
      // Test
    }
    return pos_bucket;
  }

  int get_bucket_size() {
    return this->depth * sizeof(char) + sizeof(int) * 2 + sizeof(long int) +
           this->record_size * this->factor;
  }

  void overwrite_bucket(Bucket *bucket, long int pos_bucket) {
    ofstream data_file(data_name, ios::in | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekp(pos_bucket * this->get_bucket_size(), ios::cur);

    data_file.write(bucket->get_code().c_str(), sizeof(char) * depth);

    // inserting size
    int size = bucket->get_size();
    data_file.write(reinterpret_cast<const char *>(&size), sizeof(int));

    // Inserting local depth
    int ld = bucket->get_local();
    data_file.write(reinterpret_cast<const char *>(&ld), sizeof(int));

    // Inserting pointer
    long int pointer = bucket->get_pointer();
    data_file.write(reinterpret_cast<const char *>(&pointer), sizeof(long int));

    // // The space not used is filled with weird things after th value of
    // string
    data_file.write(bucket->get_all_records().c_str(),
                    sizeof(char) * this->record_size * factor);

    data_file.close();
  }

  void overwrite_pointer(long int pos, long int new_pointer) {
    ofstream index_file(index_name, ios::in | ios::binary);
    if (!index_file.is_open())
      throw("No se pudo abrir el archivo");

    index_file.seekp(sizeof(int) * 2 + sizeof(long int) +
                         pos * sizeof(long int),
                     ios::beg); // fixed length record
    index_file.write((char *)&new_pointer, sizeof(long int));
  }

  // Helper function to obtain the pointer associated to a position
  int get_index_pointer(int pos) {
    ifstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.seekg(sizeof(int) * 2 + sizeof(long int) + pos * sizeof(long int),
               ios::beg); // fixed length record
    file.read((char *)&tmp, sizeof(long int));
    return tmp;
  }

  int get_pointer_key(int key) {
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

  string hash_func(int key) {
    string binary = "";
    for (int i = depth - 1; i >= 0; --i) {
      binary += (key & (1 << i)) ? '1' : '0';
    }
    return binary;
  }

  // Helper function to obtain bucket size
  Bucket *get_bucket(long int pos) {
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

    long int pointer;
    data_file.read((char *)&pointer, sizeof(long int));

    char *records = new char[this->record_size * this->factor];
    data_file.read(records, sizeof(char) * this->record_size * this->factor);

    Bucket *bucket =
        new Bucket(c_code, size_bucket, local_depth, pointer, records);

    data_file.close();
    return bucket;
  }

  void create_index() {
    ofstream file(index_name, ios::binary | ios::trunc);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    // The headers
    // First the factor, then the depth, finally the pointer to the free bucket
    file.write((char *)&this->factor, sizeof(int));
    file.write((char *)&this->depth, sizeof(int));
    long int temp = -1;
    file.write((char *)&temp, sizeof(long int));

    // Puts all codes with their associated pointers
    // We start with 2 buckets
    for (int i = 0; i < pow(2, this->depth); i++) {
      string binary_code = hash_func(i);
      long int pos;
      if (*(binary_code.end() - 1) == '0') {
        pos = 0;
      } else {
        pos = 1;
      }
      // To save the binary code might not be neccessary because in the search
      // it can be done internally because it's sorted
      //
      // file.write((char *)&binary_code, binary_code.size());
      file.write((char *)&pos, sizeof(long int));
    }
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

  void free_bucket(int pos_bucket, int pointer) {
    ofstream data_file(data_name, ios::in | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekp(pos_bucket * this->get_bucket_size(), ios::cur);

    // inserting pointer
    data_file.write(reinterpret_cast<const char *>(&pointer), sizeof(int));

    data_file.close();
  }

  int get_pointer_free_list(int pos) {
    ifstream file(data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    file.seekg(pos * get_bucket_size(),
               ios::beg); // fixed length record

    int tmp;
    file.read((char *)&tmp, sizeof(int));
    return tmp;
  }

  long int get_top_free_list() {
    ifstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    file.seekg(sizeof(int) * 2,
               ios::beg); // fixed length record

    long int tmp;
    file.read((char *)&tmp, sizeof(long int));
    return tmp;
  }

  void change_top_free_list(long int new_top) {
    ofstream file(index_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    file.seekp(sizeof(int) * 2,
               ios::beg); // fixed length record

    file.write((char *)&new_top, sizeof(long int));
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

  void redirect_index(int new_pointer, string code) {
    int ld = code.size();
    int start = stoi(code, nullptr, 2);
    while (start < pow(2, this->depth)) {
      this->overwrite_pointer(start, new_pointer);
      start += pow(2, ld);
    }
  }

public:
  ExtendibleHashingFile(string file_name) : FileParent(file_name) {
    this->index_name = file_name + "_index";
    this->factor = this->get_factor();
    this->depth = this->get_depth();
  }
  ExtendibleHashingFile(string file_name, int f, int d, string csv_file)
      : FileParent(file_name, csv_file) {
    this->index_name = file_name + "_index";
    this->factor = f;
    this->depth = d;

    vector<vector<string>> dataframe;
    vector<int> sizes;

    this->create_from_csv(csv_file, dataframe, sizes, 0);

    // 3. Create index from the fields
    this->create_index();

    // clean file
    ofstream data_file(this->data_name, ios::out | ios::binary | ios::trunc);
    data_file.close();
    this->record_size = this->record_size;
    create_bucket("0", 0, 1, -1, "");
    create_bucket("1", 0, 1, -1, "");

    // 4. Create data file
    for (auto iter = dataframe.begin(); iter != dataframe.end(); iter++) {
      string record = "";
      int i = 0;
      for (auto field_iter = iter->begin(); field_iter != iter->end();
           field_iter++) {

        if ((*field_iter).size() >= sizes[i]) {
          record += (*field_iter).substr(0, sizes[i]);
        } else {
          record +=
              (*field_iter) + string(sizes[i] - (*field_iter).size(), ' ');
        }
        i++;
      }
      this->insert_parsed(record);
    }
  }
  virtual ~ExtendibleHashingFile() {}

  string find(int key) {
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
        record = bucket->get_record(i, this->record_size);
        // Remember, we are assuming that the key is the first field
        current_key = record.substr(0, this->key_size);
        if (stoi(current_key) == key) {
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
  }

  vector<string> range_search(int start_key, int end_key) {
    vector<string> in_range;
    for (int i = start_key; i < end_key; i++) {
      in_range.push_back(this->find(i));
    }
    return in_range;
  }

  bool add(string record) {
    return insert_parsed(this->separate_record(record));
  }

  bool insert_parsed(string record) {
    // 1. Find bucket
    // cout << "---------------------------------------------------" << endl;
    string temp_key = record.substr(0, this->key_size);
    int key = stoi(record.substr(0, this->key_size));
    // cout << "key: " << key << endl;
    // cout << "binary key: " << hash_func(key) << endl;
    int pos_header = get_pointer_key(key);
    // cout << "pos_header: " << pos_header << endl;
    int pointer = get_index_pointer(pos_header);
    // cout << "pointer: " << pointer << endl;
    Bucket *bucket = get_bucket(pointer);
    // cout << bucket->get_code() << endl;
    // cout << bucket->get_size() << endl;
    // cout << bucket->get_local() << endl;
    // cout << bucket->get_pointer() << endl;
    // cout << bucket->get_all_records() << endl;
    // cout << "---------------------------------------------------" << endl;
    // 2. Check if bucket has space
    if (bucket->get_size() < this->factor) {
      // 2.1 If it does, just insert
      // cout << "normal insert with space" << endl;
      string all_records = bucket->get_all_records();
      all_records = all_records + record;
      bucket->change_records(all_records, this->record_size, this->factor);
      bucket->change_size(bucket->get_size() + 1);
      this->overwrite_bucket(bucket, pointer);
    } else {
      if (bucket->get_local() < this->depth) {
        // cout << "Partition" << endl;

        // 2.2 Divide elements of the bucket.
        string records_0 = "";
        string records_1 = "";

        string binary_key = this->hash_func(key);
        if (binary_key[this->depth - bucket->get_local() - 1] == '0') {
          records_0 += record;
        } else {
          records_1 += record;
        }

        // cout << "Bucket size:" << bucket->get_size() << endl;
        for (int i = 0; i < bucket->get_size(); i++) {
          string current_record = bucket->get_record(i, this->record_size);
          // cout << current_record << endl;
          // cout << "Key in string:" << current_record.substr(0,
          // this->key_size) << endl;
          key = stoi(current_record.substr(0, this->key_size));
          string binary_key = this->hash_func(key);
          // cout << "Binary key:" << binary_key << endl;
          if (binary_key[this->depth - bucket->get_local() - 1] == '0') {
            records_0 += current_record;
          } else {
            records_1 += current_record;
          }
        }

        if ((records_0.size() / this->record_size > this->factor)) {
          // Redo the breaking for record 0 by recursion
          int bucket_pos = this->create_bucket(
              "1" + bucket->get_code(), records_1.size() / this->record_size,
              bucket->get_local() + 1, -1, records_1);

          // Change pointers of headers.
          redirect_index(bucket_pos, "1" + bucket->get_code());

          // Records are the same
          bucket->change_code("0" + bucket->get_code(), this->depth);
          bucket->change_local(bucket->get_local() + 1);
          // Pointer doesn't needs to be changed
          this->overwrite_bucket(bucket, pointer);
          // Recursive call to delegate problem
          return this->insert_parsed(record);
        } else if ((records_1.size() / this->record_size > this->factor)) {
          // Redo the breaking for record 1 by recursion
          int bucket_pos = this->create_bucket(
              "1" + bucket->get_code(), bucket->get_size(),
              bucket->get_local() + 1, -1, bucket->get_all_records());

          // Change pointers of headers.
          redirect_index(bucket_pos, "1" + bucket->get_code());

          // Records are the same
          bucket->change_code("0" + bucket->get_code(), this->depth);
          bucket->change_size(records_0.size() / this->record_size);
          bucket->change_records(records_0, this->record_size, this->factor);
          bucket->change_local(bucket->get_local() + 1);
          this->overwrite_bucket(bucket, pointer);

          // Recursive call to delegate problem
          return this->insert_parsed(record);

        } else {
          int bucket_pos = this->create_bucket(
              "1" + bucket->get_code(), records_1.size() / this->record_size,
              bucket->get_local() + 1, -1, records_1);

          // Change pointers of headers.
          redirect_index(bucket_pos, "1" + bucket->get_code());
          // Change current bucket to have "0"
          bucket->change_records(records_0, this->record_size, this->factor);
          bucket->change_size(records_0.size() / this->record_size);
          bucket->change_code("0" + bucket->get_code(), this->depth);
          bucket->change_local(bucket->get_local() + 1);
          // Pointer doesn't needs to be changed
          this->overwrite_bucket(bucket, pointer);
        }
      } else {
        // 2.3 If dividing isn't possible, change pointer of bucket to a new
        // bucket and insert there
        // Travel to end of chain
        // cout << "Chain" << endl;
        while (true) {
          if (bucket->get_size() < this->factor) {
            // cout << "Chained with space: " << bucket->get_pointer() << endl;
            string records = bucket->get_all_records();
            records += record;
            bucket->change_size(records.size() / this->record_size);
            bucket->change_records(records, this->record_size, this->factor);
            this->overwrite_bucket(bucket, pointer);
            break;
          } else if (bucket->get_pointer() == -1) {
            // cout << "There is no chained: " << bucket->get_pointer() << endl;
            long int bucket_pos = this->create_bucket(
                bucket->get_code(), 1, bucket->get_local(), -1, record);
            bucket->change_pointer(bucket_pos);
            this->overwrite_bucket(bucket, pointer);
            break;
          } else {
            // cout << "Move to next bucket: " << bucket->get_pointer() << endl;
            pointer = bucket->get_pointer();
            delete bucket;
            bucket = this->get_bucket(pointer);
          }
        }
      }
    }
    delete bucket;
    return true;
  }

  bool remove(int key) {
    // If record is deleted in bucket, use move last.
    // If bucket is left empty, use free list to use it later. If possible,
    // combine the empty bucket with it's predecesor

    // 1. Find bucket
    // cout << "---------------------------------------------------" << endl;
    int pos_header = get_pointer_key(key);
    // cout << "key: " << key << endl;
    // cout << "binary key: " << hash_func(key) << endl;
    int pointer = get_index_pointer(pos_header);
    // cout << "pointer: " << pointer << endl;
    Bucket *bucket = get_bucket(pointer);
    // cout << "---------------------------------------------------" << endl;
    // 2. Check if record is in bucket
    bool found = false;
    string prev_records = "";
    while (true) {
      // cout << "current bucket: " << endl;
      // cout << bucket->get_code() << endl;
      // cout << bucket->get_size() << endl;
      // cout << bucket->get_local() << endl;
      // cout << bucket->get_pointer() << endl;
      // cout << "---------------------------------------------------" << endl;
      prev_records = "";
      for (int i = 0; i < bucket->get_size(); i++) {
        string record = bucket->get_record(i, this->record_size);
        string current_key = record.substr(0, this->key_size);
        if (stoi(current_key) == key) {
          found = true;
        } else {
          prev_records += record;
        }
      }
      if (found == true) {
        // cout << "Found key" << endl;
        break;
      } else {
        // Not found, so search in next bucket in chain if it exists
        if (bucket->get_pointer() == -1) {
          // Not found and no next bucket, so exit
          break;
        } else {
          pointer = bucket->get_pointer();
          delete bucket;
          bucket = this->get_bucket(pointer);
        }
      }
    }
    // Use move to last to move the last record in case there is a next
    // pointer
    // If this activates, it means that the pointed bucket is last, so
    // there is no momvement to next bucket However, we must check if
    // it's empty and the "sibling", has space to combine them

    if (found == false) {
      return found;
    }

    if (bucket->get_pointer() == -1) {
      // Could be changed to be activated at half capacity or something like
      // that. Would require changes in the merging
      if (bucket->get_size() == 1 && get_index_pointer(pos_header) == pointer) {
        // must check if can be combined with sibling in the case we are
        // first.
        // Find sibling
        string binary_sibling = (bucket->get_code()[0] == '0' ? "1" : "0");
        binary_sibling +=
            bucket->get_code().substr(1, bucket->get_code().size() - 1);
        // Find bucket of sibling
        int sibling_pointer =
            get_index_pointer(stoi(binary_sibling, nullptr, 2));
        Bucket *sibling_bucket = get_bucket(sibling_pointer);
        if (sibling_bucket->get_size() <= this->factor / 2) {
          // Merge!
          binary_sibling = binary_sibling.substr(1, binary_sibling.size() - 1);
          sibling_bucket->change_code(binary_sibling, this->depth);
          sibling_bucket->change_local(sibling_bucket->get_local() - 1);
          this->overwrite_bucket(sibling_bucket, sibling_pointer);
          // Records should also be changed if the main bucket isn't empty

          // change free list
          long int top = this->get_top_free_list();
          this->free_bucket(pointer, top);
          this->change_top_free_list(pointer);

          // Change index
          redirect_index(sibling_pointer, binary_sibling);
        } else {
          // if combining isn't possible, just ignore the empty bucket
          delete sibling_bucket;
        }
      } else if (bucket->get_size() == 1) {
        // Are inside a chained and it's left as empty
        // Remove pointer of bucket
        bucket->change_pointer(-1);
        // Change free list
        long int top = this->get_top_free_list();
        this->free_bucket(pointer, top);
        this->change_top_free_list(pointer);
      } else {
        // Use move last in the current bucket
        // cout << "only modifies bucket with no chaining" << endl;
        bucket->change_size(bucket->get_size() - 1);
        bucket->change_records(prev_records, this->record_size, this->factor);
        overwrite_bucket(bucket, pointer);
      }
    } else {
      int last_pointer = bucket->get_pointer();
      Bucket *last_bucket = get_bucket(last_pointer);
      // Use move last with the last chained bucket
      // cout << "use move last with the last chained bucket" << endl;
      last_pointer = last_bucket->get_pointer();
      last_bucket = get_bucket(last_bucket->get_pointer());
      while (true) {
        if (last_bucket->get_pointer() == -1) {
          // last bucket in chain!
          // Done
          break;
        } else {
          // go to next bucket
          last_pointer = last_bucket->get_pointer();
          delete last_bucket;
          last_bucket = get_bucket(last_pointer);
        }
      }
      string last_records = "";
      int i = 0;
      for (; i < bucket->get_size() - 1; i++) {
        string record = bucket->get_record(i, this->record_size);
        last_records += record;
      }
      string last_record = bucket->get_record(i, this->record_size);
      prev_records += last_record;
      bucket->change_records(prev_records, this->record_size, this->factor);
      // Check if last_bucket is empty to free
      if (last_bucket->get_size() == 1) {
        // Remove pointer of bucket
        bucket->change_pointer(-1);
        // Change free list
        long int top = this->get_top_free_list();
        this->free_bucket(pointer, top);
        this->change_top_free_list(pointer);
      } else {
        last_bucket->change_size(last_bucket->get_size() - 1);
        last_bucket->change_records(last_records, this->record_size,
                                    this->factor);
        overwrite_bucket(last_bucket, last_pointer);
      }

      overwrite_bucket(bucket, pointer);

      delete last_bucket;
    }
    // TODO
    // Test
    delete bucket;
    return found;
  }
};

#endif // HASH_H_
