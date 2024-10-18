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

void Asset3D::loadFile()
{
    _model = LoadModel(_fileName.c_str());
    _modelLoaded = true;
}

