#include "Vector2D.h"
#include <cmath>

Vector2D::Vector2D(float x, float y) : x(x), y(y) {}

Vector2D Vector2D::operator+(const Vector2D& vec) const {
    return Vector2D(x + vec.x, y + vec.y);
}

Vector2D& Vector2D::operator+=(const Vector2D& vec) {
    x += vec.x;
    y += vec.y;
    return *this;
}

Vector2D Vector2D::operator-(const Vector2D& vec) const {
    return Vector2D(x - vec.x, y - vec.y);
}

Vector2D& Vector2D::operator-=(const Vector2D& vec) {
    x -= vec.x;
    y -= vec.y;
    return *this;
}

Vector2D Vector2D::operator*(float scalar) const {
    return Vector2D(x * scalar, y * scalar);
}

Vector2D& Vector2D::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector2D Vector2D::operator/(float scalar) const {
    if (scalar == 0) {
        // Consider throwing an exception or returning a zero vector
        return Vector2D(0, 0);
    }
    return Vector2D(x / scalar, y / scalar);
}

Vector2D& Vector2D::operator/=(float scalar) {
    if (scalar == 0) {
        // Consider appropriate error handling
        x = 0;
        y = 0;
    } else {
        x /= scalar;
        y /= scalar;
    }
    return *this;
}

float Vector2D::length() const {
    return std::sqrt(x * x + y * y);
}

Vector2D Vector2D::normalize() const {
    float l = length();
    if (l > 0) {
        return (*this) * (1.0f / l);
    }
    return Vector2D(); // Return zero vector if length is zero
}

std::ostream& operator<<(std::ostream& stream, const Vector2D& vec) {
    stream << "(" << vec.x << ", " << vec.y << ")";
    return stream;
}
