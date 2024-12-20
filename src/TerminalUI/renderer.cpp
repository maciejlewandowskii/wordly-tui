#include "renderer.h"

namespace TerminalUI {
    Renderer::Renderer(std::optional<Dimension> dimensions) : Terminal(dimensions.value_or(get_terminal_dimensions())) {
        setupNonBlockingInput(); // for handling keyboard interrupts (TODO: not really needed for all Renderers, make it optional)
    }

    [[noreturn]] void Renderer::render(bool showFrameRate, std::optional<unsigned int> frameRate) {
        std::optional<std::string> fps_str;
        // main render loop
        while (true) {
            std::chrono::high_resolution_clock::time_point frame_render_start_timestamp;
            if (showFrameRate || frameRate.has_value()) {
                frame_render_start_timestamp = std::chrono::high_resolution_clock::now();
            }

            // handle keyboard interrupts
            if (isInputAvailable()) {
                handleKeyboardInterrupts(readInput());
            }

            // set pixels from specific renderer, and draw them on screen
            setFrame();
            // print frame rate from previous calculation
            if (showFrameRate && fps_str.has_value()) {
                for (unsigned int i = 0; i < fps_str.value().length(); ++i) {
                    setPixel(TerminalUI::Pixel{
                                 TerminalUI::Color::BLACK,
                                 TerminalUI::Color::WHITE,
                                 fps_str.value()[i]
                             }, TerminalUI::Position{0, i});
                }
            }
            // draw pixels on the screen
            draw();

            // artificially slow down rendering to wanted framerate
            if (frameRate.has_value()) {
                const auto frame_render_end_timestamp = std::chrono::high_resolution_clock::now();
                const auto current_frame_render_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    frame_render_start_timestamp - frame_render_end_timestamp);

                // check if frame rate is not set too high (so basically there is no purpose in slowing it down more)
                if (current_frame_render_time.count() <= 1000000000 * frameRate.value() * 60) {
                    // if not slow it down (with real elapse time subtraction)
                    std::this_thread::sleep_for(
                        std::chrono::nanoseconds(
                            1000000000 / frameRate.value() * 60 - current_frame_render_time.count()));
                }
            }

            // displaying frame rate in left top corner (black on white)
            if (showFrameRate) {
                const auto frame_render_end_timestamp = std::chrono::high_resolution_clock::now();
                const auto current_frame_render_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
                    frame_render_end_timestamp - frame_render_start_timestamp);

                // calculate how many this(current) frames could be display in second
                const unsigned long int fps = 60 * 1000000000ULL / current_frame_render_time.count();

                /*
                 *  Add an FPS counter to the next frame. By doing this, we ensure that the actual time taken to render a frame is accurately measured.
                 *  This avoids not including the time spent rendering the FPS counter itself, which would otherwise create a delay between two frames
                 *  that is not accounted for in the calculation.
                 */
                fps_str = "FPS: " + std::to_string(fps);
            }
        }
    }

    Renderer::~Renderer() = default;

    // Private:
    void Renderer::setupNonBlockingInput() {
        #ifndef _WIN32
                tcgetattr(STDIN_FILENO, &originalSettings);  // Save original terminal settings
                termios newSettings = originalSettings;
                newSettings.c_lflag &= ~ICANON;  // Disable canonical mode
                newSettings.c_lflag &= ~ECHO;   // Disable echo
                tcsetattr(STDIN_FILENO, TCSANOW, &newSettings);

                setbuf(stdout, NULL);  // Disable stdout buffering
        #endif
    }
    void Renderer::restoreInputSettings() const {
        #ifndef _WIN32
            tcsetattr(STDIN_FILENO, TCSANOW, &originalSettings);  // Restore terminal settings
            fcntl(STDIN_FILENO, F_SETFL, 0); // Clear non-blocking flag
        #endif
    }


    // Protected:

    // Default implementation of setFrame(TODO: create default impl of setFrame() for renderers)
    void Renderer::setFrame() {}
    // Default implementation of handleKeyboardInterrupts(TODO: create default impl of handleKeyboardInterrupts() for renderers)
    void Renderer::handleKeyboardInterrupts(int input) {}

    bool Renderer::isInputAvailable() {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        struct timeval timeout = {0, 0};  // No blocking, immediate return
        int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        return (retval > 0 && FD_ISSET(STDIN_FILENO, &readfds));
    }
    int Renderer::readInput() {
        int c;
        if (read(STDIN_FILENO, &c, 4) > 0) {
            return c;
        }
        return -1;
    }
}
