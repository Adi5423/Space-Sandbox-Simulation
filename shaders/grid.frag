#version 330 core

in vec3 color;
in vec3 worldPos;

out vec4 FragColor;

uniform vec3 cameraPos;
uniform float gridSpacing;
uniform float majorLineSpacing = 10.0; // Every 10th line is a major line
uniform vec4 majorLineColor = vec4(0.8, 0.8, 0.8, 0.8); // Brighter color for major lines
uniform vec4 minorLineColor = vec4(0.6, 0.6, 0.6, 0.6); // Dimmer color for minor lines

void main() {
    // Calculate grid lines using world position
    vec2 grid = abs(fract(worldPos.xz / gridSpacing - 0.5) - 0.5) / fwidth(worldPos.xz / gridSpacing);
    float line = min(grid.x, grid.y);
    
    // Check if we're on a major line
    vec2 majorGrid = abs(fract(worldPos.xz / (gridSpacing * majorLineSpacing) - 0.5) - 0.5) / fwidth(worldPos.xz / (gridSpacing * majorLineSpacing));
    float majorLine = min(majorGrid.x, majorGrid.y);
    
    // Mix colors based on line type
    vec4 finalColor = mix(minorLineColor, majorLineColor, step(0.5, 1.0 - majorLine));
    
    // Apply line intensity
    float intensity = 1.0 - min(1.0, line);
    FragColor = finalColor * intensity;
    
    // Discard fragments that are too faint
    if (FragColor.a < 0.05) discard;
} 