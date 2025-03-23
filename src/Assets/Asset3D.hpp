#pragma once

#include "AAsset.hpp"

#include <iostream>

class Asset3D : public AAsset
{
    public:
        Asset3D();
        ~Asset3D();
        Model getModel();
        bool loadFile();

    protected:
        Model _model;
        bool _modelLoaded;
};
