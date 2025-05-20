#pragma once
#include "Vector.h"
#include <cmath>

class Camera {
public:
    Camera(const Vector& position = Vector(0, 0, 1000),
           const Vector& target = Vector(0, 0, 0),
           const Vector& up = Vector(0, 1, 0))
        : position(position), target(target), up(up), fov(45.0), aspect(1.0), near(0.1), far(10000.0) {}

    // Camera movement
    void move(const Vector& delta) { position = position + delta; }
    void lookAt(const Vector& newTarget) { target = newTarget; }
    void setPosition(const Vector& newPos) { position = newPos; }

    // Camera properties
    void setFOV(double newFOV) { fov = newFOV; }
    void setAspect(double newAspect) { aspect = newAspect; }
    void setClipping(double newNear, double newFar) {
        near = newNear;
        far = newFar;
    }

    // Getters
    const Vector& getPosition() const { return position; }
    const Vector& getTarget() const { return target; }
    const Vector& getUp() const { return up; }
    double getFOV() const { return fov; }
    double getAspect() const { return aspect; }
    double getNear() const { return near; }
    double getFar() const { return far; }

    // Calculate view direction
    Vector getViewDirection() const {
        return (target - position).normalize();
    }

private:
    Vector position;  // Camera position
    Vector target;    // Look-at target
    Vector up;        // Up vector
    double fov;       // Field of view in degrees
    double aspect;    // Aspect ratio
    double near;      // Near clipping plane
    double far;       // Far clipping plane
}; 