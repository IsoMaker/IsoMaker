#include "Asset2D.hpp"

Texture2D Asset2D::getTexture()
{
    if (_textureLoaded)
        return _texture;
    return Texture2D();
}

void Asset2D::loadFile()
{
    _texture = LoadTexture(_fileName.c_str());
    if (_texture.id != 0) {
        _textureLoaded = true;
    } else {
        _textureLoaded = false;
    }
}
