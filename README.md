# Chip8 Emulator (README and PROJECT are WIP !!!)

This project is a simple Chip8 emulator implemented in C++. Chip8 is an interpreted programming language used in vintage computers, primarily video game consoles, in the late 1970s and early 1980s. This emulator allows you to run Chip8 programs on modern computers.

## Project Overview

The project consists of the following files:

- `main.cpp`: Contains the main function where the emulator is initialized and run.
- `chip8.cpp`: Contains the implementation of the `Chip8` class, which represents the Chip8 emulator.
- `chip8.h`: Contains the declaration of the `Chip8` class and other necessary declarations.

## Dependencies

The project depends on the Simple DirectMedia Layer (SDL) library for handling graphics and user input. Make sure you have SDL2 installed on your system before building the project.

## Building Instructions

To build the project, you can use the provided Makefile. Make sure you have `g++` installed on your system.

```bash
make chip8
```

This will compile the source files and link them together to produce an executable named `run`.

To clean up the compiled files, you can use:

```bash
make clean
```

## Usage

After building the project, you can run the emulator by executing the generated executable:

```bash
./run
```

The emulator will start, and you will see a window representing the Chip8 display. You can then load Chip8 ROMs into the emulator and interact with them using the keyboard.

## Chip8 ROMs

You can find various Chip8 ROMs available online. These ROMs contain programs and games written in the Chip8 programming language. Place the ROM files in the same directory as the executable (`run`) and load them into the emulator from the user interface.

(Some of the roms used are included in the repo under /roms)

## Controls

The keyboard layout for controlling the emulator is as follows:

```
Chip8 Keypad      Keyboard
+-+-+-+-+        +-+-+-+-+
|1|2|3|C|        |1|2|3|4|
+-+-+-+-+   =>   +-+-+-+-+
|4|5|6|D|        |Q|W|E|R|
+-+-+-+-+        +-+-+-+-+
|7|8|9|E|        |A|S|D|F|
+-+-+-+-+        +-+-+-+-+
|A|0|B|F|        |Z|X|C|V|
+-+-+-+-+        +-+-+-+-+
```

## Contributing

Contributions are welcome! If you find any issues or have suggestions for improvements, feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
