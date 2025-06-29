#pragma once

#include "AAsset.hpp"

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
        int getWidth() { return _width; };
        int getHeight() { return _width; };
        int getFramesCount() { return _width; };
        float getScale() { return _scale; };

        void setWidth(int width) { _width = width; };
        void setHeight(int height) { _height = height; };
        void setFramesCount(int frames) { _frames = frames; };

    protected:
        bool _textureLoaded;
        Texture2D _texture;
        int _width;
        int _height;
        int _frames;
};
