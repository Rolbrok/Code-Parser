#ifndef CODE_MANAGER
#define CODE_MANAGER

#include <iostream>
#include <vector>
#include <map>

class Code_manager
{
public:
    Code_manager();
    std::map<std::string, int>                          int_variables;
    std::map<std::string, float>                       float_variables;
    std::map<std::string, double>                   double_variables;
};

Code_manager::Code_manager()
{

}

#endif