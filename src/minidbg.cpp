#include <iostream>
#include <unistd.h>

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