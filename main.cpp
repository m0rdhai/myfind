#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>

#include "args.h"

static void search_file_single_folder(const std::string& filename, const std::filesystem::path& searchpath) {
    pid_t pid = getpid();
    if (!std::filesystem::exists(searchpath) || !std::filesystem::is_directory(searchpath)) {
        return;
    }
    for (const auto& entry : std::filesystem::directory_iterator(searchpath)) {
        if (entry.is_regular_file() && entry.path().filename() == filename) {
            std::filesystem::path path = std::filesystem::absolute(entry.path());
            std::string output = std::to_string(pid) + ": " + filename + ": " + path.string() + "\n";
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

    std::filesystem::path searchpath = args.searchpath;

    for (const auto& filename : args.filenames) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }
        else if (pid == 0) {
            search_file_single_folder(filename, searchpath);
            exit(0);
        }
    }
    int status = 0;
    while (wait(&status) > 0) {}
    return 0;
}