# Mandelbrot CUDA

This project is a CUDA-accelerated Mandelbrot set visualizer implemented in C++. The primary objective of this project is to provide a platform for practicing CUDA programming while exploring various fun ideas associated with fractal visualization.

## Project Overview

The project comprises several files, each serving a distinct purpose:

- `main.cpp`: Main entry point of the application where the Mandelbrot set visualization is initialized and managed.
- `mandelbrot.cu`: CUDA kernel implementation for calculating Mandelbrot set fractals efficiently using GPU parallelization.
- `hud.cpp` and `hud.h`: Implementation and declaration of the Heads-Up Display (HUD) functionality, allowing users to interact with and control the Mandelbrot visualization.
- `utils.cpp` and `utils.h`: Utility functions used across the project for tasks such as handling user input and managing display settings.

## Dependencies

The project relies on both CUDA and SFML libraries:

- **CUDA**: The NVIDIA CUDA Toolkit is essential for GPU parallelization and computation.
- **SFML**: The Simple and Fast Multimedia Library (SFML) is used for graphical output and user interaction. Make sure to link the appropriate SFML libraries based on your platform (Windows or Linux).

## Building Instructions

### On Windows

- Install CUDA Toolkit, SFML, and Visual Studio with MSVC compiler.
- Ensure the required SFML libraries are linked dynamically or statically as per your preference.
- Set the appropriate compiler flags, such as `SFML_STATIC` for static linking if required.
- Compile the project using Visual Studio or any preferred IDE, ensuring all necessary dependencies are correctly configured.

### On Linux

- Install CUDA Toolkit and SFML on your system.
- Compile the project using appropriate compiler commands or provided build scripts.
- Ensure dynamic linking with SFML libraries, as static linking is generally discouraged on Linux.

## Usage

After successfully building the project, you can run the Mandelbrot visualization application. Depending on your platform and build settings, execute the generated executable with appropriate command-line arguments or flags.

The application provides various controls for navigating and interacting with the Mandelbrot set visualization:

- **Move**: Use arrow keys, WSAD, or mouse clicks to navigate within the fractal space.
- **Zoom In/Out**: Adjust zoom level using mouse wheel or `+/-` keys.
- **Increase/Decrease Max Iteration**: Modify maximum iteration count using `E/Q` keys.
- **Reset**: Reset the visualization to its initial state using `R` key.
- **Hide/Show HUD**: Toggle visibility of the Heads-Up Display using `H` key.

## Contributing

Contributions to the project are encouraged and welcomed. If you have any ideas for improvements, encounter issues, or wish to suggest new features, feel free to open an issue or submit a pull request on the project's repository.

## License

This project is licensed under [MIT License](LICENSE). Feel free to use, modify, and distribute the code for both personal and commercial purposes, with appropriate attribution to the original authors.
