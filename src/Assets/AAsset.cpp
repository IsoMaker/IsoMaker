#include "AAsset.hpp"

AAsset::AAsset()
{
}

AAsset::~AAsset()
{
}

std::string AAsset::getFileName()
{
    return _fileName;
}

std::string AAsset::getDisplayName()
{
    return _displayName;
}

std::vector<std::string> AAsset::getTags()
{
    return _tags;
}

void AAsset::setFileName(std::string fileName)
{
    _fileName = fileName;
}

void AAsset::setDisplayName(std::string displayName)
{
    _displayName = displayName;
}

void AAsset::setTags(std::vector<std::string> tags)
{
    _tags = tags;
}

void AAsset::addTags(std::vector<std::string> newTags)
{
    _tags.insert(_tags.end(), newTags.begin(), newTags.end());
}

void AAsset::addTag(std::string newTag)
{
    _tags.push_back(newTag);
}

void AAsset::removeTags(std::vector<std::string> tags)
{
    for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
        removeTag(*it);
    }
}

void AAsset::removeTag(std::string tag)
{
    std::vector<std::string>::iterator it = find(_tags.begin(), _tags.end(), tag);
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
    std::vector<std::string>::iterator it = find(_tags.begin(), _tags.end(), tag);
    if (it == _tags.end())
        return false;
    return true;
}

