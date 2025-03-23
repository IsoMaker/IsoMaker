#include "Asset3D.hpp"

Asset3D::Asset3D()
{
    _modelLoaded = false;
}

Asset3D::~Asset3D()
{
}

Model Asset3D::getModel()
{
    if (_modelLoaded)
        return _model;
}

bool Asset3D::loadFile()
{
    if (!_modelLoaded) {
        _model = LoadModel(_fileName.c_str());
        if (_model.meshCount == 0)
            return false;
        _modelLoaded = true;
        std::cout << "ASSET 3D LOADED" << std::endl;
    }
    return true;
}

