#pragma once

#include <vector>
#include "glad/glad.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Grid {
public:
    Grid(float size = 100.0f, float step = 1.0f);
    ~Grid();

    void render(const glm::mat4& view, const glm::mat4& projection);
    void updateSize(float size);
    void updateStep(float step);

private:
    void generateGrid(float size, float step);
    void generateAxes();
    void updateBuffers();

    GLuint gridVAO, gridVBO;
    std::vector<float> gridVertices;

    GLuint axesVAO, axesVBO;
    std::vector<float> axesVertices;

    // Shader program
    GLuint shaderProgram;
    
    // Grid properties
    float gridSize;
    float gridStep;
    bool needsUpdate;
}; 