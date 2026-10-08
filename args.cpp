#include "args.h"
#include <iostream>
#include <getopt.h>

// Extracts flags (-R, -i), searchpath, and target filenames.
// 'optind = 1' resets getopt state. Using 'optind' after option processing
// ensures clean separation between flags and remaining path/file arguments.
Arguments parse_arguments(int argc, char* argv[]) {
    Arguments args;
    int current_option;
    optind = 1;
    while ((current_option = getopt(argc, argv, "Ri")) != EOF) {
        switch (current_option) {
            case 'R':
                args.recursive = true;
                break;
            case 'i':
                args.case_insensitive = true;
                break;
            case '?':
               args.filenames.clear();
               return args;
            default:
                break;
        }
    }

    if (optind < argc) 
    {
        args.searchpath = argv[optind++];
    } 
    
    while(optind < argc) 
    {
        args.filenames.emplace_back(argv[optind++]);
    }
    return args;
}