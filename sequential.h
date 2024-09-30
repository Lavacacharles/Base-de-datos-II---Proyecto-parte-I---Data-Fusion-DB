#ifndef HASH_H_
#define HASH_H_

#include "entry.h"
#include "parent.h"
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using namespace std;

// structure of bucket
// code, size, local depth, values, pointer
// string, int, int, TV * size, long int

// The key needs to be a int because of the hashing function. Before bitset was
// used and it works with string, but it needs a constant as limit. External
// package booster can be used to fix this, but is not implemented
template <class TK> class SequentialFile : public FileParent<TK> {
private:
  string temp_name;
  long int max_aux;
  long int separator;

  // helper functions
  int create_entry(long int pointer, string record, string key) {
    ofstream data_file(this->data_name, ios::app | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    // The space not used is filled with weird things after nth value of
    // string
    data_file.write(key.c_str(), sizeof(char) * this->key_size);
    data_file.write(record.c_str(), sizeof(char) * this->record_size);

    // Inserting pointer
    data_file.write(reinterpret_cast<const char *>(&pointer), sizeof(long int));

    data_file.close();
  }

  int get_entry_size() { return sizeof(long int) + this->record_size; }

  void overwrite_entry(Entry *entry, long int pos_entry) {
    ofstream data_file(this->data_name, ios::in | ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekp(sizeof(long int) * 3 + pos_entry * this->get_entry_size(),
                    ios::cur);

    data_file.write(entry->get_key().c_str(), this->key_size);

    data_file.write(entry->get_record().c_str(),
                    sizeof(char) * this->record_size);

    // Inserting pointer
    long int pointer = entry->get_pointer();
    data_file.write(reinterpret_cast<const char *>(&pointer), sizeof(long int));

    data_file.close();
  }

  // Helper function to obtain bucket size
  Entry *get_entry(long int pos) {
    ifstream data_file(this->data_name, ios::binary);
    if (!data_file.is_open())
      throw("No se pudo abrir el archivo");

    data_file.seekg(sizeof(long int) * 3 + pos * get_entry_size());

    char *key = new char[this->key_size];
    data_file.read(key, sizeof(char) * this->key_size);

    char *record = new char[this->record_size];
    data_file.read(record, sizeof(char) * this->record_size);

    long int pointer;
    data_file.read((char *)&pointer, sizeof(long int));

    Entry *entry = new Entry(key, record, pointer);

    data_file.close();
    return entry;
  }

  int get_top() {
    ifstream file(this->data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    long int tmp;
    file.read((char *)&tmp, sizeof(long int));
    return tmp;
  }

  int get_separator() {
    ifstream file(this->data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(sizeof(long int),
               ios::beg); // fixed length record

    long int tmp;
    file.read((char *)&tmp, sizeof(long int));
    return tmp;
  }

  int get_max_aux() {
    ifstream file(this->data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(sizeof(long int) * 2,
               ios::beg); // fixed length record

    long int tmp;
    file.read((char *)&tmp, sizeof(long int));
    return tmp;
  }

  void change_top(long int new_top) {
    ofstream file(this->data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.write((char *)&new_top, sizeof(long int));
  }

  void change_separator(long int new_separator) {
    ofstream file(this->data_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");
    file.seekp(sizeof(long int),
               ios::beg); // fixed length record

    file.write((char *)&new_separator, sizeof(long int));
  }

  int get_aux_entries() {
    ifstream file(this->data_name, ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(0, ios::end);         // ubicar cursos al final del archivo
    long total_bytes = file.tellg(); // cantidad de bytes del archivo
    total_bytes -= sizeof(long int) * 2;
    total_bytes -= (this->separator * this->get_entry_size());
    file.close();
    return total_bytes / this->get_entry_size();
  }

  int get_all_entries() {
    ifstream file(this->data_name, ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    file.seekg(0, ios::end);         // ubicar cursos al final del archivo
    long total_bytes = file.tellg(); // cantidad de bytes del archivo
    total_bytes -= sizeof(long int) * 2;
    file.close();
    return total_bytes / this->get_entry_size();
  }

public:
  SequentialFile(string file_name) : FileParent<TK>(file_name) {
    this->index_name = file_name + "_temp";
    this->max_aux = this->get_max_aux();
    this->separator = this->get_separator();
  }
  SequentialFile(string file_name, int ma, string csv_file)
      : FileParent<TK>(file_name, csv_file) {
    this->index_name = file_name + "_temp";
    this->max_aux = ma;
    this->separator = 0;

    vector<vector<string>> dataframe;
    vector<int> sizes;

    this->create_from_csv(csv_file, dataframe, sizes, 0);

    // clean file
    ofstream data_file(this->data_name, ios::binary | ios::trunc);

    // Add the headers
    // First the smallest, then the separtor of aux entreis, finally the max
    // size of the aux part of the  file
    int temp = -1;
    data_file.write((char *)&temp, sizeof(long int));
    data_file.write((char *)&this->separator, sizeof(long int));
    data_file.write((char *)&this->max_aux, sizeof(long int));

    data_file.close();

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
  virtual ~SequentialFile() {}

  pair<Entry *, int> binary_search_main(string key) {
    long int left = 0, right = this->separator - 1;
    while (left <= right) {
      long int mid = left + (right - left) / 2;

      Entry *current = this->get_entry(mid);
      if (current->get_key() == key) {
        return make_pair(current, mid);
      }

      if (current->get_key() < key) {
        left = mid + 1;
      } else {
        right = mid - 1;
      }

      delete current;
    }
    // 11 might not be enough
    Entry *current = this->get_entry(left - 1);
    return make_pair(current, left - 1);
  }

  string find(string key) {
    // 1. Search in the main part for the part using binary search
    auto prev_pair = binary_search_main(key);
    Entry *current = prev_pair.first;
    string record = "";
    while (true) {
      // 2. If found, return
      if (key == current->get_key()) {
        record = this->clean_record(current->get_key() + current->get_record());
        delete current;
        break;
      } else if (key < current->get_key()) {
        // we passed the key
        break;
      } else {
        // 3. If not found, use the pointer to go in the chain forward
        long int pos_next = current->get_pointer();
        delete current;
        current = get_entry(pos_next);
      }
    }
    return record;
  }

  vector<string> range_search(int start_key, int end_key) {
    // TODO
  }

  bool add(string record) {
    return insert_parsed(this->separate_record(record));
  }

  void merge() {
    // TODO
  }

  void insert_correctly(int prev_pos, Entry *prev, string record, string key) {

    // insert in this position
    // check if next is free to use
    // Otherwise, insert at the ned of the file
    // check if it's at the end so this doesn't fails
    Entry *next = nullptr;
    bool next_exist = false;
    if (this->get_all_entries() - 1 != prev_pos) {
      next = get_entry(prev_pos + 1);
      next_exist = true;
    }
    if (next_exist && next->get_pointer() != -2) {
      prev->change_pointer(this->get_all_entries());
      this->create_entry(prev->get_pointer(), record, key);
      overwrite_entry(prev, prev_pos);
    } else {
      prev->change_pointer(prev_pos + 1);
      overwrite_entry(prev, prev_pos);
      next->change_pointer(prev->get_pointer());
      next->change_key(key);
      next->change_record(record);
      overwrite_entry(next, prev_pos + 1);
    }
  }

  bool insert_parsed(string record) {
    // cleans key so it doesn't have space after
    string key = record.substr(0, this->key_size);
    size_t end = key.find_last_not_of(" ");
    key = key.substr(0, end + 1);

    // if there is nothing
    if (this->get_top() == -1) {
      this->create_entry(-1, record, key);
      this->change_separator(1);
      this->change_top(0);
      return true;
    }

    record = record.substr(this->key_size, record.size());
    auto prev_pair = binary_search_main(key);

    Entry *prev = prev_pair.first;
    int prev_pos = prev_pair.second;
    // Can't insert. unique key

    if (key < prev->get_key()) {
      prev_pos = this->get_top();
      delete prev;
      prev = this->get_entry(prev_pos);
      // smaller than the smaller, so we change the top
      if (key < prev->get_key()) {
        this->change_top(this->get_all_entries());
        this->create_entry(get_top(), record, key);
        return true;
      }
    } else if (key == prev->get_key()) {
      return false;
    }

    if (prev->get_pointer() == -1) {
      insert_correctly(prev_pos, prev, record, key);
      return true;
    }
    Entry *after = get_entry(prev->get_pointer());
    while (true) {
      if (key == after->get_key()) {
        // can't insert. Unique key
        return false;
      } else if (key < after->get_key()) {

        insert_correctly(prev_pos, prev, record, key);
        // check if merge is neccesary
        if (this->get_aux_entries() >= this->max_aux) {
          merge();
        }
        return true;
      } else {
        // keep travelling
        if (prev->get_pointer() == -1) {
          insert_correctly(prev_pos, prev, record, key);
          return true;
        } else {
          // Keep travelling
          prev_pos = prev->get_pointer();
          delete prev;
          prev = after;
          after = get_entry(prev->get_pointer());
        }
      }
    }
    // TODO
    // TEST
  }

  bool remove(string key) {
    // if there is nothing
    if (this->get_top() == -1) {
      return false;
    }

    auto prev_pair = binary_search_main(key);

    Entry *prev = prev_pair.first;
    int prev_pos = prev_pair.second;

    if (key < prev->get_key()) {
      prev_pos = this->get_top();
      delete prev;
      prev = this->get_entry(prev_pos);
      // smaller than the smaller, so isn't there
      if (key < prev->get_key()) {
        return false;
      } else if (key == prev->get_key()) {
        this->change_top(prev->get_pointer());
        // -2 indicates that it's deleted
        prev->change_pointer(-2);
        overwrite_entry(prev, prev_pos);
        return true;
      }
    } else if (key == prev->get_key()) {
      // 1. Find prev pos
      Entry *before = get_entry(prev_pos - 1);
      // iterate until we are 1 before
      // TODO
      return true;
    }

    // Isnt' there
    if (prev->get_pointer() == -1) {
      return false;
    }

    Entry *after = get_entry(prev->get_pointer());
    while (true) {
      if (key == after->get_key()) {
        // change pointers
        prev->change_pointer(after->get_pointer());
        overwrite_entry(prev, prev_pos);
        after->change_pointer(-2);
        overwrite_entry(after, prev->get_pointer());
        return true;
      } else if (after->get_key() < key) {
        // we passed the mark
        return false;
      } else {
        // keep travelling
        if (prev->get_pointer() == -1) {
          prev->change_pointer(after->get_pointer());
          overwrite_entry(prev, prev_pos);
          after->change_pointer(-2);
          overwrite_entry(after, prev->get_pointer());
          return true;
        } else {
          // Keep travelling
          prev_pos = prev->get_pointer();
          delete prev;
          prev = after;
          after = get_entry(prev->get_pointer());
        }
      }
    }
    // TODO
    // TEST
  }
};

#endif // HASH_H_
