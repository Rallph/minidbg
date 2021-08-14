#ifndef MINIDBG_DEBUGGER_HPP
#define MINIDBG_DEBUGGER_HPP

#include <utility>
#include <string>
#include <linux/types.h>
#include <unordered_map>
#include <libelfin/dwarf/dwarf++.hh>
#include <libelfin/elf/elf++.hh>
#include <fcntl.h>

#include "breakpoint.hpp"

namespace minidbg {
    class debugger {
        private:
            std::string m_prog_name;
            pid_t m_pid;
            std::unordered_map<std::intptr_t, breakpoint> m_breakpoints;
            dwarf::dwarf m_dwarf;
            elf::elf m_elf;

            void handle_command(const std::string& line);
            void continue_execution();

            auto read_memory(uint64_t address) -> uint64_t;
            void write_memory(uint64_t address, uint64_t value);

            auto get_pc() -> uint64_t;
            void set_pc(uint64_t);
            void step_over_breakpoint();
            void wait_for_signal();

            auto get_function_from_pc(uint64_t pc) -> dwarf::die;

        public:
            debugger(std::string prog_name, pid_t pid)
                : m_prog_name{std::move(prog_name)}, m_pid{pid} {
                    auto fd = open(m_prog_name.c_str(), O_RDONLY); // use open bc elf loader needs unix file descriptor for mmap

                    m_elf = elf::elf{elf::create_mmap_loader(fd)};
                    m_dwarf = dwarf::dwarf{dwarf::elf::create_loader(m_elf)};
                }
            void run();
            void set_breakpoint_at_address(std::intptr_t addr);
            void dump_registers();
    };
}

#endif