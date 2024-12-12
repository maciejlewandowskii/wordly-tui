cmake_minimum_required(VERSION 3.30)
add_compile_options(-include cstddef) # Compile option to make it work on MacOs
project(wordly_tui)

set(CMAKE_CXX_STANDARD 26)

add_executable(wordly_tui main.cpp
#        src/resources.cpp
        src/TerminalUI/terminal.h
        src/TerminalUI/terminal.cpp
#        src/TerminalUI/components/row_component.cpp
#        src/TerminalUI/components/row_component.h
)