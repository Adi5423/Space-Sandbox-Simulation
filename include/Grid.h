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

    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    void updateSize(float size);
    void updateStep(float step);
    void updateCameraPosition(const glm::vec3& cameraPos);
    
    // Grid customization
    void setMajorLineSpacing(float spacing);
    void setMajorLineColor(const glm::vec4& color);
    void setMinorLineColor(const glm::vec4& color);
    void setFadeDistance(float distance);
    void setGridSpacing(float spacing);

private:
    void generateGrid(float step);
    void updateBuffers();
    void updateGridSpacing(const glm::vec3& cameraPos);

    // OpenGL objects
    unsigned int gridVAO, gridVBO;
    unsigned int shaderProgram;
    
    // Shader uniform locations
    int cameraPosLoc;
    int gridSpacingLoc;
    int majorLineSpacingLoc;
    int majorLineColorLoc;
    int minorLineColorLoc;
    int fadeDistanceLoc;
    
    // Vertex data
    std::vector<float> gridVertices;
    
    // Grid properties
    float gridSize;
    float gridStep;
    float currentGridSpacing;
    float majorLineSpacing;
    glm::vec4 majorLineColor;
    glm::vec4 minorLineColor;
    float fadeDistance;
    bool needsUpdate;
}; 