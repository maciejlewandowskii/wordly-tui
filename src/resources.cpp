#include <string>
#include <ranges>
#include <vector>
#include <bits/random.h>


namespace app {
     const char english_5_word_dictionary[] = {
         #include "resources/English.db"
     };

     std::string get_random_word() {
        // convert array of char to string
        std::string data(english_5_word_dictionary, sizeof(english_5_word_dictionary) - 1);

        // split string into vector (sadly I didn't find better solution)
        std::vector<std::string> words;
        for (auto part : std::views::split(data, ',')) {
            words.emplace_back(part.begin(), part.end());
        }

        // random generator setup (still no better way to do this...)
        std::random_device rd;
        std::mt19937 gen(rd());

        // get random index of word from vector
        std::uniform_int_distribution<> dis(0, words.size() - 1);


        return words[dis(gen)];
    }
}
