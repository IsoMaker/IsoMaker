#pragma once

#include "AAsset.hpp"
#include "raymath.h"
#include <iostream>

class Asset3D : public AAsset
{
    public:
        Asset3D();
        Asset3D(std::string fileName);
        ~Asset3D() = default;
        Model getModel();
        void setModel(Model model);
        void setModelTexture(Texture2D texture);
        void loadFile();
        bool isLoaded() { return _modelLoaded; };

    protected:
        Model _model;
        bool _modelLoaded;
};
