#include "Body.h"

Body::Body(double mass, const Vector& pos, const Vector& vel)
    : mass(mass), position(pos), velocity(vel), acceleration() {} 