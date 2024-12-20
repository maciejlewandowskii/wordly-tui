#include <iostream>
#include <random>

#include "TerminalUI/renderer.h"
#include "TerminalUI/renderers/rowRenderer/row_renderer.hpp"
#include "TerminalUI/renderers/rowRenderer/components/list_component.hpp"

enum class AppState {
    Loading,
    InMenu,
    InGame,
    GameOver, Congratulation,
    Settings,
    Scoreboard
};

static const std::string WordlyLogoASCII[] = {
    R"(  __      __                   __   ___                 )",
    R"( /\ \  __/\ \                 /\ \ /\_ \                )",
    R"( \ \ \/\ \ \ \    ___   _ __  \_\ \\//\ \    __  __     )",
    R"(  \ \ \ \ \ \ \  / __`\/\`'__\/'_` \ \ \ \  /\ \/\ \    )",
    R"(   \ \ \_/ \_\ \/\ \L\ \ \ \//\ \L\ \ \_\ \_\ \ \_\ \   )",
    R"(    \ `\___x___/\ \____/\ \_\\ \___,_\/\____\\/`____ \  )",
    R"(     '\/__//__/  \/___/  \/_/ \/__,_ /\/____/ `/___/> \ )",
    R"(                                                 /\___/ )",
    R"(                                                 \/__/  )"
};

class WordlyTUI {
    AppState game_state = AppState::Loading;
    unsigned int loading_bar_progress = 0;

    std::optional<ListComponent<WordlyTUI>> menu_list;

    std::vector<char> word_input{};
    std::string word_to_guess{};
    std::vector<std::string> words_to_guess{};
    std::vector<std::string> guessed_words{};

    std::vector<std::string> english_5_word_dictionary{};
    public:
        WordlyTUI(const std::vector<std::string> &english_5_word_dictionary, const std::vector<std::string> words_to_guess) {
            this->english_5_word_dictionary = english_5_word_dictionary;
            this->words_to_guess = words_to_guess;

            // chose random word to guess
            std::random_device random_device;
            std::mt19937 engine{random_device()};
            std::uniform_int_distribution<int> dist(0, this->words_to_guess.size() - 1);
            this->word_to_guess = words_to_guess[dist(engine)];
        }

        void app_flow_control(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) {
            switch (game_state) {
                case AppState::Loading:
                    render_loading_screen(row_renderer);
                    break;
                case AppState::InMenu:
                    render_in_menu_screen(row_renderer);
                    break;
                case AppState::GameOver:
                    row_renderer->clearScreen();
                    row_renderer->addRow("GAME OVER!", TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    row_renderer->addRow("You should have guess: " + word_to_guess, TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    row_renderer->addRow("", TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    row_renderer->addRow("try again later...", TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    break;
                case AppState::Congratulation:
                    row_renderer->clearScreen();
                    row_renderer->addRow("YOU WIN!", TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    row_renderer->addRow("You were able to guess word in " + std::to_string(guessed_words.size()) + " tries.", TerminalUI::RowPosition {TerminalUI::VerticalPosition::MIDDLE, TerminalUI::HorizontalPosition::CENTER, TerminalUI::RowAlignment::CENTER});
                    break;
                case AppState::InGame:
                    render_in_game_screen(row_renderer);
                    break;
                case AppState::Settings:
                    render_settings_screen(row_renderer);
                    break;
                case AppState::Scoreboard:
                    render_scoreboard_screen(row_renderer);
                    break;
            }
        }
        void handle_interrupts(int input) {
            switch (game_state) {
                case AppState::Loading:
                    break;
                case AppState::InMenu:
                    /* arrow down */ if (input == 4348699) { menu_list.value().select_next(); }
                    /* arrow up */ if (input == 4283163) { menu_list.value().select_previous(); }
                    /* enter */ if (input == 10) {
                        std::string selected = menu_list.value().get_current_option();
                        if (selected == "Play") { game_state = AppState::InGame; }
                        else if (selected == "Settings") { game_state = AppState::Settings; }
                        else if (selected == "Scoreboard") { game_state = AppState::Scoreboard; }
                        else if (selected == "Quit") { std::exit(EXIT_SUCCESS); }
                    }
                    break;
                case AppState::InGame:
                    /* letters a-z */ if (input >= 97 && input <= 122 && word_input.size() < 5) { word_input.emplace_back(input); }
                    /* backspace */ if (input == 127 && word_input.size() > 0) { word_input.pop_back(); }
                    /* enter */ if (input == 10 && word_input.size() == 5) {
                        const std::string current_word(word_input.begin(), word_input.end());
                        // check if word exists
                        if (std::count(english_5_word_dictionary.begin(), english_5_word_dictionary.end(), current_word) ) {
                            // add word as guess
                            guessed_words.emplace_back(current_word);
                            word_input.clear();

                            // check if user guess correctly
                            if (current_word == word_to_guess) { game_state = AppState::Congratulation; }

                            // too much guesses you lose
                            if (guessed_words.size() > 5) { game_state = AppState::GameOver; }
                        }
                    }
                    break;
                case AppState::Settings:
                    break;
                case AppState::Scoreboard:
                    break;
            }
        }

    protected:
        void render_loading_screen(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) {
            row_renderer->clearRows();
            for (const std::string row : WordlyLogoASCII) {
                row_renderer->addRow(row, TerminalUI::RowPosition {
                    TerminalUI::VerticalPosition::MIDDLE,
                    TerminalUI::HorizontalPosition::CENTER,
                    TerminalUI::RowAlignment::CENTER
                });
            }
            // padding (for now best way to do this, TODO: add some padding option to row renderer)
            for (auto i = 0; i < 3; ++i) {
                row_renderer->addRow("", TerminalUI::RowPosition {
                    TerminalUI::VerticalPosition::MIDDLE,
                    TerminalUI::HorizontalPosition::CENTER,
                    TerminalUI::RowAlignment::CENTER
                });
            }

            // Loading bar (yeah, its fake)
            std::vector<TerminalUI::Pixel> loading_bar_pixels;
            for (auto px = 0; px <= std::min(loading_bar_progress, static_cast<const unsigned int>(WordlyLogoASCII[0].length())); ++px) {
                loading_bar_pixels.emplace_back(
                    TerminalUI::Pixel {
                    TerminalUI::Color::NONE,
                    TerminalUI::Color::WHITE,
                    std::nullopt
                });
            }

            row_renderer->addRow(loading_bar_pixels, TerminalUI::RowPosition {
                TerminalUI::VerticalPosition::MIDDLE,
                TerminalUI::HorizontalPosition::CENTER,
                TerminalUI::RowAlignment::LEFT
            });

            usleep(150);
            loading_bar_progress++;

            if (loading_bar_progress >= WordlyLogoASCII[0].length()) {
                loading_bar_progress = 0;
                game_state = AppState::InMenu;
                row_renderer->clearScreen();
            }
        }

        void render_in_menu_screen(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) { // NOLINT(*-convert-member-functions-to-static)
            // initialize menu if not set
            if (!menu_list.has_value()) {
                menu_list = ListComponent<WordlyTUI>(row_renderer, TerminalUI::RowPosition {
                    TerminalUI::VerticalPosition::MIDDLE,
                    TerminalUI::HorizontalPosition::CENTER,
                    TerminalUI::RowAlignment::CENTER
                }, true);
                menu_list.value().set_list(std::vector<std::string> {
                    "Play",
                    "Settings",
                    "Scoreboard",
                    "Quit"
                });
            }

            row_renderer->clearScreen();

            menu_list->drawComponent();
        }

        void render_in_game_screen(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) const { // NOLINT(*-convert-member-functions-to-static)
            row_renderer->clearScreen();

            // Loop over 6 rows (max number of rows)
            for (int i = 0; i < 6; ++i) {
                std::vector<TerminalUI::Pixel> letters;

                if (i < guessed_words.size()) {
                    // Render guessed words with colors
                    for (int c = 0; c < 5; ++c) {
                        TerminalUI::Color font_color = TerminalUI::Color::WHITE;
                        char current_letter = guessed_words[i][c];

                        // Determine color based on word_to_guess
                        int letter_pos = word_to_guess.find(current_letter);
                        if (letter_pos != std::string::npos) {
                            font_color = (letter_pos == c) ? TerminalUI::Color::GREEN : TerminalUI::Color::YELLOW;
                        }

                        letters.emplace_back(TerminalUI::Pixel{
                            font_color,
                            TerminalUI::Color::NONE,
                            current_letter
                        });

                        // Add spacing between letters
                        if (c < 4) {
                            letters.emplace_back(TerminalUI::Pixel{
                                TerminalUI::Color::NONE,
                                TerminalUI::Color::NONE,
                                std::nullopt
                            });
                        }
                    }
                } else if (i == guessed_words.size()) {
                    // Display current input row
                    for (int c = 0; c < 5; ++c) {
                        letters.emplace_back(TerminalUI::Pixel{
                            TerminalUI::Color::WHITE,
                            TerminalUI::Color::NONE,
                            (c < word_input.size()) ? word_input[c] : '_'
                        });

                        // Add spacing between letters
                        if (c < 4) {
                            letters.emplace_back(TerminalUI::Pixel{
                                TerminalUI::Color::NONE,
                                TerminalUI::Color::NONE,
                                std::nullopt
                            });
                        }
                    }
                } else {
                    // Display empty row as placeholders
                    for (int c = 0; c < 5; ++c) {
                        letters.emplace_back(TerminalUI::Pixel{
                            TerminalUI::Color::WHITE,
                            TerminalUI::Color::NONE,
                            '_'
                        });

                        // Add spacing between letters
                        if (c < 4) {
                            letters.emplace_back(TerminalUI::Pixel{
                                TerminalUI::Color::NONE,
                                TerminalUI::Color::NONE,
                                std::nullopt
                            });
                        }
                    }
                }

                row_renderer->addRow(letters, TerminalUI::RowPosition{
                    TerminalUI::VerticalPosition::MIDDLE,
                    TerminalUI::HorizontalPosition::CENTER,
                    TerminalUI::RowAlignment::CENTER
                });
            }
        }

        void render_settings_screen(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) {

        }

        void render_scoreboard_screen(TerminalUI::RowRenderer<WordlyTUI> *row_renderer) {

        }
};
