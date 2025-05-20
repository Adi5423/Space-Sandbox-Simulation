#pragma once

#include "glad/glad.h"
#include <vector>
#include <glm/glm.hpp>

class Grid {
public:
    Grid(float size = 100.0f, float step = 1.0f);
    ~Grid();

    // Grid rendering
    void render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos);
    
    // Grid configuration
    void updateSize(float size);
    void updateStep(float step);
    void setMajorLineSpacing(float spacing);
    void setMajorLineColor(const glm::vec4& color);
    void setMinorLineColor(const glm::vec4& color);
    void setGridSpacing(float spacing);
    void setFadeDistance(float distance);
    
    // Coordinate system and grid snapping
    glm::vec3 snapToGrid(const glm::vec3& worldPos) const;
    glm::ivec2 worldToGridIndex(const glm::vec3& worldPos) const;
    glm::vec3 gridIndexToWorld(const glm::ivec2& gridIndex) const;
    float getGridSpacing() const { return currentGridSpacing; }
    
    // Camera-based updates
    void updateCameraPosition(const glm::vec3& cameraPos);
    void updateGridSpacing(const glm::vec3& cameraPos);

private:
    // OpenGL resources
    unsigned int gridVAO, gridVBO;
    unsigned int axesVAO, axesVBO;
    unsigned int shaderProgram;
    
    // Shader uniform locations
    int cameraPosLoc;
    int gridSpacingLoc;
    int majorLineSpacingLoc;
    int majorLineColorLoc;
    int minorLineColorLoc;
    int fadeDistanceLoc;
    
    // Grid data
    std::vector<float> gridVertices;
    std::vector<float> axesVertices;
    float gridSize;
    float gridStep;
    float currentGridSpacing;
    float majorLineSpacing;
    glm::vec4 majorLineColor;
    glm::vec4 minorLineColor;
    float fadeDistance;
    bool needsUpdate;
    
    // Internal methods
    void generateGrid(float step);
    void generateAxes();
    void updateBuffers();
}; 