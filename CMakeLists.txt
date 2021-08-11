cmake_minimum_required(VERSION 3.21.1)

project(MiniDbg)

add_compile_options(-std=c++14)

include_directories(include)
add_executable(minidbg src/minidbg.cpp)

# libdwarf++ and libelf++ installed from libelfin via package manager
# liblinenoise installed via linenoise-git AUR package
target_link_libraries(minidbg liblinenoise libdwarf++ libelf++)