#include "Vector.hpp"

using namespace Utilities;

// VECTOR 2D
Vector2D::Vector2D(const Vector2D& other) {
    x = other.x;
    y = other.y;
}

Vector2D::Vector2D(const Vector2& other) {
    x = other.x;
    y = other.y;
}

Vector2D::Vector2D(float otherX, float otherY) {
    x = otherX;
    y = otherY;
}

Vector2D Vector2D::operator+ (const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
Vector2D Vector2D::operator- (const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
Vector2D Vector2D::operator* (const Vector2D& v) const { return Vector2D(x * v.x, y * v.y); }
Vector2D Vector2D::operator/ (const Vector2D& v) const { return Vector2D(x / v.x, y / v.y); }

Vector2D Vector2D::operator+ (const float& v) const { return Vector2D(x + v, y + v); }
Vector2D Vector2D::operator- (const float& v) const { return Vector2D(x - v, y - v); }
Vector2D Vector2D::operator* (const float& v) const { return Vector2D(x * v, y * v); }
Vector2D Vector2D::operator/ (const float& v) const { return Vector2D(x / v, y / v); }

bool Vector2D::operator< (const Vector2D& v) const {return x < v.x && y < v.y;}
bool Vector2D::operator> (const Vector2D& v) const {return x > v.x && y > v.y;}

bool Vector2D::operator== (const Vector2D& v) const {
    if (x == v.x && y == v.y)
        return true;
    return false;
}

bool Vector2D::operator!= (const Vector2D& v) const {
    if (x != v.x || y != v.y)
        return true;
    return false;
}

Vector2 Vector2D::convert() {
    return Vector2{x, y};
}

//VECTOR 3D
Vector3D::Vector3D(const Vector3D& other) {
    x = other.x;
    y = other.y;
    z = other.z;
}

Vector3D::Vector3D(const Vector3& other) {
    x = other.x;
    y = other.y;
    z = other.z;
}

Vector3D::Vector3D(float otherX, float otherY, float otherZ) {
    x = otherX;
    y = otherY;
    z = otherZ;
}

Vector3D Vector3D::operator+ (const Vector3D& v) const { return Vector3D(x + v.x, y + v.y, z + v.z); }
Vector3D Vector3D::operator- (const Vector3D& v) const { return Vector3D(x - v.x, y - v.y, z - v.z); }
Vector3D Vector3D::operator* (const Vector3D& v) const { return Vector3D(x * v.x, y * v.y, z * v.z); }
Vector3D Vector3D::operator/ (const Vector3D& v) const { return Vector3D(x / v.x, y / v.y, z / v.z); }

Vector3D Vector3D::operator+ (const float& v) const { return Vector3D(x + v, y + v, z + v); }
Vector3D Vector3D::operator- (const float& v) const { return Vector3D(x - v, y - v, z - v); }
Vector3D Vector3D::operator* (const float& v) const { return Vector3D(x * v, y * v, z * v); }
Vector3D Vector3D::operator/ (const float& v) const { return Vector3D(x / v, y / v, z / v); }

Vector3D Vector3D::operator+ (const Vector2D& v) const { return Vector3D(x + v.x, y, z + v.y); }
Vector3D Vector3D::operator- (const Vector2D& v) const { return Vector3D(x - v.x, y, z - v.y); }
Vector3D Vector3D::operator* (const Vector2D& v) const { return Vector3D(x * v.x, y, z * v.y); }
Vector3D Vector3D::operator/ (const Vector2D& v) const { return Vector3D(x / v.x, y, z / v.y); }

bool Vector3D::operator== (const Vector3D& v) const {
    if (x == v.x && y == v.y && z == v.z)
        return true;
    return false;
}

bool Vector3D::operator!= (const Vector3D& v) const {
    if (x != v.x || y != v.y || z != v.z)
        return true;
    return false;
}

Vector3 Vector3D::convert() {
    return Vector3{x, y, z};
}
