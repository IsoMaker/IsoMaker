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
        void setModelTexture(Texture2D texture);
        void loadFile();

    protected:
        Model _model;
        bool _modelLoaded;
};
