#pragma once

#include "Vector.hpp"
#include "raylib.h"

namespace Utilities {

class ObjectBox2D
{
    public:
        ObjectBox2D();
        ObjectBox2D(Vector2D, Vector2D, float scale = 1);
        ObjectBox2D(const ObjectBox2D&);
        ~ObjectBox2D();

        Vector2D getPosition() const;
        Vector2D getDimensions() const;
        Vector2D getAssetDimensions() const;
        float getScale() const;

        void setPosition(Vector2D);
        void setAssetDimensions(Vector2D);
        void setScale(float);

    private:
        Vector2D _position;
        Vector2D _assetDimensions;
        float _scale;
};

class ObjectBox3D
{
    public:
        ObjectBox3D();
        ObjectBox3D(Vector3D, Vector3D, float scale = 1);
        ObjectBox3D(const ObjectBox3D&);
        ~ObjectBox3D();

        Vector3D getPosition() const;
        Vector3D getDimensions() const;
        Vector3D getAssetDimensions() const;
        float getScale() const;

        void setPosition(Vector3D);
        void setAssetDimensions(Vector3D);
        void setAssetDimensionsFromBounding(BoundingBox);
        void setScale(float);

        BoundingBox convert();

    private:
        Vector3D _position;
        Vector3D _assetDimensions;
        float _scale;

};

}
