#pragma once
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include "Body.h"
#include "Camera.h"
#include "Mesh.h"

class World {
public:
    World();
    
    // Body management
    void addBody(const Body& body);
    size_t getBodyCount() const;
    const Body& getBody(size_t idx) const;
    Body& getBody(size_t idx);
    
    // Camera management
    void setMainCamera(const Camera& camera);
    const Camera& getMainCamera() const;
    Camera& getMainCamera();
    
    // Mesh management
    void addMesh(const std::string& name, const Mesh& mesh);
    const Mesh* getMesh(const std::string& name) const;
    Mesh* getMesh(const std::string& name);
    
    // Scene management
    void clear();

private:
    std::vector<Body> bodies;
    Camera mainCamera;
    std::unordered_map<std::string, Mesh> meshes;
}; 