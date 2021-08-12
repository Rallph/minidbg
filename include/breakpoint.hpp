#ifndef MINIDBG_BREAKPOINT_HPP
#define MINIDBG_BREAKPOINT_HPP

#include <cstdint>
#include <sys/types.h> // original tutorial doesn't need to include this to get pid_t typedef, not sure why
#include <sys/ptrace.h>

namespace minidbg {
    class breakpoint {
        private:
            pid_t m_pid;
            std::intptr_t m_addr;
            bool m_enabled;
            uint8_t m_saved_data;

        public:
            breakpoint() = default;
            breakpoint(pid_t pid, std::intptr_t addr): m_pid{pid}, m_addr{addr}, m_enabled{false}, m_saved_data{} {}

            void enable() {
                // using PTRACE_PEEKDATA request returns the word at m_addr in the process with m_pid
                auto data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr);
                m_saved_data = static_cast<uint8_t>(data & 0xff); // save bottom byte (original instruction)
                uint64_t int3 = 0xcc; // int 3 instruction triggers SIGTRAP signal
                uint64_t data_with_int3 = ((data & ~0xff) | int3); // clear original instruction from word and replace with int3 instruction
                ptrace(PTRACE_POKEDATA, m_pid, m_addr, data_with_int3); // PTRACE_POKEDATA copies the data_with_int3 word into the address specified by m_addr

                m_enabled = true;
            }

            void disable() {
                auto data = ptrace(PTRACE_PEEKDATA, m_pid, m_addr, nullptr); // get word at breakpoint address
                auto restored_data = ((data & ~0xff) | m_saved_data); // clear int3 instruction from word and replace with saved original instruction
                ptrace(PTRACE_POKEDATA, m_pid, m_addr, restored_data); // copy restored word back into process memory

                m_enabled = false;
            }

            bool is_enabled() const { return m_enabled; }

            auto get_address() const -> std::intptr_t { return m_addr; } // not sure why arrow is used here. couldnt easily find by googling

    };
}

#endif