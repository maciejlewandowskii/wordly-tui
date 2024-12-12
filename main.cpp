#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

#include "src/TerminalUI/terminal.h"

[[noreturn]] int main(void) {

    auto tui = TerminalUI::Terminal();

    auto time_clock = std::chrono::high_resolution_clock::now();
    unsigned int animation_frame = 2;
    for (int i=0; true; ++i) {
        auto now = std::chrono::high_resolution_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - time_clock).count();
        int fps = i / diff;

        std::string fps_s = "FPS: " + std::to_string(fps);

        tui.clear();
        for (int x=0; x<fps_s.size(); ++x) {
            tui.setPixel(TerminalUI::Pixel {
                TerminalUI::ForegroundColor::RED,
                TerminalUI::BackgroundColor::BLACK,
               fps_s[x]
            }, TerminalUI::Position {tui.dimensions.height/2, tui.dimensions.width/2 + x });
        }
        if (animation_frame >= tui.dimensions.width) { animation_frame = 2; }
        tui.setPixel(TerminalUI::Pixel {
            TerminalUI::ForegroundColor::WHITE,
            TerminalUI::BackgroundColor::RED,
        }, TerminalUI::Position {tui.dimensions.height/2 + 20, animation_frame-2});
        tui.setPixel(TerminalUI::Pixel {
            TerminalUI::ForegroundColor::WHITE,
            TerminalUI::BackgroundColor::RED,
        }, TerminalUI::Position {tui.dimensions.height/2 + 20, animation_frame-1});
        tui.setPixel(TerminalUI::Pixel {
            TerminalUI::ForegroundColor::WHITE,
            TerminalUI::BackgroundColor::RED,
        }, TerminalUI::Position {tui.dimensions.height/2 + 20, animation_frame});
        tui.draw();
        animation_frame++;
    }

    return EXIT_SUCCESS;
}
