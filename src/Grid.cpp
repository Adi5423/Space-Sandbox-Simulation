#include "Grid.h"
#include "ResourceManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <cmath> // For std::abs

Grid::Grid(float size, float step) 
    : gridVAO(0), gridVBO(0),
      shaderProgram(0),
      cameraPosLoc(-1), gridSpacingLoc(-1),
      majorLineSpacingLoc(-1), majorLineColorLoc(-1),
      minorLineColorLoc(-1), fadeDistanceLoc(-1),
      gridVertices(),
      gridSize(size), gridStep(step),
      currentGridSpacing(step), majorLineSpacing(10.0f),
      majorLineColor(0.7f, 0.7f, 0.7f, 0.5f),
      minorLineColor(0.5f, 0.5f, 0.5f, 0.3f),
      fadeDistance(100.0f),
      needsUpdate(true) {
    
    // Create and compile shaders
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    vShaderFile.open(ResourceManager::getResourcePath("shaders/grid.vert"));
    fShaderFile.open(ResourceManager::getResourcePath("shaders/grid.frag"));
    
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
    
    // Get uniform locations for grid shader
    cameraPosLoc = glGetUniformLocation(shaderProgram, "cameraPos");
    gridSpacingLoc = glGetUniformLocation(shaderProgram, "gridSpacing");
    majorLineSpacingLoc = glGetUniformLocation(shaderProgram, "majorLineSpacing");
    majorLineColorLoc = glGetUniformLocation(shaderProgram, "majorLineColor");
    minorLineColorLoc = glGetUniformLocation(shaderProgram, "minorLineColor");
    fadeDistanceLoc = glGetUniformLocation(shaderProgram, "fadeDistance");
    
    // Generate grid geometry
    generateGrid(step);
}

Grid::~Grid() {
    glDeleteVertexArrays(1, &gridVAO);
    glDeleteBuffers(1, &gridVBO);
    glDeleteProgram(shaderProgram);
}

void Grid::updateSize(float size) {
    gridSize = size;
    needsUpdate = true;
    generateGrid(gridStep);
}

void Grid::updateStep(float step) {
    gridStep = step;
    needsUpdate = true;
    generateGrid(gridStep);
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

void Grid::updateBuffers() {
    // Update grid buffers
    if (gridVAO == 0) {
        glGenVertexArrays(1, &gridVAO);
        glGenBuffers(1, &gridVBO);
    }
    
    glBindVertexArray(gridVAO);
    glBindBuffer(GL_ARRAY_BUFFER, gridVBO);
    glBufferData(GL_ARRAY_BUFFER, gridVertices.size() * sizeof(float), gridVertices.data(), GL_STATIC_DRAW);
    
    // Position attribute for grid shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color attribute for grid shader
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // Unbind VAO and VBO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    needsUpdate = false;
}

void Grid::render(const glm::mat4& view, const glm::mat4& projection, const glm::vec3& cameraPos) {
    if (shaderProgram == 0) return; // Cannot render grid without grid shader

    if (needsUpdate) {
        updateBuffers();
    }

    // Enable depth testing and blending
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Render grid
    glUseProgram(shaderProgram);
    glBindVertexArray(gridVAO);
    
    // Set uniforms for grid shader
    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    GLint projLoc = glGetUniformLocation(shaderProgram, "projection");
    
    // Create a model matrix that centers the grid on the camera's XZ position
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(
        std::floor(cameraPos.x / currentGridSpacing) * currentGridSpacing,
        0.0f,
        std::floor(cameraPos.z / currentGridSpacing) * currentGridSpacing
    ));
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Set custom uniforms for grid shader
    glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));
    glUniform1f(gridSpacingLoc, currentGridSpacing);
    glUniform1f(majorLineSpacingLoc, majorLineSpacing);
    glUniform4fv(majorLineColorLoc, 1, glm::value_ptr(majorLineColor));
    glUniform4fv(minorLineColorLoc, 1, glm::value_ptr(minorLineColor));
    
    // Disable depth writing but keep depth testing for grid
    glDepthMask(GL_FALSE);
    
    // Render grid quad
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Cleanup
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glBindVertexArray(0);
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