#pragma once

#include <string>
#include <glm/glm.hpp>
#include "glad/glad.h"
#include <vector>

class TextRenderer {
public:
    TextRenderer(unsigned int width, unsigned int height);
    ~TextRenderer();

    // Initialize the text renderer
    bool init();
    
    // Render text
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color = glm::vec3(1.0f));
    
    // Calculate the width of a text string
    float getTextWidth(const std::string& text, float scale) const;
    
    // Update viewport dimensions
    void updateViewport(unsigned int width, unsigned int height);

private:
    // Render state
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
    unsigned int fontTexture;
    
    // Text rendering shader uniform locations
    int textColorLoc;
    int projectionLoc;
    
    // Viewport dimensions
    unsigned int viewportWidth;
    unsigned int viewportHeight;
    
    // Internal methods
    bool createShaders();
    void setupBuffers();
    void createFontTexture();
    
    // Character dimensions
    static constexpr int CHAR_WIDTH = 8;
    static constexpr int CHAR_HEIGHT = 8;
    static constexpr int CHARS_PER_ROW = 16;
    static constexpr int CHARS_PER_COL = 16;
}; 