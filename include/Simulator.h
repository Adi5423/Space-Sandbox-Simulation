#pragma once
#include "World.h"

class Simulator {
public:
    Simulator(World& world);
    void step(double dt);
    void clear();

private:
    World& world;
    void calculateForces();
    void updatePositions(double dt);
}; 