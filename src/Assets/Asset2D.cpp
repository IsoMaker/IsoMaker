#include "Asset2D.hpp"

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

