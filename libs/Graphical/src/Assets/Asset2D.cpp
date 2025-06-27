#include "Asset2D.hpp"

Asset2D::Asset2D()
{
    _textureLoaded = false;
}

Asset2D::Asset2D(Texture2D texture)
{
    setTexture(texture);
}

Texture2D Asset2D::getTexture()
{
    if (_textureLoaded)
        return _texture;
    return Texture2D();
}

void Asset2D::setTexture(Texture2D texture)
{
    _texture = texture;
    if (_texture.id != 0) {
        _textureLoaded = true;
    } else {
        _textureLoaded = false;
    }
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
