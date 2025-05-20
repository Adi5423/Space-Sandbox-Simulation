#include "World.h"

World::World() : mainCamera() {}

void World::addBody(const Body& body) {
    bodies.push_back(body);
}

size_t World::getBodyCount() const {
    return bodies.size();
}

const Body& World::getBody(size_t idx) const {
    return bodies[idx];
}

Body& World::getBody(size_t idx) {
    return bodies[idx];
}

void World::setMainCamera(const Camera& camera) {
    mainCamera = camera;
}

const Camera& World::getMainCamera() const {
    return mainCamera;
}

Camera& World::getMainCamera() {
    return mainCamera;
}

void World::addMesh(const std::string& name, const Mesh& mesh) {
    meshes[name] = mesh;
}

const Mesh* World::getMesh(const std::string& name) const {
    auto it = meshes.find(name);
    return it != meshes.end() ? &it->second : nullptr;
}

Mesh* World::getMesh(const std::string& name) {
    auto it = meshes.find(name);
    return it != meshes.end() ? &it->second : nullptr;
}

void World::clear() {
    bodies.clear();
    meshes.clear();
} 