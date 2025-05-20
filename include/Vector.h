#pragma once
#include <cmath>

struct Vector {
    double x, y, z;

    // Constructors
    Vector(double x = 0, double y = 0, double z = 0) : x(x), y(y), z(z) {}

    // Basic arithmetic operations
    Vector operator+(const Vector& other) const {
        return Vector(x + other.x, y + other.y, z + other.z);
    }

    Vector operator-(const Vector& other) const {
        return Vector(x - other.x, y - other.y, z - other.z);
    }

    Vector operator*(double scalar) const {
        return Vector(x * scalar, y * scalar, z * scalar);
    }

    Vector operator/(double scalar) const {
        return Vector(x / scalar, y / scalar, z / scalar);
    }

    // Dot product
    double dot(const Vector& other) const {
        return x * other.x + y * other.y + z * other.z;
    }

    // Cross product
    Vector cross(const Vector& other) const {
        return Vector(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    // Magnitude (length) of the vector
    double magnitude() const {
        return std::sqrt(x * x + y * y + z * z);
    }

    // Normalize the vector (make it unit length)
    Vector normalize() const {
        double mag = magnitude();
        if (mag == 0) return Vector();
        return *this / mag;
    }
}; 