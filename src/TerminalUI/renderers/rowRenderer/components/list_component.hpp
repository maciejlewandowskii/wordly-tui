#pragma once
#include "../row_renderer.hpp"

static const std::vector<TerminalUI::Pixel> selected_prefix {
    TerminalUI::Pixel { TerminalUI::Color::BLUE, TerminalUI::Color::NONE, '-'},
    TerminalUI::Pixel { TerminalUI::Color::BLUE, TerminalUI::Color::NONE, '>'},
    TerminalUI::Pixel { TerminalUI::Color::NONE, TerminalUI::Color::NONE, std::nullopt},
    TerminalUI::Pixel { TerminalUI::Color::NONE, TerminalUI::Color::NONE, std::nullopt},
};

template<class T>
class ListComponent {
    TerminalUI::RowRenderer<T>* renderer;

    bool is_selectable{};
    unsigned int selected_index{};

    TerminalUI::RowPosition list_position{};

    std::vector<std::vector<TerminalUI::Pixel>> list{};


    public:
        explicit ListComponent(TerminalUI::RowRenderer<T> *row_renderer, TerminalUI::RowPosition position, bool selectable = false) {
            renderer = row_renderer;
            list_position = position;
            is_selectable = selectable;
        }

        void set_list(std::vector<std::vector<TerminalUI::Pixel>> list) {
            this->list = list;
        }
        void set_list(std::vector<std::string> list) {
            for (int i = 0; i < list.size(); i++) {
                std::vector<TerminalUI::Pixel> string_pixels;

                for (int c = 0; c < list[i].size(); c++) {
                    string_pixels.emplace_back(
                        TerminalUI::Pixel {
                        TerminalUI::Color::WHITE,
                        TerminalUI::Color::NONE,
                            list[i][c]
                    });
                }

                this->list.emplace_back(string_pixels);
            }
        }

        void select_next() {
            if (is_selectable && selected_index < list.size() - 1) { selected_index++; }
        }
        [[nodiscard]] std::string get_current_option() const {
            const std::vector<TerminalUI::Pixel> row = list[selected_index];

            std::string selected_option;
            for (int c = 0; c < row.size(); c++) {
                selected_option += row[c].character.value();
            }

            return selected_option;
        }
        [[nodiscard]] unsigned int get_selected_index() const { return selected_index; }
        void select_previous() {
            if (is_selectable && selected_index > 0) { selected_index--; }
        }

        void drawComponent() {
            for (int i = 0; i < list.size(); i++) {
                std::vector<TerminalUI::Pixel> row = list[i];

                // add prefix to current row, if list is selectable
                if (is_selectable && selected_index == i) {
                    row.insert(row.begin(), selected_prefix.begin(), selected_prefix.end());
                    // render but with left alignment
                    auto position_left = TerminalUI::RowPosition {
                        list_position.vertical_position,
                        list_position.horizontal_position,
                        TerminalUI::RowAlignment::LEFT
                    };

                    renderer->addRow(row, position_left);
                } else { renderer->addRow(row, list_position); }
            }
        }
};
