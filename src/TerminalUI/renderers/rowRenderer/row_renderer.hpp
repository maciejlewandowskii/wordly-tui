#ifndef ROW_COMPONENT_H
#define ROW_COMPONENT_H

#include <map>
#include <vector>

#include "../../renderer.h"

namespace TerminalUI {
    enum class VerticalPosition {
        TOP,
        MIDDLE,
        BOTTOM
    };
    enum class HorizontalPosition {
        LEFT,
        CENTER,
        RIGHT
    };
    enum class RowAlignment {
        LEFT,
        CENTER,
        RIGHT
    };

    /*
     *  Struct for positioning row onto terminal
     *  for making it easier, it just supports handful predefined positions.
     *  Because rows will be rendered top to bottom depending on insert order (in their positions containers),
     *  to not allow for overlying, addRow() methods will use detectPositionOverlap() method to determinate,
     *  if new Row is not overlapping with different rows (mostly onto different positions) and also with terminal dimensions.
     */
    struct RowPosition {
        VerticalPosition vertical_position;
        HorizontalPosition horizontal_position;
        RowAlignment alignment;
    };

    /*
     *  This renderer simplify rendering onto terminal, instead of pixels you use rows
     *  with predefine position on screen, it makes it way easier to render simple text UI without
     *  the need for manually adding, removing pixels from screen. In a way its abstraction of pixel rendering.
     *  But it still allows you to insert a row of Pixels, and thanks for that you can insert multicolor
     *  characters or pixels anywhere on the screen.
     *  But if you need more control you can allways call setPixel() method, be aware that it doesn't have overlapping check!
     *  It also has component system that creates for you ready-to-use elements of UI, for example: list, frame etc...
     */
    template <typename T>
    class RowRenderer : Renderer {
        // implementation of drawFrame() method for RowRenderer
        void setFrame() override {
            // handle control flow
            executeAppControlFlow();

            // convert rows to pixels
            for (const auto& [group, rows] : grouped_rows) {
                if (rows.empty()) { continue; }
                // Find the row with the biggest pixel count
                unsigned int absolute_row_width;
                auto widest_row_iter = max_element(rows.begin(), rows.end(),
                    [](const auto& a, const auto& b) {
                        return std::get<0>(a).size() < std::get<0>(b).size(); // Compare pixel vector sizes
                    });

                if (widest_row_iter != rows.end()) {
                    const auto& pixels = std::get<0>(*widest_row_iter);
                    absolute_row_width = pixels.size();
                } else {
                    // if you can't find the widest row just panic
                    throw std::runtime_error("Can't find widest row, something go wrong!");
                }

                for (int row_idx = 0; row_idx < rows.size(); ++row_idx) {
                    const auto row = rows[row_idx];
                    for (int pixel = 0; pixel < std::get<0>(row).size(); ++pixel) {
                        // ReSharper disable once CppLocalVariableMightNotBeInitialized // it panics if can't find the widest row
                        Position position = convertRowPosition(std::get<0>(row), std::get<1>(row), absolute_row_width, rows.size());

                        // calculate offsets
                        position.x += pixel;
                        // rows with bottom vertical positions have different y-axis calculations
                        if (group.first == VerticalPosition::BOTTOM) {
                            // BOTTOM position rows
                            position.y -= rows.size() - row_idx;
                        }
                        else { position.y += row_idx; }

                        setPixel(std::get<0>(row)[pixel], position);
                    }
                }
            }
        }

        static Position convertRowPosition(std::vector<Pixel> row, RowPosition position, unsigned int absolute_row_width, unsigned int virtual_container_height) { // NOLINT(*-convert-member-functions-to-static)
            const Dimension terminal_dimensions = Terminal().dimensions;

            Position calculated_position;

            // calculate x-axis position
            switch (position.horizontal_position) {
                case HorizontalPosition::LEFT:
                    calculated_position.x = 0;
                    break;
                case HorizontalPosition::CENTER:
                    calculated_position.x = terminal_dimensions.width / 2;
                    break;
                case HorizontalPosition::RIGHT:
                    calculated_position.x = terminal_dimensions.width;
                    break;
            }
            // calculate y-axis position
            switch (position.vertical_position) {
                case VerticalPosition::TOP:
                    // 1 because most terminals need some padding on top
                    calculated_position.y = 1;
                    break;
                case VerticalPosition::MIDDLE:
                    calculated_position.y = terminal_dimensions.height / 2 - virtual_container_height / 2;
                    break;
                case VerticalPosition::BOTTOM:
                    calculated_position.y = terminal_dimensions.height;
                    break;
            }
            // calculate absolute (use case for this is to calculate alignment based on widest row) alignment
            switch (position.alignment) {
                case RowAlignment::LEFT:
                    switch (position.horizontal_position) {
                        case HorizontalPosition::LEFT:
                            // don't change anything
                            break;
                        case HorizontalPosition::CENTER:
                            // move left by half of widest row
                            calculated_position.x -= absolute_row_width / 2;
                            break;
                        case HorizontalPosition::RIGHT:
                            // move left by length of widest row
                            calculated_position.x -= absolute_row_width;
                            break;
                    }
                    break;
                case RowAlignment::CENTER:
                    switch (position.horizontal_position) {
                        case HorizontalPosition::LEFT:
                            // move right by (widest_row - current_row)/2
                            calculated_position.x += (absolute_row_width - row.size()) / 2;
                            break;
                        case HorizontalPosition::CENTER:
                            // move left by half of current row
                            calculated_position.x -= row.size() / 2;
                            break;
                        case HorizontalPosition::RIGHT:
                            // move left by widest row - (widest_row - current_row) / 2
                            calculated_position.x -= absolute_row_width - (absolute_row_width - row.size()) / 2;
                            break;
                    }
                    break;
                case RowAlignment::RIGHT:
                   switch (position.horizontal_position) {
                        case HorizontalPosition::LEFT:
                            // move right by widest_row - current_row
                            calculated_position.x += absolute_row_width - row.size();
                            break;
                        case HorizontalPosition::CENTER:
                            // move left by half of the widest_row - (widest_row - current_row)
                            calculated_position.x -= absolute_row_width / 2 - (absolute_row_width - row.size());
                            break;
                        case HorizontalPosition::RIGHT:
                            // move left by the length of current row
                            calculated_position.x -= row.size();
                            break;
                    }
                    break;
            }

            return calculated_position;
        }
        // implementation of drawFrame() method for RowRenderer
        void handleKeyboardInterrupts(int input) override {
            executeAppInterrupts(input);
        }

        // vector of rows that will be rendered (pixel array and position) [they are grouped by their position]
        std::map<std::pair<VerticalPosition, HorizontalPosition>, std::vector<std::tuple<std::vector<Pixel>, RowPosition>>> grouped_rows{};

        public:
            using AppControlFlowCallback = void (T::*)(RowRenderer*);
            using AppInterruptsCallback = void (T::*)(int);

            RowRenderer(T* instance, AppControlFlowCallback appControlFlow, AppInterruptsCallback appInterrupts, std::optional<Dimension> dimensions = std::nullopt): Renderer(dimensions), instance_(instance), appControlFlow_(appControlFlow), appInterrupts_(appInterrupts) {
                // Compile-time checks
                static_assert(std::is_member_function_pointer<AppControlFlowCallback>::value,
                              "appControlFlow must be a member function pointer with the correct signature");
                static_assert(std::is_member_function_pointer<AppInterruptsCallback>::value,
                              "appInterrupts must be a member function pointer with the correct signature");
            }

            // for more control, but WITHOUT overlapping check! (because Terminal object is private)
            void setPixel(Pixel pixel, Position position) const override {
                Renderer::setPixel(pixel, position);
            }

            // multiple addRow() methods for more user-friendly row inserting
            void addRow(std::string row, RowPosition position) { // NOLINT(*-convert-member-functions-to-static)
                std::vector<Pixel> pixels;
                pixels.resize(row.length());
                for (int i = 0; i < row.length(); i++) {
                    pixels[i] = Pixel {
                        Color::WHITE,
                        Color::NONE,
                        row[i]
                    };
                }
                auto key = std::make_pair(position.vertical_position, position.horizontal_position);
                grouped_rows[key].push_back(std::make_tuple(pixels, position));
            }
            void addRow(std::vector<Pixel> pixels, RowPosition position) { // NOLINT(*-convert-member-functions-to-static)
                auto key = std::make_pair(position.vertical_position, position.horizontal_position);
                grouped_rows[key].push_back(std::make_tuple(pixels, position));
            }

            void clearRows() {
                grouped_rows.clear();
            }
            void clearScreen() { // NOLINT(*-convert-member-functions-to-static)
                grouped_rows.clear();
                clear();
            }

            [[noreturn]] void render(bool showFrameRate, std::optional<unsigned int> frameRate = std::nullopt) override {
                Renderer::render(showFrameRate, frameRate);
            }

        private:
            // callback function to handle application logic (it will be called every frame, and it can interact with rendered content)
            T* instance_;                           // Pointer to the instance
            AppControlFlowCallback appControlFlow_; // Pointer to the appControlFlow member function
            AppInterruptsCallback appInterrupts_;   // Pointer to the appInterrupts member function

            // Callbacks that invoke the member functions
            void executeAppControlFlow() {
                if (instance_ && appControlFlow_) {
                    (instance_->*appControlFlow_)(this);
                }
            }

            void executeAppInterrupts(int input) {
                if (instance_ && appInterrupts_) {
                    (instance_->*appInterrupts_)(input);
                }
            }
    };
} // TerminalUI

#endif //ROW_COMPONENT_H
