# AstroSandbox

A lightweight space simulation project that demonstrates basic orbital mechanics and 3D rendering.

## Prerequisites

- CMake (version 3.10 or higher)
- MinGW-w64 (for Windows)
- Git

## Project Structure
```
P1_B1/
├── build/           # Build output directory
├── include/         # Header files
├── lib/            # Libraries (GLFW, etc.)
├── src/            # Source files
├── shaders/        # GLSL shaders
├── tests/          # Test files
└── docs/           # Documentation
```

## Building the Project

### Windows

1. Clone the repository:
```bash
git clone <repository-url>
cd P1_B1
```

2. Build GLFW (first time only):
```bash
.\build_glfw.bat
```

3. Build the project:
```bash
.\build.bat
```

The executable will be created in the `build` directory.

### Running the Simulation

After building, run the executable:
```bash
.\build\astro-sim.exe
```

## Controls

- WASD: Move forward/backward, strafe left/right
- Space: Move up
- Left Control: Move down
- Left Shift: Hold to sprint (move faster)
- Mouse: Look around (when cursor is hidden)
- C: Toggle mouse cursor visibility
- ESC: Exit

## Dependencies

- GLFW 3.4
- GLM
- OpenGL
- GLAD

## Development Progress

See [PROGRESS.md](PROGRESS.md) for detailed development status and planned features.

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details. 