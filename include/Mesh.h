#pragma once
#include "Vector.h"
#include <vector>
#include <string>

struct Vertex {
    Vector position;
    Vector normal;
    Vector texCoord;  // For texture coordinates
};

struct Triangle {
    int v1, v2, v3;  // Indices into vertex array
};

class Mesh {
public:
    Mesh() = default;
    
    // Mesh data
    void addVertex(const Vertex& vertex) { vertices.push_back(vertex); }
    void addTriangle(const Triangle& triangle) { triangles.push_back(triangle); }
    
    // Mesh properties
    void setName(const std::string& newName) { name = newName; }
    void setColor(const Vector& newColor) { color = newColor; }
    
    // Getters
    const std::vector<Vertex>& getVertices() const { return vertices; }
    const std::vector<Triangle>& getTriangles() const { return triangles; }
    const std::string& getName() const { return name; }
    const Vector& getColor() const { return color; }
    
    // Mesh operations
    void clear() {
        vertices.clear();
        triangles.clear();
    }
    
    // Create a simple sphere mesh
    static Mesh createSphere(double radius, int segments);
    
    // Create a simple cube mesh
    static Mesh createCube(double size);

private:
    std::vector<Vertex> vertices;
    std::vector<Triangle> triangles;
    std::string name;
    Vector color = Vector(1, 1, 1);  // Default white color
}; 