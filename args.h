#ifndef MYFIND_ARGS_H
#define MYFIND_ARGS_H

#include <string>
#include <vector>

struct Arguments {
    std::string searchpath;
    std::vector<std::string> filenames;
    bool recursive = false;
    bool case_insensitive = false;
};

Arguments parse_args(int argc, char* argv[]);

#endif //MYFIND_ARGS_H
