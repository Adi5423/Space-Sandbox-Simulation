#include "Mesh.h"
#include <cmath>

Mesh Mesh::createSphere(double radius, int segments) {
    Mesh sphere;
    sphere.setName("Sphere");
    
    // Generate vertices
    for (int i = 0; i <= segments; i++) {
        double phi = M_PI * i / segments;
        for (int j = 0; j <= segments; j++) {
            double theta = 2 * M_PI * j / segments;
            
            // Calculate vertex position
            double x = radius * sin(phi) * cos(theta);
            double y = radius * cos(phi);
            double z = radius * sin(phi) * sin(theta);
            
            // Create vertex
            Vertex v;
            v.position = Vector(x, y, z);
            v.normal = Vector(x, y, z).normalize();
            v.texCoord = Vector(j / (double)segments, i / (double)segments, 0);
            
            sphere.addVertex(v);
        }
    }
    
    // Generate triangles
    for (int i = 0; i < segments; i++) {
        for (int j = 0; j < segments; j++) {
            int v1 = i * (segments + 1) + j;
            int v2 = v1 + 1;
            int v3 = (i + 1) * (segments + 1) + j;
            int v4 = v3 + 1;
            
            // First triangle
            Triangle t1 = {v1, v2, v3};
            sphere.addTriangle(t1);
            
            // Second triangle
            Triangle t2 = {v2, v4, v3};
            sphere.addTriangle(t2);
        }
    }
    
    return sphere;
}

Mesh Mesh::createCube(double size) {
    Mesh cube;
    cube.setName("Cube");
    
    // Define vertices (8 corners of the cube)
    double s = size / 2.0;
    Vertex vertices[] = {
        {Vector(-s, -s, -s), Vector(-1, -1, -1), Vector(0, 0, 0)},
        {Vector(s, -s, -s), Vector(1, -1, -1), Vector(1, 0, 0)},
        {Vector(s, s, -s), Vector(1, 1, -1), Vector(1, 1, 0)},
        {Vector(-s, s, -s), Vector(-1, 1, -1), Vector(0, 1, 0)},
        {Vector(-s, -s, s), Vector(-1, -1, 1), Vector(0, 0, 1)},
        {Vector(s, -s, s), Vector(1, -1, 1), Vector(1, 0, 1)},
        {Vector(s, s, s), Vector(1, 1, 1), Vector(1, 1, 1)},
        {Vector(-s, s, s), Vector(-1, 1, 1), Vector(0, 1, 1)}
    };
    
    // Add vertices
    for (const auto& v : vertices) {
        cube.addVertex(v);
    }
    
    // Define triangles (6 faces, 2 triangles per face)
    int indices[] = {
        // Front face
        0, 1, 2, 0, 2, 3,
        // Back face
        4, 6, 5, 4, 7, 6,
        // Top face
        3, 2, 6, 3, 6, 7,
        // Bottom face
        0, 5, 1, 0, 4, 5,
        // Right face
        1, 5, 6, 1, 6, 2,
        // Left face
        0, 3, 7, 0, 7, 4
    };
    
    // Add triangles
    for (int i = 0; i < 36; i += 3) {
        Triangle t = {indices[i], indices[i+1], indices[i+2]};
        cube.addTriangle(t);
    }
    
    return cube;
} 