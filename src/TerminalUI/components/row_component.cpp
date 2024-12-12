//
// Created by Maciej Lewandowski on 12/11/24.
//

#include "row_component.h"

namespace TerminalUI {
} // TerminalUI
#include <chrono>
#include <iostream>
#include <tuple>
#include <vector>

#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/screen.hpp"

struct CharProperties;
using Row = std::vector<std::tuple<char, ftxui::Color>>;

// This class is a wrapper around FTXUI, designed to simplify drawing rows at various positions in the terminal.
class Terminal {
    ftxui::Screen screen;
    // position to draw rows onto
    struct Position {
        enum class Vertical {
            TOP,
            CENTER,
            BOTTOM
        };
        enum class Horizontal {
            LEFT,
            CENTER,
            RIGHT
        };
    };
    Position position;
    // for purpose of this app, drawing only rows is more than enough
    std::vector<Row> rows;
    // fps counter (keep track when last time draw() was called)
    std::chrono::time_point<std::chrono::high_resolution_clock> FPS_chrono;

public:
    Terminal(): screen(ftxui::Screen::Create(ftxui::Dimension::Full(), ftxui::Dimension::Full()))   {}

    // setting the position that rows will be drawn onto
    void setPosition(const Position &pos) {
        position = pos;
    }

    void pushRow(const Row &row) {
        rows.push_back(row);
    }
    void editRow(const Row &row, const int row_index) {
        rows[row_index] = row;
    }
    void removeRow(const int row) {
        rows.erase(rows.begin() + row);
    }

    [[nodiscard]] std::tuple<int, int> get_terminal_center() const {
        return std::make_tuple(screen.dimx()/2, screen.dimy()/2);
    }

    std::tuple<int, int> find_center_offset() {
        // get number of rows to determinate relative center of y-axis
        unsigned int y_offset = rows.size() / 2;

        // get the longest "string" (longest vector) in rows vector to determinate relative center of x-axis
        auto biggest_vector = std::max_element(rows.begin(), rows.end(),
            [](const Row &a, const Row &b) {
                return a.size() < b.size();
            }
        );
        // make sure that biggest element was found (panic if it fails is fine)
        if (biggest_vector == rows.end()) { throw std::runtime_error("Can't find biggest element in vector!"); }
        unsigned int x_offset = biggest_vector->size() / 2;

        return std::make_tuple(x_offset, y_offset);
    }

    void draw(bool show_fps = false) {
        screen.Clear();

        if (show_fps) {
            FPS_chrono = std::chrono::high_resolution_clock::now();
            // draw fps counter (allways in left top corner)
            auto& fps_counter = screen.PixelAt(0, 0);
            screen.Loop
            fps_counter.character = 'FPS: ';
        }

        const std::tuple<int, int> terminal_center = get_terminal_center();
        const std::tuple<int, int> center_offset = find_center_offset();

        // determinate starting position for x and y axis's
        unsigned int x = std::get<0>(terminal_center) - std::get<0>(center_offset);
        unsigned int y = std::get<1>(terminal_center) - std::get<1>(center_offset);

        // so for now all rows will be aligned to left
        for(const Row& row : rows) {
            for (auto &character : row) {
                auto& pixel = screen.PixelAt(x, y);
                pixel.character = std::get<0>(character);
                pixel.foreground_color = std::get<1>(character);

                x++;
            }
            y++;
        }

        // draw content to terminal
        std::cout << screen.ToString();
    }
};