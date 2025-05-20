#pragma once

// OpenGL includes (GLAD must be included before GLFW)
#include "glad/glad.h"
#include "GLFW/glfw3.h"

// Standard includes
#include <string>
#include <memory>
#include <vector>

// Project includes
#include "World.h"
#include "Camera.h"
#include "Mesh.h"

class Renderer {
public:
    Renderer(int width = 1280, int height = 720, const std::string& title = "AstroSandbox");
    ~Renderer();

    // Window management
    bool initialize();
    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();

    // Rendering
    void render(const World& world);
    void clear();

    // Getters
    GLFWwindow* getWindow() const { return window; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

private:
    // OpenGL setup
    bool initGL();
    bool createShaders();
    void setupMeshBuffers(const Mesh& mesh);

    // Rendering helpers
    void renderMesh(const Mesh& mesh, const Vector& position, const Vector& scale);
    void updateViewMatrix(const Camera& camera);
    void updateProjectionMatrix();

    GLFWwindow* window;
    int width, height;
    std::string title;

    // OpenGL objects
    GLuint shaderProgram;
    GLuint vao, vbo, ebo;
    
    // Shader locations
    GLint modelLoc, viewLoc, projLoc;
    GLint colorLoc;

    // Matrices
    float viewMatrix[16];
    float projMatrix[16];
}; 