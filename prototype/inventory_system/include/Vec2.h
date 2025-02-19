// Vec2.h
#pragma once
#include <cmath>

class Vec2 {
public:
    float x, y;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float x, float y) : x(x), y(y) {}

    // Addition
    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    // Subtraction
    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    // Scalar multiplication
    Vec2 operator*(float scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    // Compound operators
    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(float scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Length/magnitude of vector
    float length() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector (make it length 1)
    Vec2 normalize() const {
        float len = length();
        if (len == 0.0f) return Vec2();
        return Vec2(x / len, y / len);
    }

    // Dot product
    float dot(const Vec2& other) const {
        return x * other.x + y * other.y;
    }
};
