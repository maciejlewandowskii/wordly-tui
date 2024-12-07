cmake_minimum_required(VERSION 3.30)
add_compile_options(-include cstddef) # Compile option to make it work on MacOs
project(wordly_tui)

set(CMAKE_CXX_STANDARD 26)

add_executable(wordly_tui main.cpp)

include(FetchContent)
set(FETCHCONTENT_UPDATES_DISCONNECTED TRUE)
FetchContent_Declare(ftxui
        GIT_REPOSITORY https://github.com/ArthurSonzogni/ftxui
        GIT_TAG main
)
FetchContent_MakeAvailable(ftxui)

target_link_libraries(wordly_tui
        PRIVATE ftxui::screen
        PRIVATE ftxui::dom
        PRIVATE ftxui::component
)