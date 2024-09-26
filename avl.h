#include "parent.h"
#include <chrono>
#include <cstring>
#include <fstream>
#include <initializer_list>
#include <iostream>
#include <vector>

#define HEAD_FILE 8;
#define BYTES_TO_NEXTDEL 0
#define BYTES_TO_LEFT 4
#define BYTES_TO_RIGHT 8
#define BYTES_TO_HEIGHT 12
#define BYTES_TO_DATA 16

using namespace std;

struct HeadAVL {
  long root = -1;
  long nextDel = -1;
};

struct LeavesAVL {
  long left;
  long right;
};

struct NodeAVL {
  long nextDel;
  long left;
  long right;
  int height;
  string data;
  NodeAVL(string record)
      : data(record), left(-1), right(-1), height(0), nextDel(-1) {}
  NodeAVL() {}
  int getKey(int key_size) { return stoi(data.substr(0, key_size)); }
};

template <class T> class AVLFile : public FileParent<T> {

  long getPosFisical(long pos) { return pos * this->getSizeNode() + HEAD_FILE; }

  HeadAVL head;
  string result;

  int getBalance(long pos) {
    ifstream file(this->data_name, ios::in | ios::binary);
    LeavesAVL leaves;
    file.seekg(getPosFisical(pos) + BYTES_TO_LEFT, ios::beg);
    file.read((char *)&leaves, sizeof(LeavesAVL));

    if (leaves.left == -1 && leaves.right == -1) {
      return 0;
    }
    int hLeft = -1, hRight = -1;
    if (leaves.left > -1) {
      file.seekg(getPosFisical(leaves.left) + BYTES_TO_HEIGHT, ios::beg);
      file.read((char *)&hLeft, sizeof(int));
    }

    if (leaves.right > -1) {
      file.seekg(getPosFisical(leaves.right) + BYTES_TO_HEIGHT, ios::beg);
      file.read((char *)&hRight, sizeof(int));
    }
    file.close();
    return hLeft - hRight;
  }

  int getHeight(long pos) {
    if (pos == -1)
      return -1;
    ifstream file(this->data_name, ios::in | ios::binary);
    int height = 0;
    file.seekg(getPosFisical(pos) + BYTES_TO_HEIGHT, ios::beg);
    file.read((char *)&height, sizeof(int));
    file.close();
    return height;
  }

  template <class K> K read(long pos) {
    K data;
    ifstream file(this->data_name, ios::in | ios::binary);
    file.seekg(pos, ios::beg);
    int pos_ = file.tellg();
    file.read((char *)&data, sizeof(K));
    file.close();
    return data;
  }

  NodeAVL read(long pos) {
    NodeAVL data;
    ifstream file(this->data_name, ios::in | ios::binary);
    file.seekg(pos, ios::beg);
    int pos_ = file.tellg();
    file.read((char *)&data, this->getSizeNode());
    file.close();
    return data;
  }

  template <class M> void write(long pos, M data) {
    ofstream file(this->data_name, ios::in | ios::out | ios::ate | ios::binary);
    file.seekp(pos, ios::beg);
    file.write((char *)&data, sizeof(M));
    file.flush();
    file.close();
  }

  void write(long pos, NodeAVL data) {
    ofstream file(this->data_name, ios::in | ios::out | ios::ate | ios::binary);
    file.seekp(pos, ios::beg);
    file.write((char *)&data, this->getSizeNode());
    file.flush();
    file.close();
  }

  void write(long pos, string data) {
    ofstream file(this->data_name, ios::in | ios::out | ios::ate | ios::binary);
    file.seekp(pos, ios::beg);
    file.write((char *)&data, data.size() * sizeof(char));
    file.flush();
    file.close();
  }

  long rotateRight(long pos) {
    long x, t, rightPos;
    x = read<long>(getPosFisical(pos) + BYTES_TO_LEFT);
    rightPos = read<long>(getPosFisical(pos) + BYTES_TO_RIGHT);
    t = read<long>(getPosFisical(x) + BYTES_TO_RIGHT);
    write<long>(getPosFisical(x) + BYTES_TO_RIGHT, pos);
    write<long>(getPosFisical(pos) + BYTES_TO_LEFT, t);

    int height = max(getHeight(t), getHeight(rightPos)) + 1;
    write<int>(getPosFisical(pos) + BYTES_TO_HEIGHT, height);
    int heightX =
        max(getHeight(pos),
            getHeight(read<long>(getPosFisical(x) + BYTES_TO_RIGHT))) +
        1;
    write<int>(getPosFisical(x) + BYTES_TO_HEIGHT, heightX);
    return x;
  };

  long rotateLeft(long pos) {
    long y, t, leftPos;
    leftPos = read<long>(getPosFisical(pos) + BYTES_TO_LEFT);
    y = read<long>(getPosFisical(pos) + BYTES_TO_RIGHT);
    t = read<long>(getPosFisical(y) + BYTES_TO_LEFT);
    write<long>(getPosFisical(y) + BYTES_TO_LEFT, pos);
    write<long>(getPosFisical(pos) + BYTES_TO_RIGHT, t);
    int height = max(getHeight(t), getHeight(leftPos)) + 1;
    write<int>(getPosFisical(pos) + BYTES_TO_HEIGHT, height);
    int heightY =
        max(getHeight(pos),
            getHeight(read<long>(getPosFisical(y) + BYTES_TO_RIGHT))) +
        1;
    write<int>(getPosFisical(y) + BYTES_TO_HEIGHT, heightY);
    return y;
  };

  void rewriteHead() {
    ofstream file(this->data_name, ios::in | ios::out | ios::ate | ios::binary);
    file.seekp(0, ios::beg);
    file.write((char *)&head, sizeof(HeadAVL));
    file.flush();
    file.close();
  }

  int getSizeNode() {
    return sizeof(long) * 3 + sizeof(int) + this->record_size;
  }

  pair<bool, long> insert(long posRoot, string record) {
    bool isAdded = true;
    fstream file;
    cout << posRoot << endl;
    if (posRoot == -1) {
      NodeAVL node(record);
      long pos;
      if (head.nextDel == -1) {
        file.open(this->data_name, ios::in | ios::out | ios::app | ios::binary);
        file.seekp(0, ios::end);
        pos = file.tellp();
        file.write((char *)&node, this->getSizeNode());
        file.flush();

      } else {
        file.open(this->data_name, ios::in | ios::out | ios::ate | ios::binary);
        file.seekp(getPosFisical(head.nextDel), ios::beg);
        pos = file.tellp();
        NodeAVL nodeDeleted;
        file.read((char *)&nodeDeleted, this->getSizeNode());
        write(getPosFisical(head.nextDel), node);
        posRoot = head.nextDel;
        head.nextDel = nodeDeleted.nextDel;
      }
      file.close();
      long sizeHeadAVL = sizeof(HeadAVL);
      long sizeNodeAVL = this->getSizeNode();
      long newpos = (pos - sizeHeadAVL) / sizeNodeAVL;
      return make_pair(true, newpos);
    }
    NodeAVL nodeRoot = read<NodeAVL>(getPosFisical(posRoot));
    int record_key = stoi(record.substr(0, this->key_size));
    if (record_key < nodeRoot.getKey(this->key_size)) {
      auto result = insert(nodeRoot.left, record);
      write<long>(getPosFisical(posRoot) + BYTES_TO_LEFT, result.second);
      isAdded = result.first;
    } else if (record_key > nodeRoot.getKey(this->key_size)) {
      auto result = insert(nodeRoot.right, record);
      write<long>(getPosFisical(posRoot) + BYTES_TO_RIGHT, result.second);
      isAdded = result.first;
    } else
      return make_pair(false, posRoot);

    nodeRoot = read<NodeAVL>(getPosFisical(posRoot));
    int height = max(getHeight(nodeRoot.left), getHeight(nodeRoot.right)) + 1;
    write<int>(getPosFisical(posRoot) + BYTES_TO_HEIGHT, height);

    int balance = getBalance(posRoot);

    nodeRoot = read<NodeAVL>(getPosFisical(posRoot));

    NodeAVL *leaveLeft =
        (nodeRoot.left > -1)
            ? new NodeAVL(read<NodeAVL>(getPosFisical(nodeRoot.left)))
            : nullptr;
    NodeAVL *leaveRight =
        (nodeRoot.right > -1)
            ? new NodeAVL(read<NodeAVL>(getPosFisical(nodeRoot.right)))
            : nullptr;

    if (balance > 1 && record_key < leaveLeft->getKey(this->key_size) &&
        nodeRoot.left > -1) {
      return make_pair(true, rotateRight(posRoot));
    }
    if (balance < -1 && record_key > leaveRight->getKey(this->key_size) &&
        nodeRoot.right > -1) {
      return make_pair(true, rotateLeft(posRoot));
    }
    if (balance > 1 && record_key > leaveLeft->getKey(this->key_size) &&
        nodeRoot.left > -1) {
      long leftRoot = rotateLeft(nodeRoot.left);
      write<long>(getPosFisical(posRoot) + BYTES_TO_LEFT, leftRoot);
      return make_pair(true, rotateRight(posRoot));
    }
    if (balance < -1 && record_key < leaveRight->getKey(this->key_size) &&
        nodeRoot.right > -1) {
      long rightRoot = rotateRight(nodeRoot.right);
      write<long>(getPosFisical(posRoot) + BYTES_TO_RIGHT, rightRoot);
      return make_pair(true, rotateLeft(posRoot));
    }
    file.close();
    return make_pair(isAdded, posRoot);
  }

  long mininValue(long pos) {
    auto *leaves =
        new LeavesAVL(read<LeavesAVL>(getPosFisical(pos) + BYTES_TO_LEFT));
    while (leaves->left > -1) {
      pos = leaves->left;
      leaves = new LeavesAVL(
          read<LeavesAVL>(getPosFisical(leaves->left) + BYTES_TO_LEFT));
    }
    delete leaves;
    return pos;
  }

  pair<bool, long> deleteFL(long posRoot, T key) {
    if (posRoot == -1)
      return make_pair(false, posRoot);

    NodeAVL nodeRoot = read<NodeAVL>(getPosFisical(posRoot));

    if (key < nodeRoot.getKey(this->key_size)) {
      auto result = deleteFL(nodeRoot.left, key);
      write<long>(getPosFisical(posRoot) + BYTES_TO_LEFT, result.second);
    } else if (key > nodeRoot.getKey(this->key_size)) {
      auto result = deleteFL(nodeRoot.right, key);
      write<long>(getPosFisical(posRoot) + BYTES_TO_RIGHT, result.second);
    } else {
      if (nodeRoot.left == -1 && nodeRoot.right == -1) {
        nodeRoot.nextDel = head.nextDel;
        head.nextDel = posRoot;
        write(getPosFisical(posRoot), nodeRoot);
        return make_pair(true, -1);
      }
      if (nodeRoot.left == -1) {
        head.nextDel = posRoot;
        return make_pair(true, nodeRoot.right);
      }
      if (nodeRoot.right == -1) {
        head.nextDel = posRoot;
        return make_pair(true, nodeRoot.left);
      }
      long posMinRight = mininValue(nodeRoot.right);
      NodeAVL minRightNode = read<NodeAVL>(getPosFisical(posMinRight));

      nodeRoot.data = minRightNode.data;
      // write(getPosFisical(posRoot), nodeRoot);
      write<string>(getPosFisical(posRoot) + BYTES_TO_DATA, nodeRoot.data);

      auto result =
          deleteFL(nodeRoot.right, minRightNode.getKey(this->key_size));
      write<long>(getPosFisical(posRoot) + BYTES_TO_RIGHT, result.second);
    }

    int height = max(getHeight(nodeRoot.left), getHeight(nodeRoot.right)) + 1;
    write<int>(getPosFisical(posRoot) + BYTES_TO_HEIGHT, height);

    int balance = getBalance(posRoot);

    if (balance > 1 && getBalance(nodeRoot.left) >= 0) {
      return make_pair(true, rotateRight(posRoot));
    }

    if (balance > 1 && getBalance(nodeRoot.left) < 0) {
      write<long>(getPosFisical(nodeRoot.left) + BYTES_TO_LEFT,
                  rotateLeft(nodeRoot.left));
      return make_pair(true, rotateRight(posRoot));
    }

    if (balance < -1 && getBalance(nodeRoot.right) <= 0) {
      return make_pair(true, rotateLeft(posRoot));
    }

    if (balance < -1 && getBalance(nodeRoot.right) > 0) {
      write<long>(getPosFisical(nodeRoot.right) + BYTES_TO_RIGHT,
                  rotateRight(nodeRoot.right));
      return make_pair(true, rotateLeft(posRoot));
    }

    return make_pair(true, posRoot);
  }

  pair<bool, string> search(long posRoot, T key) {
    if (posRoot == -1)
      return make_pair(false, string());

    NodeAVL nodeRoot = read<NodeAVL>(getPosFisical(posRoot));

    if (key < nodeRoot.getKey(this->key_size))
      return search(nodeRoot.left, key);

    if (key > nodeRoot.getKey(this->key_size))
      return search(nodeRoot.right, key);

    return make_pair(true, nodeRoot.data);
  }

  void range_search(long posRoot, T keyMin, T keyMax, vector<string> &result) {
    if (posRoot == -1)
      return;
    NodeAVL nodeRoot = read<NodeAVL>(getPosFisical(posRoot));
    if (keyMin < nodeRoot.getKey(this->key_size))
      range_search(nodeRoot.left, keyMin, keyMax, result);

    if (keyMin <= nodeRoot.getKey(this->key_size) &&
        nodeRoot.getKey(this->key_size) <= keyMax)
      result.push_back(nodeRoot.data);

    if (keyMax > nodeRoot.getKey(this->key_size))
      range_search(nodeRoot.right, keyMin, keyMax, result);
  }

  void writeOutputFile(string &record, const int &duration, const bool state) {
    // string parsed_record = "";
    // int start = 0;
    // for (int i = 0; i < this->get_number_fields(); i++) {
    //   int field_size = this->get_field_size(i);
    //   parsed_record += record.substr(start, field_size) + ",";
    //   start += field_size += 1;
    // }
    // parsed_record += "\n";
    ofstream outputFile;
    outputFile.open(result, ios::out | ios::trunc);
    cout << "Writing in writeoutput" << endl;
    if (state) {
      outputFile << record << "\n";
    }
    string outputString = "Execution time: " + to_string(duration) + "ms.\n";
    outputFile << outputString;
    outputFile.close();
    cout << "done with writeoutput" << endl;
  }
  void writeOutputFile(vector<string> &records, const int &duration,
                       const bool state) {

    string parsed_record = "";
    int start = 0;
    vector<int> sizes;
    for (int i = 0; i < this->get_number_fields(); i++) {
      sizes.push_back(this->get_field_size(i));
    }

    ofstream outputFile;
    outputFile.open(result, ios::out | ios::trunc);
    if (state) {
      for (int i = 0; i < records.size(); i++) {
        string parsed_record = "";
        int start = 0;
        for (int j = 0; j < sizes.size(); j++) {
          int field_size = sizes[j];
          parsed_record += records[i].substr(start, field_size);
          start += field_size += 1;
        }
        parsed_record += "\n";
        outputFile << parsed_record;
      }
    }
    string outputString = "Execution time: " + to_string(duration) + "ms.\n";
    outputFile << outputString;
  }
  void writeOutputFile(const int &duration, const bool state) {
    ofstream outputFile;
    outputFile.open(result, ios::out | ios::trunc);
    if (!state) {
      string outputString =
          "Process failed in " + to_string(duration) + "ms.\n";
      outputFile << outputString;
      return;
    }
    string outputString = "Execution time: " + to_string(duration) + "ms.\n";
    outputFile << outputString;
  }

public:
  AVLFile(string data_name, string csv_file, string output = "output.csv")
      : FileParent<T>(data_name, csv_file), result(output) {
    fstream file(this->data_name);
    if (!file.good()) {
      cout << "file isn't good" << endl;
      file.close();
      file.open(this->data_name, ios::out | ios::binary);
      file.write((char *)&head, sizeof(HeadAVL));
      file.flush();
    } else {
      file.read((char *)&head, sizeof(HeadAVL));
      cout << "Root: " << head.root << endl;
      cout << "NextDel: " << head.nextDel << endl;
    }
    file.close();

    initialize(csv_file);
  }

  void initialize(string csv_file) {
    vector<vector<string>> dataframe;
    vector<int> sizes;
    this->create_from_csv(csv_file, dataframe, sizes);

    // cleans file
    ofstream data_file(this->data_name, ios::out | ios::binary | ios::trunc);
    data_file.close();

    cout << "after create from csv" << endl;
    cout << "Size of dataframe: " << dataframe.size() << endl;
    for (auto iter = dataframe.begin(); iter != dataframe.end(); iter++) {
      string record = "";
      int i = 0;
      cout << i << endl;
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
      cout << record << endl;
      this->add_parsed(record);
    }
  }

  void viewFile() {
    fstream file(this->data_name, ios::in | ios::binary);
    file.seekg(0, ios::beg);
    file.read((char *)&head, sizeof(HeadAVL));
    cout << "Root: " << head.root << "\tNextDel: " << head.nextDel << endl;
    int count = 0;
    while (file.peek() != EOF) {
      NodeAVL node;
      file.read((char *)&node, this->getSizeNode());
      cout << count << " : ";
      cout << "Left(" << node.left << ") - Right(" << node.right;
      cout << ") - Height(" << node.height << ") - nextDel(" << node.nextDel
           << ")\t";
      cout << node.data;
      cout << endl;
      count++;
    }
  }

  // Use the separate record to receive a record separated with ,
  bool add(string record) { return add_parsed(this->separate_record(record)); }

  bool add_parsed(string record) {
    auto start = chrono::high_resolution_clock::now();
    auto root = insert(head.root, record);
    auto end = chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (root.first) {
      HeadAVL nodeRoot = read<HeadAVL>(0);
      if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
        if (head.root != root.second)
          head.root = root.second;
        rewriteHead();
      }
    }
    writeOutputFile(duration.count(), root.first);
    cout << (root.first ? "Insercion correcta del " : "Ya existe el ")
         << "registro con key(" << record.substr(0, this->key_size) << ")\n";
    return root.first;
  }

  bool remove(T key) {
    auto start = chrono::high_resolution_clock::now();
    auto root = deleteFL(head.root, key);
    auto end = chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    if (root.first) {
      HeadAVL nodeRoot = read<HeadAVL>(0);
      if (head.root != root.second || head.nextDel != nodeRoot.nextDel) {
        if (head.root != root.second)
          head.root = root.second;
        rewriteHead();
      }
    }
    writeOutputFile(duration.count(), root.first);
    cout << (root.first ? "Eliminacion correcta del " : "No se encontro el ")
         << "registro con key(" << key << ")\n";
    return root.first;
  }

  string search(T key) {
    auto start = chrono::high_resolution_clock::now();
    auto result = search(head.root, key);
    auto end = chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    writeOutputFile(result.second, duration.count(), result.first);
    cout << ((result.first) ? "Registro encontrado"
                            : "No se encontró el registro")
         << "con key(" << key << "):\n";
    cout << result.second;
    cout << endl;

    return result.second;
  }

  vector<string> range_search(T keyMin, T keyMax) {
    vector<string> result;
    auto start = chrono::high_resolution_clock::now();
    range_search(head.root, keyMin, keyMax, result);
    auto end = chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    bool state = (!result.empty()) ? true : false;
    writeOutputFile(result, duration.count(), state);

    if (state) {
      cout << "Registros en el rango [" << keyMin << ", " << keyMax << "]:\n";
      for (const auto &record : result) {
        cout << record;
        cout << endl;
      }
    } else
      cout << "No se encontraron registros en el rango [" << keyMin << ", "
           << keyMax << "].\n";

    return result;
  }
};
