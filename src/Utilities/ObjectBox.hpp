#pragma once

#include "Vector.hpp"
#include "raylib.h"

namespace Utilities {

    class ObjectBox2D {
        public:
            ObjectBox2D();
            ObjectBox2D(Vector2D, Vector2D, float scale = 1);
            ObjectBox2D(const ObjectBox2D&);
            ~ObjectBox2D();

            Vector2D position;
            Vector2D assetDimensions;
            float scale;
    };

    class ObjectBox3D {
        public:
            ObjectBox3D();
            ObjectBox3D(Vector3D, Vector3D, float scale = 1);
            ObjectBox3D(const ObjectBox3D&);
            ~ObjectBox3D();

            BoundingBox convert();
            void ObjectBox3D::setAssetDimensionsFromBounding(BoundingBox);

            Vector3D position;
            Vector3D assetDimensions;
            float scale;

    };

}