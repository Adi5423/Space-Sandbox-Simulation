#pragma once
#include "Vector.h"

class Body {
public:
    double mass;
    Vector position, velocity, acceleration;
    // (Later) Add name, color, mesh, etc.

    Body(double mass = 1.0, 
         const Vector& pos = Vector(), 
         const Vector& vel = Vector());
}; 