#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include "Renderer.h"
#include "World.h"
#include "Simulator.h"

// Global variables for window dimensions
int windowWidth = 800;
int windowHeight = 600;
glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 1000.0f);

// Global variables for camera control
static double lastX = windowWidth / 2.0;
static double lastY = windowHeight / 2.0;
static bool firstMouse = true;
static float yaw = -90.0f; // Yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right
static float pitch = 0.0f;
static glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 10.0f);
static glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); // Initial direction vector pointing forward
static glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
static float baseCameraSpeed = 2.5f; // Base speed
static float cameraSpeed = baseCameraSpeed; // Current speed
static float sprintMultiplier = 5.0f; // Sprint speed multiplier
static float cameraSensitivity = 0.1f;
static float speedMultiplier = 1.0f;
static bool eKeyPressed = false;
static bool qKeyPressed = false;

// Timing
float deltaTime = 0.0f; // Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

// Global variables for cursor state
static bool cursorHidden = true;
static bool cKeyPressed = false; // To track if C was pressed in the last frame

// Callback function for window resize
void framebufferSizeCallback(GLFWwindow* /*window*/, int width, int height) {
    glViewport(0, 0, width, height);
    windowWidth = width;
    windowHeight = height;
    projection = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 1000.0f);
}

// Mouse callback function
void mouseCallback(GLFWwindow* /*window*/, double xpos, double ypos) {
    if (!cursorHidden) return; // Only process mouse movement when cursor is hidden

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    xoffset *= cameraSensitivity;
    yoffset *= cameraSensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Update cameraFront vector using Euler angles
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Create window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "AstroSandbox", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        glfwTerminate();
        return -1;
    }

    // Create renderer
    Renderer renderer(window);

    // Create world and simulator
    World world;
    Simulator simulator(world);

    // Mouse callback setup
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Per-frame time logic
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Handle input
        float currentMoveSpeed = baseCameraSpeed * speedMultiplier;
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            currentMoveSpeed *= sprintMultiplier;
        }
        float velocity = currentMoveSpeed * deltaTime;

        // Speed control with E and Q
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && !eKeyPressed) {
            speedMultiplier *= 1.5f;
            eKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_RELEASE) {
            eKeyPressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && !qKeyPressed) {
            speedMultiplier /= 1.5f;
            qKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_RELEASE) {
            qKeyPressed = false;
        }

        // Movement controls
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += velocity * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= velocity * cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * velocity;
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            cameraPos += velocity * cameraUp;
        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
            cameraPos -= velocity * cameraUp;

        // Toggle cursor visibility
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !cKeyPressed) {
            cursorHidden = !cursorHidden;
            if (cursorHidden) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                // Recenter mouse when hiding to prevent sudden jumps
                glfwSetCursorPos(window, windowWidth / 2.0, windowHeight / 2.0);
                lastX = windowWidth / 2.0;
                lastY = windowHeight / 2.0;
            } else {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            }
            cKeyPressed = true;
        }
        if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE) {
            cKeyPressed = false;
        }

        // Calculate view matrix using cameraFront
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

        // Render
        renderer.render(view, projection, cameraPos);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glfwTerminate();
    return 0;
} 