#ifndef FILE_READER
#define FILE_READER

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <time.h>
#include <map>
#include <algorithm>

double atod(const char *p) {
    double r = 0.0;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        r = (r*10.0) + (*p - '0');
        ++p;
    }
    if (*p == '.') {
        double f = 0.0;
        int n = 0;
        ++p;
        while (*p >= '0' && *p <= '9') {
            f = (f*10.0) + (*p - '0');
            ++p;
            ++n;
        }
        r += f / std::pow(10.0, n);
    }
    if (neg) {
        r = -r;
    }
    return r;
}

struct Code_manager
{
    std::map<std::string, std::pair<std::string, std::string> > variables;
};

class file_reader
{
public:
    file_reader(std::string filename, bool verbose);   
private:
    int                                         readFile();
    void                                      printErrors();
    std::string                            getError(int code);
    double                                  getValue(std::string v_name);
    void                                      runCode(int i, std::string name, std::string type, std::string value, bool p_type);
    void                                      runCode(int i, std::string v_name, std::string v_arg);
    bool                                      verbose = false;
    std::string                             filename;
    Code_manager                    v_man;
    std::vector<std::string>      lines;
    std::vector<std::string>      reserved_keywords;
    std::map<int, int>               error_codes;
    clock_t                                  _readStart;
};

file_reader::file_reader(std::string filename, bool v = false) : verbose(v)
{
    reserved_keywords.push_back("int");
    reserved_keywords.push_back("float");
    reserved_keywords.push_back("double");

    std::ifstream file(filename.c_str());
    if(file) {
        this->filename = filename;
        readFile();
    } else {
        std::cout << "Error Opening file." << std::endl;
    }
}

std::string file_reader::getError(int code)
{
    if(code == 0) {
        return "No type or value assigned.";
    } else if(code==1) {
        return "Variable uninitialized.";
    } else if(code==-1){
        return "Unreadable code.";
    } else if(code==-2){
        return "Missing semicolon.";
    } else if(code==2) {
        return "This function need arguments.";
    } else if(code==3){
        return "Variable undeclared.";
    } else if(code==4) {
        return "No such type.";
    } else if(code==5) {
        return "This is a reserved keyword.";
    } else {
        return "No such error code.";
    }
}

void file_reader::printErrors()
{
    for(int i=0; i<lines.size(); i++){
        if(error_codes.find(i) != error_codes.end()){
            std::cout << "Error [" << error_codes[i] << "]: " << getError(error_codes[i]) << std::endl;
            std::cout <<  "    line " << i+1 <<": " <<  lines[i] << std::endl << std::endl;
        }
    }
}

int file_reader::readFile()
{
    _readStart = clock();
    std::ifstream file(filename.c_str());
    std::string line;

    if(verbose)
            std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] FILE_OPENED NAME: " << filename << std::endl;

    for(int i=0; std::getline(file, line); i++) {
        lines.push_back(line);
        std::string v_name, v_type, v_value, v_arg;
        bool p_type = false, p_value = false, p_func = false, p_closed = false, no_comment = true;
        int nb_slash = 0, p_open = 0;
        if(!line.empty()) {
            for(char& ch : line) {
                if(ch == ';'){
                    if(!p_func) runCode(i, v_name, v_type, v_value, p_type);
                    else runCode(i, v_name, v_arg);
                   
                    p_type = false; p_value=false;p_func=false;
                    nb_slash = 0; p_open = 0;
                    v_name.clear(); v_type.clear(); v_value.clear(); v_arg.clear();
                    p_closed = true;

                    continue;
                } else if(nb_slash>=2){
                    no_comment = false;
                    break;
                } else if(ch == ' ') {
                    continue;
                } else if(ch=='/'){
                    nb_slash +=1;
                    continue;
                } else if(ch=='('){
                    p_open=1;
                    p_func = true;
                } else if(ch==')'){
                    p_open=0;
                } else if(ch == ':') {
                    p_type = true;
                } else if(ch == '=') {
                    p_value = true;
                } else if(p_value) {
                    v_value += ch;
                } else if(p_type) {
                    v_type += ch;
                } else if(p_func) {
                    v_arg += ch;
                } else {
                    v_name += ch;
                }
            }
            if(!p_closed && no_comment){
                error_codes[i] = -2;
                if (verbose)
                    std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: -2" << std::endl;
            }
        }
    }
    printErrors();
}

void file_reader::runCode(int i, std::string v_name, std::string v_type, std::string v_value, bool p_type)
{
    if(!v_name.empty()){
        if(std::find(reserved_keywords.begin(), reserved_keywords.end(), v_name)!=reserved_keywords.end())
        {
            error_codes[i] = 5; // Reserved keyword
            if (verbose)
                std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 5" << std::endl;
        } else if(!v_type.empty()) {
            if(std::find(reserved_keywords.begin(), reserved_keywords.end(), v_type)!=reserved_keywords.end()) {
                bool newVar = (v_man.variables.find(v_name) == v_man.variables.end());
                v_man.variables[v_name] = std::make_pair(v_type, v_value);
                if(verbose){
                    if(newVar)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_VARIABLE NAME: " << v_name << " TYPE: " << v_type << " VALUE: " << v_value << std::endl;
                    else
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] ALTER_VARIABLE NAME: " << v_name << " TYPE: " << v_type << " VALUE: " << v_value << std::endl;
                }
            } else {
                error_codes[i] = 4; // Unknown type
                if (verbose)
                    std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 4" << std::endl;
            }
        } else if(!v_value.empty()) {
            if(v_man.variables.find(v_name) != v_man.variables.end()) {
                v_man.variables[v_name].second = v_value;
                if(verbose)
                    std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] ALTER_VARIABLE NAME: " << v_name << " VALUE: " << atoi(v_value.c_str()) << std::endl;
            } else {
                // Since double is the largest implemented variable yet, every variable without type will be a double.
                if(!p_type) {
                    error_codes[i] = 1; // Error 1: Variable undeclared before
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 1" << std::endl;
                } else {
                    v_man.variables[v_name] = std::make_pair("double", v_value);
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_DOUBLE_VARIABLE NAME: " << v_name << " VALUE: " << atod(v_value.c_str()) << std::endl;
                }
            }
        } else {
            error_codes[i] =0; // Error 0: Just a name, no type nor value
            if(verbose)
                std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 0" << std::endl;
        }
    }
}

//template<typename T>
double file_reader::getValue(std::string v_name)
{
    if(v_man.variables[v_name].first == "int") {
        return atoi(v_man.variables[v_name].second.c_str());
    } else if(v_man.variables[v_name].first == "float") {
        return atof(v_man.variables[v_name].second.c_str());
    } else if(v_man.variables[v_name].first == "double") {
        return atod(v_man.variables[v_name].second.c_str());
    }
}

void file_reader::runCode(int i, std::string v_name, std::string v_arg)
{
    if(!v_name.empty()) {
        if(!v_arg.empty()) {
            if(v_name=="print"){
                if(v_man.variables.find(v_arg) != v_man.variables.end()) {
                    std::cout << getValue(v_arg) << std::endl;
                } else {
                    error_codes[i] = 3; // Error: No Such Variable
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 3" << std::endl;
                }
            }
        } else {
            error_codes[i] = 2; // Error 2: No arguments there
            if(verbose)
                std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 2" << std::endl;
        }
    }
}

#endif