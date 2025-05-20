#include "Grid.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <cmath> // For std::abs

Grid::Grid(float size, float step) 
    : gridVAO(0), gridVBO(0),
      axesVAO(0), axesVBO(0),
      shaderProgram(0),
      cameraPosLoc(-1), gridSpacingLoc(-1),
      majorLineSpacingLoc(-1), majorLineColorLoc(-1),
      minorLineColorLoc(-1), fadeDistanceLoc(-1),
      gridVertices(), axesVertices(),
      gridSize(size), gridStep(step),
      currentGridSpacing(step), majorLineSpacing(10.0f),
      majorLineColor(0.7f, 0.7f, 0.7f, 0.5f),
      minorLineColor(0.5f, 0.5f, 0.5f, 0.3f),
      fadeDistance(100.0f),
      needsUpdate(true) {
    
    // Create and compile shaders
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    vShaderFile.open("shaders/grid.vert");
    fShaderFile.open("shaders/grid.frag");
    
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        shaderProgram = 0; // Indicate shader loading failed
        return;
    }
    
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    
    // Check for vertex shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        shaderProgram = 0; // Indicate shader loading failed
        return;
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    
    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader);
        shaderProgram = 0; // Indicate shader loading failed
        return;
    }
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
        glDeleteProgram(shaderProgram);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        shaderProgram = 0; // Indicate shader loading failed
        return;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Get uniform locations
    cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    gridSpacingLoc = glGetUniformLocation(shaderProgram, "gridSpacing");
    majorLineSpacingLoc = glGetUniformLocation(shaderProgram, "majorLineSpacing");
    majorLineColorLoc = glGetUniformLocation(shaderProgram, "majorLineColor");
    minorLineColorLoc = glGetUniformLocation(shaderProgram, "minorLineColor");
    fadeDistanceLoc = glGetUniformLocation(shaderProgram, "fadeDistance");
    
    // Generate grid and axes geometry
    generateGrid(step);
    generateAxes();
}

Grid::~Grid() {
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteVertexArrays(1, &axesVAO);
    glDeleteBuffers(1, &axesVBO);
    glDeleteProgram(shaderProgram);
}

void Grid::updateSize(float size) {
    gridSize = size;
    needsUpdate = true;
    generateGrid(gridStep);
    generateAxes();
}

void Grid::updateStep(float step) {
    gridStep = step;
    needsUpdate = true;
    generateGrid(gridStep);
    generateAxes();
}

void Grid::setMajorLineSpacing(float spacing) {
    majorLineSpacing = spacing;
    needsUpdate = true;
}

void Grid::setMajorLineColor(const glm::vec4& color) {
    majorLineColor = color;
    needsUpdate = true;
}

void Grid::setMinorLineColor(const glm::vec4& color) {
    minorLineColor = color;
    needsUpdate = true;
}

void Grid::setFadeDistance(float distance) {
    fadeDistance = distance;
    needsUpdate = true;
}

void Grid::setGridSpacing(float spacing) {
    currentGridSpacing = spacing;
    needsUpdate = true;
}

void Grid::updateCameraPosition(const glm::vec3& cameraPos) {
    updateGridSpacing(cameraPos);
}

void Grid::updateGridSpacing(const glm::vec3& cameraPos) {
    // Adjust grid spacing based on camera height
    float height = std::abs(cameraPos.y);
    float newSpacing = gridStep;
    
    if (height > 100.0f) {
        newSpacing = gridStep * 10.0f;
    } else if (height > 50.0f) {
        newSpacing = gridStep * 5.0f;
    } else if (height > 20.0f) {
        newSpacing = gridStep * 2.0f;
    }
    
    if (newSpacing != currentGridSpacing) {
        currentGridSpacing = newSpacing;
        needsUpdate = true;
    }
}

void Grid::generateGrid(float /*step*/) {
    gridVertices.clear();
    
    // Generate a quad that's centered on the camera's position
    // Using a fixed radius of 1000 units
    float quadSize = 1000.0f; // Radius around camera
    
    // Position and color for each vertex
    gridVertices = {
        // Position (x, y, z) and Color (r, g, b)
        -quadSize, 0.0f, -quadSize,  0.5f, 0.5f, 0.5f,
         quadSize, 0.0f, -quadSize,  0.5f, 0.5f, 0.5f,
         quadSize, 0.0f,  quadSize,  0.5f, 0.5f, 0.5f,
        -quadSize, 0.0f,  quadSize,  0.5f, 0.5f, 0.5f
    };
    
    updateBuffers();
}

void Grid::generateAxes() {
    axesVertices.clear();
    
    // Define the size of the world gizmo and colored axes
    float gizmoSize = 5.0f; // Size of the grey world gizmo
    float coloredAxesSize = 10.0f; // Size of the colored XYZ axes
    
    // Existing Grey Axes with Arrows
    // X-axis (Grey)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(gizmoSize - 0.5f); axesVertices.push_back(0.5f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(gizmoSize - 0.5f); axesVertices.push_back(-0.5f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(-0.5f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize - 0.5f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    // Y-axis (Grey)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.5f); axesVertices.push_back(gizmoSize - 0.5f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(-0.5f); axesVertices.push_back(gizmoSize - 0.5f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    
    axesVertices.push_back(-0.5f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize - 0.5f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    // Z-axis (Grey)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    axesVertices.push_back(0.5f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize - 0.5f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    axesVertices.push_back(-0.5f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize - 0.5f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    axesVertices.push_back(-0.5f); axesVertices.push_back(0.0f); axesVertices.push_back(gizmoSize - 0.5f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    // New Colored XYZ Axes
    // X-axis (Red)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // Origin
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // Red color
    axesVertices.push_back(coloredAxesSize); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // End of X axis
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // Red color

    // Y-axis (Green)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // Origin
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); // Green color
    axesVertices.push_back(0.0f); axesVertices.push_back(coloredAxesSize); axesVertices.push_back(0.0f); // End of Y axis
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); // Green color

    // Z-axis (Blue)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); // Origin
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); // Blue color
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(coloredAxesSize); // End of Z axis
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); // Blue color

    updateBuffers();
}

void Grid::updateBuffers() {
    // Update grid buffers
    if (gridVAO == 0) {
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);
    }
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Update axes buffers
    if (axesVAO == 0) {
        glGenVertexArrays(1, &axesVAO);
        glGenBuffers(1, &axesVBO);
    }
    
    glBindVertexArray(axesVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axesVBO);
    glBufferData(GL_ARRAY_BUFFER, axesVertices.size() * sizeof(float), axesVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    needsUpdate = false;
}

void Grid::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (shaderProgram == 0) return;

    if (needsUpdate) {
        updateBuffers();
    }

    glUseProgram(shaderProgram);
    
    // Enable blending for transparency
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Set uniforms
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    // Create a model matrix that centers the grid on the camera's XZ position
    // Snap to grid spacing to prevent jittering
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(
        std::floor(cameraPos.x / currentGridSpacing) * currentGridSpacing,
        0.0f,
        std::floor(cameraPos.z / currentGridSpacing) * currentGridSpacing
    ));
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Set custom uniforms
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));
    glUniform1f(gridSpacingLoc, currentGridSpacing);
    glUniform1f(majorLineSpacingLoc, majorLineSpacing);
    glUniform4fv(majorLineColorLoc, 1, glm::value_ptr(majorLineColor));
    glUniform4fv(minorLineColorLoc, 1, glm::value_ptr(minorLineColor));
    
    // Disable depth writing but keep depth testing
    glDepthMask(GL_FALSE);
    
    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    
    // Render grid quad
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    
    // Re-enable depth writing for axes
    glDepthMask(GL_TRUE);
    
    // Render axes with thicker lines
    glLineWidth(3.0f);
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, axesVertices.size() / 6);
    
    // Disable blending
    glDisable(GL_BLEND);
}

// Coordinate system functions
glm::vec3 Grid::snapToGrid(const glm::vec3& worldPos) const {
    // Project onto XZ plane
    float u = worldPos.x;
    float v = worldPos.z;

    // Compute cell indices
    int i = static_cast<int>(std::floor(u / currentGridSpacing));
    int j = static_cast<int>(std::floor(v / currentGridSpacing));

    // Reconstruct snapped position
    glm::vec3 snapped;
    snapped.x = i * currentGridSpacing;
    snapped.y = worldPos.y;  // Preserve original height
    snapped.z = j * currentGridSpacing;
    return snapped;
}

glm::ivec2 Grid::worldToGridIndex(const glm::vec3& worldPos) const {
    return glm::ivec2(
        static_cast<int>(std::floor(worldPos.x / currentGridSpacing)),
        static_cast<int>(std::floor(worldPos.z / currentGridSpacing))
    );
}

glm::vec3 Grid::gridIndexToWorld(const glm::ivec2& gridIndex) const {
    return glm::vec3(
        gridIndex.x * currentGridSpacing,
        0.0f,  // Default to ground plane
        gridIndex.y * currentGridSpacing
    );
} 