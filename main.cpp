#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>

#include "args.h"

static void search_file_single_folder(const std::string& filename, const std::filesystem::path& searchpath) {
    if (!std::filesystem::exists(searchpath) || !std::filesystem::is_directory(searchpath)) {
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(searchpath)) {
        if (entry.is_regular_file() && entry.path().filename() == filename) {
            std::filesystem::path path = std::filesystem::absolute(entry.path());
            std::string output = "<pid>: " + filename + ": " + path.string() + "\n";
            ::write(1, output.c_str(), output.size());
            return;
        }
    }
}

int main (int argc, char* argv[]) {
    Arguments args = parse_arguments(argc, argv);
    if (args.filenames.empty()) {
        std::cerr << "Usage: " << argv[0] << " [-R] [-i] searchpath filename1 [filename2 ...]\n";
        return 1;
    }
    std::cout << "Searchpath: " << args.searchpath << "\n";
    std::cout << "Searching for " << args.filenames.size() << " file(s)...\n";

    // test file search
    search_file_single_folder(args.filenames.at(0), args.searchpath);
}