#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/ptrace.h>


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
    }

}