#version 330 core

in vec3 color;
out vec4 FragColor;

void main() {
    // Use the interpolated color from the vertex shader, with a fixed alpha for transparency.
    FragColor = vec4(color, 0.5); // Example: use vertex color with 50% opacity
} 