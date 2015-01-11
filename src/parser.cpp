#include <iostream>
#include "file_reader.hpp"
///////////////
///
///         TODO LIST:
///         -one array for all variables, rather than one array per type
///         -functions
///         -include files
///         -standard library, with basic functions, such as 'print()'
///
///////////////

std::pair<bool, std::string> arg_parser(int argc, const char* argv[])
{
    std::string arg, file;
    bool f_parsed = false, a_parsed = false;

    for(int a=1;a<argc;a++) {
        std::string argument = (std::string)argv[a];
        for(char& ch : argument) {
            if(ch == '-') {
                a_parsed = true;
            } else if(a_parsed) {
                arg += ch;
            } else {
                file += ch;
            }
        }
    }

    bool verbose = false;
    std::size_t found = arg.find("v");
    if(found!=std::string::npos)
        verbose  = true;

    std::pair<bool, std::string> values;
    values = std::make_pair(verbose, file);
    return values;
}

int main(int argc, char const *argv[])
{
    std::pair<bool, std::string> parsed = arg_parser(argc, argv);

    if(!parsed.second.empty()) {
            file_reader reader(parsed.second, parsed.first);
    } else {
        std::cout << "Usage: parser [arguments] [filename]" << std::endl;
        return 0;
    }
    
    return 0;
}