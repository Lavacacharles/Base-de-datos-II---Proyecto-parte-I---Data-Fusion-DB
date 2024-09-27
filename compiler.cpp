#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>
#include "avl.h"
#include "hash.h"

template <class T> struct Record {
    char key[260];
    char data[250]{};
    Record() = default;
    Record(T key_, const string &data_) {
        strcpy(this->key, convertToString(key_).c_str());
        strcpy(this->data, data_.c_str());
    }
    string getData() {
        string output = concatenate(key, ",") + data + "\n";
        return output;
    }
    T getKey() const { return convert<T>(key); }
};

template<class T>
Record<string> getRecord(string line) {
    string id = "", data="", field;
    istringstream lineStream(line);
    getline(lineStream, id, ',');
    data = line.substr(id.length() + 1);
    return Record<T>(id, data.c_str());
}

template <class R, class T>
static void buildFromCSV(AVLFile<R, T> &avl, string fileName, int idPosition) {
    fstream file(fileName);
    if (!file.good()) {
        cerr << "File not found: " << fileName << endl;
        return;
    }
    string campos;
    getline(file, campos);
    while (!file.eof()) {
        string line, id = "", field;
        getline(file, line);
        istringstream lineStream(line);

        string data = "";
        int count = 0;
        while (getline(lineStream, field, ',')) {
            if (count == idPosition) {
                id = field;
            } else {
                if (data != "")
                    data += ",";
                data += field;
            }

            count++;
        }
        if (data != "" || id != "") {
            Record<T> record(id, data.c_str());
            avl.add(record);
        }
    }
}

bool is_number(const std::string& s)
{
    std::string::const_iterator it = s.begin();
    while (it != s.end() && std::isdigit(*it)) ++it;
    return !s.empty() && it == s.end();
}

int extract_type(const std::string& s) {
    std::ifstream archivo(s + "_header", std::ios::binary);

    if (!archivo) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return -1;
    }

    int type = -1;

    archivo.seekg(sizeof(int), std::ios::beg);
    archivo.read(reinterpret_cast<char*>(&type), sizeof(int));
    archivo.close();

    return type;
}

std::vector<string> extraerNumerosEntre(const std::string& texto) {
    std::vector<string> numeros;
    std::regex betweenRegex(R"((\d+)\s+and\s+(\d+))"); // Expresión regular para capturar "números entre and"
    std::smatch match;

    if (std::regex_search(texto, match, betweenRegex)) {
        numeros.push_back(match[1].str()); // Primer número
        numeros.push_back(match[2].str()); // Segundo número
    }

    return numeros;
}

std::vector<string> separateId_data(const std::string& texto) {
    std::vector<string> data;
    string temp = "";
    for (int i = 0; i < texto.size(); i++) {
        if (texto[i] == ',') {
            data.push_back(temp);
            data.push_back(texto.substr(i+1, texto.size()));
            return data;
        }
        temp += texto[i];
    }
    return data;
}

class SQLCompiler {
private:
    std::string trim(const std::string& str) {
        static const std::regex trimmer(R"(^\s+|\s+$)");
        return std::regex_replace(str, trimmer, "");
    }

    std::vector<std::string> splitString(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(str);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(trim(token));
        }
        return tokens;
    }

    void validateCreateTable(const std::string& statement) {
        static const std::regex createTableRegex(R"(create\s+table\s+(\w+)\s+from\s+file\s+"([^"]+)\"\s+using\s+index\s+(\w+)\("(\w+)\"\))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, createTableRegex)) {
            throw std::runtime_error("Invalid CREATE TABLE syntax");
        }
        std::cout << "CREATE TABLE operation detected" << std::endl;
        std::cout << "Table name: " << matches[1] << std::endl;
        std::cout << "File path: " << matches[2] << std::endl;
        std::cout << "Index type: " << matches[3] << std::endl;
        std::cout << "Index column: " << matches[4] << std::endl;
        if (matches[3].str() == "hash") {
            auto* file = new ExtendibleHashingFile(matches[1].str(), 2, 3, matches[2].str());
            delete file;
            cout << "Se creo la tabla correctamente" << endl;
        } else if (matches[3].str() == "avl") {
            auto* file = new AVLFile<Record<string>, string>(matches[1].str());
            buildFromCSV(*file, matches[2].str(), 0);
            delete file;
            cout << "Se creo la tabla correctamente" << endl;
        }
    }

    void validateSelect(const std::string& statement) {
        static const std::regex selectRegex(R"(select\s+from\s+(\w+)\s+where\s+(\w+)\s*(=|between)\s*(.*))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, selectRegex)) {
            throw std::runtime_error("Invalid SELECT syntax");
        }
        if (matches[3] == "between") {
            std::cout << "SEARCH RANGE operation detected" << std::endl;
            cout << "Tipo: " << extract_type(matches[1].str()) << endl;
            if (extract_type(matches[1].str()) == 0) {
                cout << "Table Search: " << matches[1].str() << std::endl;
                auto* file = new ExtendibleHashingFile(matches[1].str());
                vector<string> range = extraerNumerosEntre(matches[4].str());
                cout << "Key: " << range[0] << " " << range[1]  << std::endl;
                const vector<string> result = file->range_search(stoi(range[0]), stoi(range[1]));
                cout << "Result: " << std::endl;
                for (const auto & i : result) {
                    if (i != "Key not found")
                        cout << i << endl;
                }
                cout << "-- End --" << std::endl;
                delete file;
            }
            else if(extract_type(matches[1].str()) == 1) {
                cout << "Table Search: " << matches[1].str() << std::endl;
                auto* file = new AVLFile<Record<string>, string>(matches[1].str());
                vector<string> range = extraerNumerosEntre(matches[4].str());
                cout << "Key: " << range[0] << " " << range[1]  << std::endl;
                const vector<string> result = file->rangeSearch(range[0], range[1]);
                cout << "Result: " << std::endl;
                for (const auto & i : result) {
                    cout << i << endl;
                }
                cout << "-- End --" << std::endl;
                delete file;
            }
        } else {
            std::cout << "SEARCH operation detected" << std::endl;
            cout << "Tipo: " << extract_type(matches[1].str()) << endl;
            if (extract_type(matches[1].str()) == 0) {
                cout << "Table Search: " << matches[1].str() << std::endl;
                auto* file = new ExtendibleHashingFile(matches[1].str());
                cout << "Key: " << stoi(matches[4].str()) << std::endl;
                const string result = file->find(stoi(matches[4].str()));
                cout << "Find Result: " << result << endl;
                cout << "-- End --" << std::endl;
                delete file;
            }
            else if(extract_type(matches[1].str()) == 1) {
                cout << "Table Search: " << matches[1].str() << std::endl;
                auto* file = new AVLFile<Record<string>, string>(matches[1].str());
                cout << "Key: " << matches[4].str() << std::endl;
                const string result = file->search(matches[4].str());
                cout << "Find Result: " << result << endl;
                cout << "-- End --" << std::endl;
                delete file;
            }
        }
    }

    void validateInsert(const std::string& statement) {
        static const std::regex insertRegex(R"(insert\s+into\s+(\w+)\s+values\s*\((.*)\))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, insertRegex)) {
            throw std::runtime_error("Invalid INSERT syntax");
        }
        std::cout << "INSERT operation detected" << std::endl;
        cout << "Tipo: " << extract_type(matches[1].str()) << endl;
        if (extract_type(matches[1].str()) == 0) {
            cout << "Table Search: " << matches[1].str() << std::endl;
            auto* file = new ExtendibleHashingFile(matches[1].str());
            const bool is_success = file->add(matches[2].str());
            cout << (is_success? "Se inserto correctamente" + matches[2].str()  : "El key se repite o hubo un fallo interno") << std::endl;
            cout << "-- End --" << std::endl;
            delete file;
        }
        else if(extract_type(matches[1].str()) == 1) {
            cout << "Table Search: " << matches[1].str() << std::endl;
            auto* file = new AVLFile<Record<string>, string>(matches[1].str());
            const bool is_success = file->add(getRecord<string>(matches[2].str()));
            cout << (is_success? "Se inserto correctamente" + matches[2].str() : "El key se repite o hubo un fallo interno") << std::endl;
            cout << "-- End --" << std::endl;
            delete file;
        }
    }

    void validateDelete(const std::string& statement) {
        static const std::regex deleteRegex(R"(delete\s+from\s+(\w+)\s+where\s+(\w+)\s*=\s*(.*))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, deleteRegex)) {
            throw std::runtime_error("Invalid DELETE syntax");
        }
        std::cout << "REMOVE operation detected" << std::endl;
        cout << "Tipo: " << extract_type(matches[1].str()) << endl;
        if (extract_type(matches[1].str()) == 0) {
            cout << "Table Search: " << matches[1].str() << std::endl;
            auto* file = new ExtendibleHashingFile(matches[1].str());
            cout << "Key: " << matches[3].str() << std::endl;
            const bool is_success = file->remove(stoi(matches[3].str()));
            cout << (is_success? "Se Elimino correctamente el registro con key " + matches[3].str()  : "Ocurrio un error, intenta de nuevo") << std::endl;
            cout << "-- End --" << std::endl;
            delete file;
        }
        else if(extract_type(matches[1].str()) == 1) {
            cout << "Table Search: " << matches[1].str() << std::endl;
            auto* file = new AVLFile<Record<string>, string>(matches[1].str());
            cout << "Key: " << matches[3].str() << std::endl;
            const bool is_success = file->remove(matches[3].str());
            cout << (is_success? "Se Elimino correctamente el registro con key " + matches[3].str()  : "Ocurrio un error, intenta de nuevo") << std::endl;
            cout << "-- End --" << std::endl;
            delete file;
        }
    }

public:
    void processQuery(const std::string& query) {
        std::vector<std::string> statements = splitString(query, ';');
        
        for (const auto& statement : statements) {
            if (statement.empty()) continue;
            
            try {
                std::istringstream iss(statement);
                std::string command;
                iss >> command;
                
                if (command == "create") {
                    validateCreateTable(statement);
                }
                else if (command == "select") {
                    validateSelect(statement);
                }
                else if (command == "insert") {
                    validateInsert(statement);
                }
                else if (command == "delete") {
                    validateDelete(statement);
                }
                else {
                    throw std::runtime_error("Unknown command: " + command);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error processing statement: " << statement << std::endl;
                std::cerr << "  " << e.what() << std::endl;
            }
        }
    }
};

int main() {
    SQLCompiler compiler;
    std::string query_create_hash = "create table Customer from file \"../datos_small.csv\" using index hash(\"Codigo\");";
    std::string query_create_avl = "create table CustomerAVL from file \"../datos_small.csv\" using index avl(\"Codigo\");";
    std::string query_find1_hash = "select from Customer where Codigo = 1;";
    std::string query_find1_avl = "select from CustomerAVL where Codigo = 1;";
    std::string query_range_hash = "select from Customer where Codigo between 22 and 32;";
    std::string query_range_avl =  "select from CustomerAVL where Codigo between 22 and 32;";
    std::string query_insert_hash = "insert into Customer values (3,John,Doe,5);";
    std::string query_insert_avl = "insert into CustomerAVL values (3,John,Doe,5);";
    std::string query_find2_hash = "select from Customer where Codigo = 3;";
    std::string query_find2_avl = "select from CustomerAVL where Codigo = 3;";
    std::string query_remove_hash = "delete from Customer where Codigo = 3;";
    std::string query_remove_avl = "delete from CustomerAVL where Codigo = 3;";


    compiler.processQuery(query_create_hash);
    cout << endl;
    compiler.processQuery(query_create_avl);
    cout << endl;

    compiler.processQuery(query_find1_hash);
    cout << endl;
    compiler.processQuery(query_find1_avl);
    cout << endl;
    compiler.processQuery(query_range_hash);
    cout << endl;
    compiler.processQuery(query_range_avl);
    cout << endl;
    compiler.processQuery(query_insert_hash);
    cout << endl;
    compiler.processQuery(query_insert_avl);
    cout << endl;
    compiler.processQuery(query_find2_hash);
    cout << endl;
    compiler.processQuery(query_find2_avl);
    cout << endl;
    compiler.processQuery(query_remove_hash);
    cout << endl;
    compiler.processQuery(query_remove_avl);
    cout << endl;

    return 0;
}