#ifndef FILE_READER
#define FILE_READER

#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <time.h>
#include "Code_manager.hpp"

double naive(const char *p) {
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

class file_reader
{
public:
    file_reader(std::string filename, bool verbose);   
private:
    int                                         readFile();
    void                                      printErrors();
    std::string                            getError(int code);
    bool                                      keywordCheck(std::string key);
    bool                                      verbose = false;
    std::string                             filename;
    Code_manager                    v_man;
    std::vector<std::string>      lines;
    std::map<int, int>               error_codes;
};

file_reader::file_reader(std::string filename, bool v = false) : verbose(v)
{
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
    } else if(code==2) {
        return "This function need arguments.";
    } else if(code==3){
        return "Variable undeclared.";
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

bool file_reader::keywordCheck(std::string key)
{
    if(!key.empty()){
        return true;
    } else {
        return false;
    }
}

int file_reader::readFile()
{
    clock_t _readStart = clock();
    std::ifstream file(filename.c_str());
    std::string line;

    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] FILE_OPENED NAME: " << filename << std::endl;

    for(int i=0; std::getline(file, line); i++) {
        lines.push_back(line);
        std::string v_name, v_type, v_value, v_arg;
        bool p_type = false, p_value = false, p_func = false;
        int nb_slash = 0, p_open = 0;
        if(!line.empty()) {
            for(char& ch : line) {
                if(ch == ';' || nb_slash>=2){
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
        }
        if(!p_func && keywordCheck(v_name)){
            if(!v_type.empty()) {
                if(v_type=="int"){
                    v_man.int_variables[v_name] = atoi(v_value.c_str());
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_INT_VARIABLE NAME: " << v_name << " VALUE: " << atoi(v_value.c_str()) << std::endl;
                } else if(v_type=="float"){
                    v_man.float_variables[v_name] = atof(v_value.c_str());
                     if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_FLOAT_VARIABLE NAME: " << v_name << " VALUE: " << atof(v_value.c_str()) << std::endl;
                } else if(v_type=="double"){
                    v_man.double_variables[v_name] = naive(v_value.c_str());
                     if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_DOUBLE_VARIABLE NAME: " << v_name << " VALUE: " << naive(v_value.c_str()) << std::endl;
                }
            } else if(!v_value.empty()) {
                if(v_man.int_variables[v_name]) {
                    v_man.int_variables[v_name] = atoi(v_value.c_str());
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] ALTER_INT_VARIABLE NAME: " << v_name << " VALUE: " << atoi(v_value.c_str()) << std::endl;
                } else if(v_man.float_variables[v_name]) {
                    v_man.float_variables[v_name] = atof(v_value.c_str());
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] ALTER_FLOAT_VARIABLE NAME: " << v_name << " VALUE: " << atof(v_value.c_str()) << std::endl;
                } else if(v_man.double_variables[v_name]) {
                    v_man.double_variables[v_name] = naive(v_value.c_str());
                    if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] ALTER_DOUBLE_VARIABLE NAME: " << v_name << " VALUE: " << naive(v_value.c_str()) << std::endl;
                } else {
                    // Since double is the largest implemented variable yet, every variable without type will be a double.
                    if(!p_type) {
                        error_codes[i] = 1; // Error 1: Variable undeclared before
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 1" << std::endl;
                    } else {
                        v_man.double_variables[v_name] = naive(v_value.c_str());
                        if(verbose)
                            std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] NEW_DOUBLE_VARIABLE NAME: " << v_name << " VALUE: " << naive(v_value.c_str()) << std::endl;
                    }
                }
            } else {
                error_codes[i] =0; // Error 0: Just a name, no type nor value
                if(verbose)
                    std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: 0" << std::endl;
            }
        } else if(p_func) { 
            if(!v_name.empty()) {
                if(!v_arg.empty()) {
                    if(v_name=="print"){
                        if(v_man.int_variables[v_arg]) {
                            std::cout << v_man.int_variables[v_arg] << std::endl;
                        } else if(v_man.float_variables[v_arg]) {
                            std::cout << v_man.float_variables[v_arg] << std::endl;
                        } else if(v_man.double_variables[v_arg]) {
                            std::cout << v_man.double_variables[v_arg] << std::endl;
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
        } else if(!v_type.empty() || !v_value.empty()) {
            error_codes[i] = -1; // Error -1: WTF
            if(verbose)
                        std::cout << "[" <<double(clock()-_readStart)/CLOCKS_PER_SEC << "s] DECL_ERROR_CODE LINE: " << i+1 << " CODE: -1" << std::endl;
        }
    }
    printErrors();
}
#endif