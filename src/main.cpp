#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Renderer.h"
#include "World.h"
#include "Simulator.h"
#include "ResourceManager.h"
#include "gui/MainWindow.h"
#include <QApplication>

// Global variables for window dimensions
int windowWidth = 800;
int windowHeight = 600;
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

// Global variables for camera control
static glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 10.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Initial direction vector pointing forward
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// Timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

int main(int argc, char* argv[]) {
    // Initialize Qt Application
    QApplication app(argc, argv);
    
    // Create and show the main window
    MainWindow mainWindow;
    mainWindow.show();

    // Initialize resource manager
    ResourceManager::initialize(argv[0]);

    // Create world and simulator
    World world;
    Simulator simulator(world);

    // Enter Qt event loop
    return app.exec();
} 