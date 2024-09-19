#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <stdexcept>

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
    }

    void validateSelect(const std::string& statement) {
        static const std::regex selectRegex(R"(select\s+from\s+(\w+)\s+where\s+(\w+)\s*(=|between)\s*(.*))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, selectRegex)) {
            throw std::runtime_error("Invalid SELECT syntax");
        }
        if (matches[3] == "between") {
            std::cout << "SEARCH RANGE operation detected" << std::endl;
        } else {
            std::cout << "SEARCH operation detected" << std::endl;
        }
        std::cout << "Table: " << matches[1] << std::endl;
        std::cout << "Column: " << matches[2] << std::endl;
        std::cout << "Condition: " << matches[3] << " " << matches[4] << std::endl;
    }

    void validateInsert(const std::string& statement) {
        static const std::regex insertRegex(R"(insert\s+into\s+(\w+)\s+values\s*\((.*)\))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, insertRegex)) {
            throw std::runtime_error("Invalid INSERT syntax");
        }
        std::cout << "INSERT operation detected" << std::endl;
        std::cout << "Table: " << matches[1] << std::endl;
        std::cout << "Values: " << matches[2] << std::endl;
    }

    void validateDelete(const std::string& statement) {
        static const std::regex deleteRegex(R"(delete\s+from\s+(\w+)\s+where\s+(\w+)\s*=\s*(.*))");
        std::smatch matches;
        if (!std::regex_search(statement, matches, deleteRegex)) {
            throw std::runtime_error("Invalid DELETE syntax");
        }
        std::cout << "REMOVE operation detected" << std::endl;
        std::cout << "Table: " << matches[1] << std::endl;
        std::cout << "Condition: " << matches[2] << " = " << matches[3] << std::endl;
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
    std::string query = "create table Customer from file \"C:\\data.csv\" using index hash(\"DNI\");"
                        "select from Customer where DNI = x;"
                        "select from Customer where DNI between x and y;"
                        "insert into Customer values (1, John, Doe);"
                        "delete from Customer where DNI = x;"
                        "invalid statement;";
    
    compiler.processQuery(query);
    return 0;
}