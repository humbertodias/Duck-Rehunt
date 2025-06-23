#ifndef VECTOR2D_H
#define VECTOR2D_H

#include <iostream>

class Vector2D {
public:
    float x;
    float y;

    Vector2D(float x = 0.0f, float y = 0.0f);

    // Basic operations
    Vector2D operator+(const Vector2D& vec) const;
    Vector2D& operator+=(const Vector2D& vec);
    Vector2D operator-(const Vector2D& vec) const;
    Vector2D& operator-=(const Vector2D& vec);
    Vector2D operator*(float scalar) const;
    Vector2D& operator*=(float scalar);
    Vector2D operator/(float scalar) const;
    Vector2D& operator/=(float scalar);

    float length() const;
    Vector2D normalize() const;

    friend std::ostream& operator<<(std::ostream& stream, const Vector2D& vec);
};

#endif // VECTOR2D_H
