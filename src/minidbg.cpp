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

bool is_prefix(const std::string &s, const std::string &of) {
    if (s.size() > of.size()) {
        return false;
    }

    return std::equal(s.begin(), s.end(), of.begin());
}

void debugger::handle_command(const std::string& line) {
    auto args = split(line, ' ');
    auto command = args[0];

    if (is_prefix(command, "cont")) {
        continue_execution();
    } else if (is_prefix(command, "break")) {
        std::string addr {args[1], 2}; // removes first two characters, assuming user wrote 0xADDRESS
        set_breakpoint_at_address(std::stol(addr, nullptr, 16)); // stol with radix 16 converts address from string to hexadecimal integer
    } else {
        std::cerr << "Unknown command\n";
    }
}

void debugger::set_breakpoint_at_address(std::intptr_t addr) {
    std::cout << "Set breakpoint at address 0x" << std::hex << addr << std::endl;
    breakpoint bp {m_pid, addr};
    bp.enable();
    m_breakpoints[addr] = bp;
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

void debugger::continue_execution() {
    ptrace(PTRACE_CONT, m_pid, nullptr, nullptr);

    int wait_status;
    auto options = 0;
    waitpid(m_pid, &wait_status, options);
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