#ifndef PARENT_H_
#define PARENT_H_

#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

template <class TK = int> class FileParent {
protected:
  string data_name;
  string header_name;

  int record_size;
  int key_size;
  // helper functions

  int get_header_name_size() {
    ifstream file(header_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    file.read(reinterpret_cast<char *>(&tmp), sizeof(int));
    return tmp;
  }

  int get_field_size(int pos) {
    ifstream file(header_name, ios::in | ios::binary);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

    int tmp;
    int header_name_size = get_header_name_size();
    file.seekg(sizeof(int) * 2 + pos * (header_name_size + sizeof(int)) +
                   header_name_size,
               ios::beg); // fixed length record
    file.read((char *)&tmp, sizeof(int));
    file.close();
    return tmp;
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

  string separate_record(string record) {
    istringstream ss(record);
    string parsed_record = "";
    int i = 0;
    for (string value; getline(ss, value, ',');) {
      int current_size = this->get_field_size(i);
      if (value.size() >= current_size) {
        parsed_record += value.substr(0, current_size);
      } else {
        parsed_record += value + string(current_size - value.size(), ' ');
      }
      i++;
    }
    return record;
  }

  string get_file_name(string _fileName, string sufix) {
    if (_fileName.find(".dat")!= -1)
      return _fileName.substr(0,_fileName.find(".dat")) + sufix +".dat";
    if (_fileName.find(".txt")!= -1)
      return _fileName.substr(0,_fileName.find(".txt")) + sufix +".txt";
    if (_fileName.find(".bin")!= -1)
      return _fileName.substr(0,_fileName.find(".bin")) + sufix +".bin";
    return _fileName+sufix;
  }

public:
  FileParent(string file_name) {
    this->data_name = get_file_name(file_name,"_data");
    this->header_name = get_file_name(file_name , "_header");
    this->record_size = this->get_record_size();
    this->key_size = this->get_field_size(0);
  }
  FileParent(string file_name, string csv_file) {
    cout << "doble input constructor" << endl;
    this->data_name = get_file_name(file_name,"_data");
    this->header_name = get_file_name(file_name , "_header");
    this->record_size = 0;
    this->key_size = 0;
  }
  virtual ~FileParent() {}

  virtual bool add(string record) = 0;

  virtual string find(TK key) = 0;

  virtual vector<string> range_search(TK start_key, TK end_key) = 0;

  virtual bool remove(TK key) = 0;

  bool create_headers_basic(int type) {
    ofstream header_file(this->header_name,
                         ios::out | ios::binary | ios::trunc);
    if (!header_file.is_open())
      throw("No se pudo abrir el archivo");
    int temp = 0;
    header_file.write((char *)&temp, sizeof(int));
    // this second int indicates the type of the strategy used for the file
    header_file.write((char *)&type, sizeof(int));

    header_file.close();
    return true;
  }

  bool create_from_csv(string csvfile, vector<vector<string>> &dataframe,
                       vector<int> &sizes, int type) {
    ifstream file(csvfile);
    if (!file.is_open())
      throw("No se pudo abrir el archivo");

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
    sizes = vector<int>(headers.size(), 0);

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
    ofstream header_file(this->header_name,
                         ios::out | ios::binary | ios::trunc);
    if (!header_file.is_open())
      throw("No se pudo abrir el archivo");
    header_file.write((char *)&longest_header, sizeof(int));
    header_file.write((char *)&type, sizeof(int));
    for (int i = 0; i < headers.size(); i++) {
      header_file.write((char *)&headers[i], sizeof(char) * longest_header);
      header_file.write((char *)&sizes[i], sizeof(int));
    }
    header_file.close();

    return true;
  }
};

#endif // PARENT_H_
