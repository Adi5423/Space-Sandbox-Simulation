#pragma once

#include "glad/glad.h"
#include <glm/glm.hpp>
#include "Grid.h"
#include "TextRenderer.h"
#include "World.h"
#include "Mesh.h"
#include "Vector.h"
#include "Camera.h"

// Forward declarations
class World;
class Mesh;
class Vector;

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void render(const World& world);
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void clear();

private:
    int width;
    int height;
    std::string title = "AstroSandbox";
    
    // OpenGL resources
    unsigned int shaderProgram;
    int modelLoc;
    int viewLoc;
    int projLoc;
    int colorLoc;
    
    // Matrices
    float viewMatrix[16];
    float projMatrix[16];
    
    // Grid and text renderer
    Grid* grid;
    TextRenderer* textRenderer;
    
    // Internal methods
    bool createShaders();
    void updateViewMatrix(const Camera& camera);
    void updateViewMatrix(const glm::vec3& cameraPos);
    void updateProjectionMatrix();
    void renderMesh(const Mesh& mesh, const Vector& position, const Vector& scale);
}; 