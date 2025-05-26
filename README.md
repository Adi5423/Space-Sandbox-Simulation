# Space Sandbox Simulation

A modern C++ application for simulating celestial bodies and their interactions in a 3D space environment.

## Features

- 🌌 3D space simulation with realistic physics
- 🎮 Interactive controls and real-time parameter adjustment
- 🖥️ Customizable, dockable user interface
- 📊 Debug visualization and performance monitoring
- 🎨 Modern OpenGL rendering with advanced effects
- 🔧 Extensible architecture for custom components

## Screenshots

[Add screenshots here]

## Requirements

- Windows 10 or later
- Qt6 (6.9.0)
- OpenGL 4.5+
- MinGW-w64 compiler
- CMake 3.10 or later

## Quick Start

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/Space-Sandbox-Simulation.git
   cd Space-Sandbox-Simulation
   ```

2. Run the setup script to download dependencies:
   ```bash
   setup.bat
   ```

3. Build the project:
   ```bash
   build.bat
   ```

4. Run the application:
   ```bash
   build/astro-sim.exe
   ```

## Project Structure

```
Space-Sandbox-Simulation/
├── src/                    # Source files
│   ├── gui/               # GUI components
│   └── ...                # Other source files
├── include/               # Header files
├── external/              # External dependencies
├── docs/                  # Documentation
├── build/                 # Build directory
├── CMakeLists.txt         # CMake configuration
├── build.bat             # Build script
└── setup.bat             # Setup script
```

## Documentation

- [Project Overview](docs/ProjectOverview.md) - Detailed technical documentation
- [User Guide](docs/UserGuide.md) - Usage instructions and features
- [Development Guide](docs/DevelopmentGuide.md) - Development setup and guidelines

## Features in Detail

### 3D Space Simulation
- Realistic gravitational interactions
- Orbital mechanics
- Collision detection
- Customizable physics parameters

### User Interface
- Dockable windows
- Customizable layouts
- Real-time controls
- Debug visualization

### Rendering System
- Modern OpenGL implementation
- Advanced shader effects
- Efficient resource management
- High-performance rendering pipeline

## Contributing

1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Known Issues

- 3D viewport rendering issues in main window
- Performance optimization needed for large simulations
- Some UI elements need refinement

## License

[Your License]

## Contact

[Your Contact Information]

## Acknowledgments

- Qt6 for the GUI framework
- OpenGL for 3D rendering
- GLM for mathematics
- ImGui for debug UI
- All contributors and supporters 