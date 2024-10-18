#include "Asset2D.hpp"

Asset2D::Asset2D()
{
    _textureLoaded = false;
}

Asset2D::~Asset2D()
{
}

Texture2D Asset2D::getTexture()
{
    if (_textureLoaded)
        return _texture;
}

void Asset2D::loadFile()
{
    _texture = LoadTexture(_fileName.c_str());
    _textureLoaded = true;
}

