#include "args.h"

Arguments parse_arguments(int argc, char* argv[]) {
    Arguments args;

    // Just base parsing, delete later
    if (argc >= 3) {
        args.searchpath = argv[1];
        for (int i = 2; i < argc; ++i) {
            args.filenames.push_back(argv[i]);
        }
    }
    
    return args;
}