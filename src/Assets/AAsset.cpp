#include "AAsset.hpp"

void AAsset::removeTags(std::vector<std::string> tags)
{
    for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
        removeTag(*it);
    }
}

void AAsset::removeTag(std::string tag)
{
    std::vector<std::string>::iterator it = std::find(_tags.begin(), _tags.end(), tag);
    if (it != _tags.end()) {
        _tags.erase(it);
    }
}

bool AAsset::findAllTags(std::vector<std::string> tag)
{
    for (std::vector<std::string>::iterator it = tag.begin(); it != tag.end(); it++) {
        if (!findTag(*it))
            return false;
    }
    return true;
}

bool AAsset::findAnyTags(std::vector<std::string> tags)
{
    for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
        if (findTag(*it))
            return true;
    }
    return false;
}

bool AAsset::findTag(std::string tag)
{
    std::vector<std::string>::iterator it = std::find(_tags.begin(), _tags.end(), tag);
    if (it == _tags.end())
        return false;
    return true;
}

void AAsset::displayLibraryAsset(int x, int y)
{
    DrawRectangle(x, y, ASSETWIDTH, ASSETHEIGHT, LIGHTGRAY);
    DrawText(_fileName.c_str(), x, y + ASSETHEIGHT/2 - 10, 20, BLACK);
}
