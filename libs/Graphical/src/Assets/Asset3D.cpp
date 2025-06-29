#include "Asset3D.hpp"

Asset3D::Asset3D()
{
    _modelLoaded = false;
}

Asset3D::Asset3D(std::string fileName) : AAsset(fileName)
{
    loadFile();
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

void Asset3D::setModel(Model model)
{
    _model = model;
}

void Asset3D::setModelTexture(Texture2D texture)
{
    _model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;
}

// void Asset3D::rotateModel(float angle)
// {
//     if (_modelLoaded) {
//         _model.transform = MatrixIdentity();
//         Matrix rotation = MatrixRotate(Vector3{ 1, 0, 0 }, angle);
//         _model.transform = MatrixMultiply(_model.transform, rotation);
//     }
// }
