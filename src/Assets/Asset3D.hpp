#ifndef ASSET3D_HPP_
#define ASSET3D_HPP_

#include "AAsset.hpp"

class Asset3D : public AAsset
{
    public:
        Asset3D();
        ~Asset3D();
        Model getModel();
        void loadFile();

    protected:
        Model _model;
        bool _modelLoaded;
};

#endif