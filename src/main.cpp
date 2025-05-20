#include <iostream>
#include <iomanip>
#include "Vector.h"
#include "Body.h"
#include "World.h"
#include "Simulator.h"
#include "Camera.h"
#include "Mesh.h"
#include "Renderer.h"

int main() {
    std::cout << "AstroSandbox v0.1 - Starting 3D simulation...\n\n";

    // Create renderer
    Renderer renderer(1280, 720, "AstroSandbox 3D");
    if (!renderer.initialize()) {
        std::cerr << "Failed to initialize renderer" << std::endl;
        return 1;
    }

    // Create world with camera and meshes
    World world;
    
    // Set up camera
    Camera camera(Vector(0, 0, 2000), Vector(0, 0, 0));
    world.setMainCamera(camera);
    
    // Add some meshes
    world.addMesh("earth", Mesh::createSphere(6371, 32));  // Earth radius in km
    world.addMesh("moon", Mesh::createSphere(1737, 16));   // Moon radius in km
    
    // Add bodies with their meshes
    world.addBody(Body(5.972e24, Vector(0, 0, 0), Vector(0, 0, 0)));
    world.addBody(Body(7.342e22, Vector(384400e3, 0, 0), Vector(0, 1022, 0)));

    // Create simulator
    Simulator sim(world);
    
    // Main loop
    const double dt = 1.0;
    while (!renderer.shouldClose()) {
        // Handle input
        renderer.pollEvents();
        
        // Update simulation
        sim.step(dt);
        
        // Update camera to follow moon
        world.getMainCamera().lookAt(world.getBody(1).position);
        
        // Render scene
        renderer.render(world);
        renderer.swapBuffers();
    }

    return 0;
} 