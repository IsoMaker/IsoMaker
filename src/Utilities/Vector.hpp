#pragma once

#include "raylib.h"
#include "raymath.h"

namespace Utilities {
    class Vector2D {
        public:
            Vector2D();
            Vector2D(const Vector2D& other);
            Vector2D(const Vector2& other);
            Vector2D(float, float);
            ~Vector2D();

            Vector2D operator+ (const Vector2D& v) const;
            Vector2D operator- (const Vector2D& v) const;
            Vector2D operator* (const Vector2D& v) const;
            Vector2D operator/ (const Vector2D& v) const;
            Vector2D operator- (const float& v) const;
            Vector2D operator+ (const float& v) const;
            Vector2D operator* (const float& v) const;
            Vector2D operator/ (const float& v) const;

            bool operator== (const Vector2D& v) const;
            bool operator!= (const Vector2D& v) const;

            Vector2 convert();

            float x, y;
    };

    class Vector3D {
        public:
            Vector3D();
            Vector3D(const Vector3D& other);
            Vector3D(const Vector3& other);
            Vector3D(float, float, float);
            ~Vector3D();

            Vector3D operator+ (const Vector3D& v) const;
            Vector3D operator- (const Vector3D& v) const;
            Vector3D operator* (const Vector3D& v) const;
            Vector3D operator/ (const Vector3D& v) const;
            Vector3D operator- (const float& v) const;
            Vector3D operator+ (const float& v) const;
            Vector3D operator* (const float& v) const;
            Vector3D operator/ (const float& v) const;
            Vector3D operator+ (const Vector2D& v) const;
            Vector3D operator- (const Vector2D& v) const;
            Vector3D operator* (const Vector2D& v) const;
            Vector3D operator/ (const Vector2D& v) const;

            bool operator== (const Vector3D& v) const;
            bool operator!= (const Vector3D& v) const;

            Vector3 convert();

            float x, y, z;
    };

}
