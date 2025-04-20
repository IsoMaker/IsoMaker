#pragma once

#include "AAsset.hpp"
#include "raymath.h"
#include <iostream>

class Asset3D : public AAsset
{
    public:
        Asset3D();
        ~Asset3D();
        Model getModel();
        void loadFile();
        // void rotateModel(float angle);

    protected:
        Model _model;
        bool _modelLoaded;
};
