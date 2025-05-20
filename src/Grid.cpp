#include "Grid.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <cmath> // For std::abs

Grid::Grid(float size, float step) : gridSize(size), gridStep(step), needsUpdate(true) {
    // Create and compile shaders
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile, fShaderFile;
    
    vShaderFile.open("shaders/grid.vert");
    fShaderFile.open("shaders/grid.frag");
    
    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Failed to open shader files" << std::endl;
        // Exit or handle error appropriately, as without shaders, nothing will render
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
    
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    
    // Check for vertex shader compile errors
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(vertexShader);
        shaderProgram = 0; // Indicate shader loading failed
        // Exit or handle error
        return;
    }
    
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    
    // Check for fragment shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
        glDeleteShader(fragmentShader);
        glDeleteShader(vertexShader); // Also delete vertex shader if fragment fails
        shaderProgram = 0; // Indicate shader loading failed
        // Exit or handle error
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
        // Exit or handle error
        return;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    
    // Initialize VAOs and VBOs to 0 before generation
    gridVAO = 0;
    gridVBO = 0;
    axesVAO = 0;
    axesVBO = 0;

    // Generate grid and axes geometry (buffers are updated within these calls)
    generateGrid(size, step);
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
    generateGrid(gridSize, gridStep); // generateGrid calls updateBuffers
    generateAxes(); // generateAxes calls updateBuffers
}

void Grid::updateStep(float step) {
    gridStep = step;
    needsUpdate = true;
    generateGrid(gridSize, gridStep); // generateGrid calls updateBuffers
    generateAxes(); // generateAxes calls updateBuffers
}

void Grid::generateGrid(float size, float step) {
    gridVertices.clear();
    
    // Generate grid lines with a uniform grey color
    for (float i = -size; i <= size; i += step) {
        glm::vec3 color = glm::vec3(0.5f, 0.5f, 0.5f); // Grey color
        
        // Lines parallel to X-axis
        gridVertices.push_back(-size); gridVertices.push_back(0.0f); gridVertices.push_back(i);
        gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
        gridVertices.push_back(size); gridVertices.push_back(0.0f); gridVertices.push_back(i);
        gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
        
        // Lines parallel to Z-axis
        gridVertices.push_back(i); gridVertices.push_back(0.0f); gridVertices.push_back(-size);
        gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
        gridVertices.push_back(i); gridVertices.push_back(0.0f); gridVertices.push_back(size);
        gridVertices.push_back(color.r); gridVertices.push_back(color.g); gridVertices.push_back(color.b);
    }
    
    updateBuffers(); // Ensure buffers are updated after generating vertices
}

void Grid::generateAxes() {
    axesVertices.clear();
    
    // X-axis (Red)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(gridSize); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(1.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    
    // Y-axis (Green)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(gridSize); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(1.0f); axesVertices.push_back(0.0f);
    
    // Z-axis (Blue)
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(0.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(gridSize);
    axesVertices.push_back(0.0f); axesVertices.push_back(0.0f); axesVertices.push_back(1.0f);
    
    updateBuffers(); // Ensure buffers are updated after generating vertices
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

void Grid::render(const glm::mat4& view, const glm::mat4& projection) {
    if (shaderProgram == 0) return; // Don't render if shader loading failed

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
    
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(glm::mat4(1.0f)));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    
    // Render grid
    glLineWidth(1.0f);
    glBindVertexArray(gridVAO);
    glDrawArrays(GL_LINES, 0, gridVertices.size() / 6); // Note: size / 6 because each vertex has 3 pos + 3 color
    
    // Render axes with thicker lines
    glLineWidth(2.0f);
    glBindVertexArray(axesVAO);
    glDrawArrays(GL_LINES, 0, axesVertices.size() / 6); // Note: size / 6 because each vertex has 3 pos + 3 color
    
    // Disable blending
    glDisable(GL_BLEND);
} 