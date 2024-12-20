#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -g") # for memory leaks debuging only
cmake_minimum_required(VERSION 3.30)
add_compile_options(-include cstddef) # Compile option to make it work on MacOs
project(wordly_tui)

set(CMAKE_CXX_STANDARD 26)

add_executable(wordly_tui
    main.cpp
    src/game.cpp

    # TerminalUI library
        src/TerminalUI/terminal.h
        src/TerminalUI/terminal.cpp
        src/TerminalUI/renderer.h
        src/TerminalUI/renderer.cpp
        # Renderers of TerminalUI lib (for now just 1)
            src/TerminalUI/renderers/rowRenderer/row_renderer.hpp
            # Components of RowRenderer
            src/TerminalUI/renderers/rowRenderer/components/list_component.hpp
)