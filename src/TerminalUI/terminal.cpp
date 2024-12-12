#include "terminal.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <sys/ioctl.h>
#include <unistd.h>

namespace TerminalUI {
    Terminal::Terminal(const Dimension dimensions_) {
        // Hide cursor
        std::cout << "\033[?25l";

        dimensions = dimensions_;
        // set array size depending on dimensions provided in constructor parameters (for both pixels and buffer)
        pixels = new std::optional<Pixel>*[dimensions.height]; // set y-axis dimension
        buffer = new std::optional<Pixel>*[dimensions.height]; // set y-axis dimension
        for (int y = 0; y < dimensions.height; ++y) {
            pixels[y] = new std::optional<Pixel>[dimensions.width]; // set x-axis dimension
            buffer[y] = new std::optional<Pixel>[dimensions.width]; // set x-axis dimension
            // because 'std::optional' doesn't automatically assign 'std::nullopt' when initializing array we need to do it manually
            for (int x = 0; x < dimensions.width; ++x) {
                pixels[y][x] = std::nullopt;
                buffer[y][x] = std::nullopt;
            }
        }
    }

    void Terminal::setPixel(Pixel pixel, Position position) const {
        if (position.y >= dimensions.height || position.x >= dimensions.width) { throw::std::runtime_error("Pixel position is out of bounds!"); }
        pixels[position.y][position.x] = pixel;
    }

    void Terminal::draw() const {
        // check if first frame was drawn, if not just draw entire frame
        if (!first_frame_drawn)  { fresh_draw(); return; }

        // find changes in new frame (compare pixels with buffer)
        for (int y = 0; y < dimensions.height; ++y) {
            for (int x = 0; x < dimensions.width; ++x) {
                if (pixels[y][x] != buffer[y][x]) {
                    // here we can just swap pixel for new one (and update buffer)
                    buffer[y][x] = pixels[y][x];

                    // Save cursor position
                    std::cout << "\033[s";
                    // move cursor
                    std::cout << "\033[" << y << ";" << x << "H";
                    // print pixel
                    draw_pixel(pixels[y][x]);
                    // Restore cursor position
                    std::cout << "\033[u";
                }
            }
        }
    }

    void Terminal::fresh_draw() const {
        // save frame to buffer (and set fresh draw flag to true)
        first_frame_drawn = true;
        // Deep copy of pixels to buffer
        for (int y = 0; y < dimensions.height; ++y) {
            for (int x = 0; x < dimensions.width; ++x) {
                buffer[y][x] = pixels[y][x];
            }
        }

        // Save cursor position
        std::cout << "\033[s";

        for (int y = 0; y < dimensions.height; ++y) {
            for (int x = 0; x < dimensions.width; ++x) {
                draw_pixel(pixels[y][x]);
            }
            // when finish printing row (x-axis) go to next line
            std::cout << std::endl;
        }
    }

    void Terminal::clear() const {
        // Restore cursor position (for clearing terminal)
        std::cout << "\033[u";

        // fill all elements of 2-dimensional array with NullOpt
        for (int y = 0; y < dimensions.height; ++y) {
            for (int x = 0; x < dimensions.width; ++x) {
                pixels[y][x] = std::nullopt;
            }
        }
    }

    Terminal::~Terminal() {
        // Deallocate memory to prevent memory leaks
        for (int i = 0; i < sizeof(pixels); ++i) {
            delete[] pixels[i]; // deallocate inside array (x-axis)
        }
        delete[] pixels; // deallocate array itself (y-axis)

        // Shows cursor
        std::cout << "\033[?25h";
    }


    // Private:
    void Terminal::draw_pixel(const std::optional<Pixel> pixel) const {
        // if pixel is empty option, just output spacer
        if (pixel == std::nullopt) { std::cout << " " << reset; }
        else {
            // check if pixel has character to print
            if (pixel.value().character != std::nullopt) {
                // if true print character with background and foreground (character) color
                std::cout
                    << get_background_escape_code(pixel.value().background_color)
                    << get_foreground_escape_code(pixel.value().foreground_color)
                    << pixel.value().character.value()
                    << reset;
            }
            else {
                // if not, just print spacer with specified background color
                std::cout
                    << get_background_escape_code(pixel.value().background_color)
                    << " "
                    << reset;
            }
        }
    }

    Dimension Terminal::get_terminal_dimensions() {
        // get actual terminal dimensions
        winsize w{};
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == -1) { throw::std::runtime_error("Can't get terminal dimensions!"); }

        return { w.ws_row, w.ws_col };
    }
}
