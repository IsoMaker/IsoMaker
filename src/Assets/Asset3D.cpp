#include "Asset3D.hpp"

Asset3D::Asset3D()
{
    _modelLoaded = false;
}

Asset3D::~Asset3D()
{
}

void Asset3D::loadFile()
{
    _model = LoadModel(_fileName.c_str());
    _modelLoaded = true;
}

Model Asset3D::getModel()
{
    if (_modelLoaded)
        return _model;
    return Model();
}

// void Asset3D::rotateModel(float angle)
// {
//     if (_modelLoaded) {
//         _model.transform = MatrixIdentity();
//         Matrix rotation = MatrixRotate(Vector3{ 1, 0, 0 }, angle);
//         _model.transform = MatrixMultiply(_model.transform, rotation);
//     }
// }
