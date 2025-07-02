#pragma once

#include "Assets/AAsset.hpp"

class Asset2D : public AAsset
{
    public:
        Asset2D();
        Asset2D(std::string fileName);
        Asset2D(Texture2D texture);
        ~Asset2D() = default;
        Texture2D getTexture();
        void setTexture(Texture2D texture);
        void loadFile();
        bool isLoaded() { return _textureLoaded; };

    protected:
        bool _textureLoaded;
        Texture2D _texture;
};
