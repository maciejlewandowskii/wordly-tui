#ifndef TERMINAL_H
#define TERMINAL_H
#include <optional>

namespace TerminalUI {

    enum class Color {
        NONE,
        WHITE,
        BLACK,
        RED,
        GREEN,
        BLUE,
        YELLOW
    };

    // ANSI escape codes for ForegroundColors
    constexpr const char* ForegroundColorCodes[] = {
        "", // None (Default color)
        "\033[37m", // WHITE
        "\033[30m", // BLACK
        "\033[31m", // RED
        "\033[32m", // GREEN
        "\033[34m", // BLUE
        "\033[33m"  // YELLOW
    };

    // ANSI escape codes for BackgroundColors
    constexpr const char* BackgroundColorCodes[] = {
        "", // None (Default color)
        "\033[47m", // WHITE
        "\033[40m", // BLACK
        "\033[41m", // RED
        "\033[42m", // GREEN
        "\033[44m", // BLUE
        "\033[43m"  // YELLOW
    };

    // Helper function to retrieve the corresponding escape code for the colors
    constexpr const char* get_foreground_escape_code(Color color) {
        return ForegroundColorCodes[static_cast<int>(color)];
    }
    constexpr const char* get_background_escape_code(Color color) {
        return BackgroundColorCodes[static_cast<int>(color)];
    }

    struct Pixel {
        // change of foreground color will only appear when rendering character, ('character' field needs to be set)
        Color foreground_color = Color::WHITE;
        Color background_color = Color::BLACK;
        // This option will render character instead of full pixel, character color will be set to foreground_color
        std::optional<char> character;

        // implementation to compare two Pixel struct (mainly for buffer purpose)
        bool operator!=(const Pixel& other) const {
            return foreground_color != other.foreground_color ||
                   background_color != other.background_color ||
                   character != other.character;
        }
    };

    struct Dimension {
        unsigned int height = 0;
        unsigned int width = 0;
    };

    struct Position {
        unsigned int y = 0;
        unsigned int x = 0;
    };

    class Terminal {
        mutable std::optional<Pixel>** buffer;
        mutable bool first_frame_drawn = false;

        const char* reset = "\033[0m";
        void draw_pixel(std::optional<Pixel> pixel) const;

        static void hideCursor();
        static void showCursor();

        protected:
            static Dimension get_terminal_dimensions();
            std::optional<Pixel>** pixels;

        public:
            // better to use this helper struct, because calling sizeof on array of pixels is inefficient (and it's really a pointer, so there are other issue)
            Dimension dimensions;

            explicit Terminal(Dimension dimensions = get_terminal_dimensions());

        virtual void setPixel(Pixel pixel, Position position) const;
            void draw() const;
            void fresh_draw() const;
            void clear() const;
            // only for deallocating memory
            virtual ~Terminal();
    };
} // TerminalUI

#endif //TERMINAL_H
