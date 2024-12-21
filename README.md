# WordlyTUI

WordlyTUI is a terminal-based implementation of the Wordly game, featuring custom rendering and gameplay mechanics. This project was developed as a college project.

## Features

### Gameplay
- Classic Wordly game mechanics.
- Database of Wordly words that have been guessed in the past (source unknown but good enough).
- 5-letter English dictionary for validating words.
- Scoreboard functionality:
    - Results stored in `scoreboard.db`.
    - Displays top 10 results (scrolling not yet implemented).

### Terminal Rendering Engine
- Custom rendering engine located in the `TerminalUI` directory within `src/`.
- Simple and flexible rendering design:
    - Implements a buffer and essential rendering functions (`terminal.cpp`, `terminal.h`).
    - Allows custom renderer implementations by inheriting from the base (`renderer.cpp`, `renderer.h`).
- Prebuilt renderer (`row_renderer`):
    - Enables rendering rows of text or individual pixels anywhere on the screen.
    - It supports reusable UI components, such as a list.

### Releases
- Prebuilt binaries available for Windows, macOS, and Linux (Untested).

## Requirements

- [CMake](https://cmake.org/) (to compile the project).
- A C++ compiler (e.g., GCC, Clang, or MSVC).
- On Windows, Cygwin is required.

## Installation

1. Clone the repository:

   ```bash
   git clone <repository_url>
   cd WordlyTUI
   ```

2. Create a build directory and navigate to it:

   ```bash
   mkdir build && cd build
   ```

3. Run CMake to configure the project:

   ```bash
   cmake ..
   ```

4. Build the application:

   ```bash
   cmake --build .
   ```

## Usage

After building the application, run the executable generated in the build directory. For example:

```bash
./WordlyTUI
```

On Windows, use:

```bash
./WordlyTUI.exe
```
or just open the program, by double-clicking

## Notes

- On Windows, Cygwin is required due to its Unix API translation capabilities.
- Linux binaries are untested, so feedback is welcome.
- The rendering engine allows easy customization for other projects or enhanced UI components.
- The scoreboard displays only the top 10 results due to the lack of scrolling implementation in the renderer.
- Feel free to use or modify the code without asking for permission.

## Contributing

Contributions are welcome! If you have any ideas, questions, or improvements, feel free to reach out or submit a pull request.

## License

This project is open-source. Use the code as you wish!
