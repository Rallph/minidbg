#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <unistd.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <linenoise.h>

#include "debugger.hpp"

using namespace minidbg;

std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> out{};
    std::stringstream ss {s};
    std::string item;

    while (std::getline(ss, item, delimiter)) {
        out.push_back(item);
    }

    return out;
}

void debugger::run() {
    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);

    char* line = nullptr;
    while ((line = linenoise("minidbg> ")) != nullptr) {
        // handle command
        linenoiseHistoryAdd(line);
        linenoiseFree(line);
    }
}

void execute_debugee(const std::string& program_name) {
    if (ptrace(PTRACE_TRACEME, 0, nullptr, nullptr) < 0) {
        std::cerr << "Error in ptrace\n";
        return;
    }
    execl(program_name.c_str(), program_name.c_str(), nullptr);
}

int main(int argc, char* argv[]) {

    if (argc < 2) {
        std::cerr << "Program name not specified";
        return -1;
    }

    auto program_name = argv[1];

    auto pid = fork();
    if (pid == 0) {
        // we're in child process
        // execute debugee
    } else if (pid >= 1) {
        // we're in parent process
        // execute debugger
        std::cout << "Started debugging process " << pid << '\n';
        debugger dbg{program_name, pid};
        dbg.run();
    }

}