#include "Simulator.h"
#include "World.h"
#include <iostream>

Simulator::Simulator(World& world) : world(world) {}

void Simulator::clear() {
    // Optionally clear world bodies if needed
}

void Simulator::calculateForces() {
    const double G = 6.67430e-11; // Gravitational constant
    size_t n = world.getBodyCount();
    // Reset accelerations
    for (size_t i = 0; i < n; ++i) {
        world.getBody(i).acceleration = Vector();
    }
    // Calculate gravitational forces between all pairs of bodies
    for (size_t i = 0; i < n; ++i) {
        for (size_t j = i + 1; j < n; ++j) {
            Vector r = world.getBody(j).position - world.getBody(i).position;
            double r_mag = r.magnitude();
            if (r_mag == 0) continue;
            double force_mag = G * world.getBody(i).mass * world.getBody(j).mass / (r_mag * r_mag);
            Vector force = r.normalize() * force_mag;
            world.getBody(i).acceleration = world.getBody(i).acceleration + force / world.getBody(i).mass;
            world.getBody(j).acceleration = world.getBody(j).acceleration - force / world.getBody(j).mass;
        }
    }
}

void Simulator::updatePositions(double dt) {
    size_t n = world.getBodyCount();
    for (size_t i = 0; i < n; ++i) {
        Body& body = world.getBody(i);
        body.velocity = body.velocity + body.acceleration * dt;
        body.position = body.position + body.velocity * dt;
    }
}

void Simulator::step(double dt) {
    calculateForces();
    updatePositions(dt);
} 