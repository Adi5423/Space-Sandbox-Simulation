#pragma once

#include <string>
#include <glm/glm.hpp>
#include "glad/glad.h"
#include <map>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextRenderer {
public:
    TextRenderer(unsigned int width, unsigned int height);
    ~TextRenderer();

    // Initialize and load the font
    bool init(const std::string& fontPath);
    
    // Render text
    void renderText(const std::string& text, float x, float y, float scale, const glm::vec3& color = glm::vec3(1.0f));
    
    // Update viewport dimensions
    void updateViewport(unsigned int width, unsigned int height);

private:
    // Render state
    unsigned int VAO, VBO;
    unsigned int shaderProgram;
    
    // Text rendering shader uniform locations
    int textColorLoc;
    int projectionLoc;
    
    // Character map
    std::map<char, Character> characters;
    
    // Viewport dimensions
    unsigned int viewportWidth;
    unsigned int viewportHeight;
    
    // Internal methods
    bool createShaders();
    void setupBuffers();
}; 