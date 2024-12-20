#ifndef COMPONENT_H
#define COMPONENT_H
#include <optional>
#include <cstdio>
#include <chrono>
#include <thread>

#include "terminal.h"

// Platform-specific includes
#ifdef _WIN32
    #include <conio.h>  // Windows-specific for _kbhit and _getch
#else
    #include <unistd.h>
    #include <termios.h>
    #include <fcntl.h>
#endif

namespace TerminalUI {
    class Renderer : public Terminal {
        // Platform-specific setup for non-blocking input
        void setupNonBlockingInput();

        // Restore terminal settings (POSIX only)
        void restoreInputSettings() const;

        // Member variable to store original terminal settings (POSIX)
        #ifndef _WIN32
            termios originalSettings{};
        #endif

        protected:
            // main logic of Renderer class, sets pixel in Terminal renderer pixel array
            virtual void setFrame();
            virtual void handleKeyboardInterrupts(int input);

            // Keyboard interrupts
            static bool isInputAvailable();
            static int readInput();

        public:
            explicit Renderer(std::optional<Dimension> dimensions = std::nullopt);

            /*
             *  default implementation of render method for most use cases,
             *  uses internal drawFrame to set pixels for new Frame, and draws them on screen in infinite loop
             *  also allows you to draw frame rate (for debugging purpose), or set specific frame rate (artificially)
             */
            [[noreturn]] virtual void render(bool showFrameRate = false, std::optional<unsigned int> frameRate = std::nullopt);

            virtual ~Renderer();
    };
}

#endif //COMPONENT_H
