# Space Sandbox Simulation - User Guide

## Getting Started

### Installation
1. Download the latest release
2. Run `setup.bat` to install dependencies
3. Run `build.bat` to build the application
4. Launch `astro-sim.exe`

### First Launch
1. The application will open with the default layout
2. The main viewport will be in the center
3. Dockable windows will be arranged around the viewport

## User Interface

### Main Window
- **Viewport**: Central 3D view of the simulation
- **Menu Bar**: File, Edit, View, and Help menus
- **Tool Bar**: Quick access to common functions
- **Status Bar**: Shows simulation status and FPS

### Dockable Windows

#### Simulation Controls
- Start/Stop simulation
- Adjust simulation speed
- Reset simulation
- Save/Load scenarios

#### Body Properties
- Add new bodies
- Edit body properties
  - Mass
  - Position
  - Velocity
  - Size
  - Color
- Delete bodies

#### View Settings
- Camera controls
- Grid settings
- Lighting options
- View mode selection

#### Debug Information
- Performance metrics
- Physics calculations
- Memory usage
- Error logs

## Controls

### Camera Controls
- **WASD**: Move camera
- **Mouse**: Look around
- **Q/E**: Roll camera
- **Shift**: Move faster
- **Ctrl**: Move slower
- **Space**: Move up
- **C**: Toggle cursor

### Viewport Controls
- **Left Click + Drag**: Rotate view
- **Right Click + Drag**: Pan view
- **Mouse Wheel**: Zoom
- **F**: Focus on selected body
- **G**: Toggle grid
- **L**: Toggle lighting

### Window Management
- **Drag Title Bar**: Move window
- **Drag Edge**: Resize window
- **Double Click Title**: Maximize/Restore
- **Right Click Title**: Window menu

## Features

### Simulation
1. **Adding Bodies**
   - Click "Add Body" in Body Properties
   - Set initial properties
   - Click "Create"

2. **Editing Bodies**
   - Select body in viewport
   - Modify properties in Body Properties
   - Changes apply immediately

3. **Running Simulation**
   - Click "Start" in Simulation Controls
   - Adjust speed as needed
   - Use "Pause" to freeze simulation

### Customization

#### Window Layout
1. **Default Layout**
   - Viewport in center
   - Controls on right
   - Properties on left
   - Debug at bottom

2. **Custom Layout**
   - Drag windows to desired positions
   - Dock windows together
   - Save layout for future use

3. **Layout Management**
   - Save current layout
   - Load saved layout
   - Reset to default

#### View Settings
1. **Camera**
   - First person
   - Orbit
   - Free look
   - Follow body

2. **Grid**
   - Show/hide
   - Change size
   - Change color
   - Change opacity

3. **Lighting**
   - Ambient light
   - Directional light
   - Point lights
   - Shadows

## Tips and Tricks

### Performance
- Reduce number of bodies for better performance
- Lower quality settings for large simulations
- Use debug window to monitor performance

### Visualization
- Use different view modes for different purposes
- Enable grid for better spatial reference
- Use lighting to highlight important features

### Workflow
- Save frequently
- Use keyboard shortcuts
- Customize layout for your needs
- Use debug tools when needed

## Troubleshooting

### Common Issues
1. **Rendering Issues**
   - Update graphics drivers
   - Check OpenGL version
   - Reduce quality settings

2. **Performance Issues**
   - Reduce number of bodies
   - Lower quality settings
   - Close other applications

3. **UI Issues**
   - Reset window layout
   - Check screen resolution
   - Update application

### Getting Help
- Check documentation
- Search known issues
- Contact support
- Submit bug report

## Advanced Features

### Scripting
- Custom body behaviors
- Automated scenarios
- Physics modifications
- Custom rendering

### Data Export
- Save simulation data
- Export trajectories
- Generate reports
- Create visualizations

### Customization
- Custom shaders
- Custom physics
- Custom UI themes
- Custom layouts 