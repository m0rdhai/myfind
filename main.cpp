#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <unistd.h>
#include <sys/wait.h>
#include <algorithm>
#include <cctype>

#include "args.h"

// Transforms every character in the string to lowercase
static std::string to_lowercase(const std::string& str) {
    std::string lower= str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower;
}

// Performs exact or case-insensitive comparison based on the -i flag
static bool is_matching_filename(const std::string& current, const std::string& target, bool case_insensitive) {
    if (case_insensitive) {
        return to_lowercase(target) == to_lowercase(current);
    } else {
        return target == current;
    }
}

// Guarantees complete string buffer delivery over POSIX write() instead of std::cout to guarantee atomic output
static void safe_write_stdout(const std::string& str) {
    const char* buf = str.c_str();
    size_t count = str.size();
    while (count > 0) {
        ssize_t written = ::write(STDOUT_FILENO, buf, count);
        if (written <= 0) {
            if (written < 0 && errno == EINTR) {
                continue;
            }
            break;
        }
        buf += written;
        count -= written;
    }
}

// Inspects single directory entry and writes match
static void process_entry(const std::filesystem::directory_entry& entry, const std::string& filename, const Arguments& args, pid_t pid) {
    std::error_code ec;
    if (entry.is_regular_file(ec) && is_matching_filename(entry.path().filename().string(), filename, args.case_insensitive)) {
        std::filesystem::path path = std::filesystem::weakly_canonical(entry.path(), ec);
        if (!ec) {
            std::string output = std::to_string(pid) + ": " + filename + ": " + path.string() + "\n";
            safe_write_stdout(output);
        }
    }
}

// Traverses directory (flat or recursively with -R) and outputs matches
static void search_file_single_folder(const std::string& filename, const std::filesystem::path& searchpath, const Arguments& args) {
    pid_t pid = getpid();

    try {
        if (!std::filesystem::exists(searchpath) || !std::filesystem::is_directory(searchpath)) {
            return;
        }
        // Skip over files with no access
        constexpr auto dir_opts = std::filesystem::directory_options::skip_permission_denied;

        // Recursive search
        if (args.recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(searchpath, dir_opts)) {
                process_entry(entry, filename, args, pid);
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(searchpath, dir_opts)) {
                process_entry(entry, filename, args, pid);
            }
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error searching for " << filename << ": " << e.what() << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error searching for " << filename << ": " << e.what() << "\n";
    }
}

/*
 * Output-Synchronization Concept
 *
 * Standard C++ output streams like std::cout maintain internal user-space buffers.
 * When multiple child processes spawned via fork() write concurrently to stdout,
 * these buffers can flush in interleaved fragments, resulting in mangled output lines.
 *
 * To guarantee atomic, thread- and process-safe full-line output without explicit
 * cross-process locking (e.g., semaphores or mutexes):
 *
 * 1. Format the complete line (including PID, filename, path, and newline) into a single
 *    contiguous std::string buffer in memory first.
 * 2. Execute a direct POSIX system call ::write(STDOUT_FILENO, buffer.c_str(), count).
 * 3. Wrap the system call in a loop to handle potential partial writes or EINTR
 *    interruptions, ensuring the complete line is committed atomically to the kernel's
 *    file description table in a single uninterrupted operation.
 */

int main(int argc, char* argv[]) {
    Arguments args = parse_arguments(argc, argv);
    if (args.filenames.empty()) {
        std::cerr << "Usage: " << argv[0] << " [-R] [-i] searchpath filename1 [filename2 ...]\n";
        return 1;
    }

    std::cout << "Searchpath: " << args.searchpath << "\n";
    std::cout << "Searching for " << args.filenames.size() << " file(s)...\n";
    // Flush output before forking
    std::cout << std::flush;

    std::filesystem::path searchpath = args.searchpath;
    bool fork_failed = false;

    // Spawns child processes for each file
    for (const auto& filename : args.filenames) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork failed");
            fork_failed = true;
            break;
        }
        if (pid == 0) {
            search_file_single_folder(filename, searchpath, args);
            _exit(0);
        }
    }

    // Wait until all children finished
    int status = 0;
    while (true) {
        errno = 0;
        pid_t wpid = wait(&status);
        if (wpid < 0) {
            if (errno == EINTR) {
                continue;
            }
            break; // No more child processes left
        }
    }

    return fork_failed ? 1 : 0;
}