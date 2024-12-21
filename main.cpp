#include "src/TerminalUI/renderers/rowRenderer/row_renderer.hpp"

#include "src/game.cpp"


[[noreturn]] int main(void) {
    std::vector<std::string> english_5_word_dictionary = {
         #include "src/resources/English.db"
    };
    // convert to lowercase, TODO: just convert dict to all lowercase
    for(int i = 0; i<english_5_word_dictionary.size(); i++){
        for(auto& c : english_5_word_dictionary[i])
        {
            c = tolower(c);
        }
    }

    std::vector<std::string> wordle_answers_list = {
         #include "src/resources/wordle-answers.db"
    };

    auto game = WordlyTUI(english_5_word_dictionary, wordle_answers_list);

    auto terminal_engine = TerminalUI::RowRenderer<WordlyTUI>(&game, &WordlyTUI::app_flow_control, &WordlyTUI::handle_interrupts);

    terminal_engine.render(true);
}