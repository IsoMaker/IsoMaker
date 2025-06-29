#pragma once

#include <vector>
#include <algorithm>

#include "IAsset.hpp"

class AAsset : public IAsset
{
    public:
        AAsset() = default;
        AAsset(std::string fileName);
        ~AAsset()  = default;

        std::string getFileName() { return _fileName; };
        std::string getDisplayName() { return _displayName; };

        std::vector<std::string> getTags() { return _tags; };

        void setFileName(std::string fileName) { _fileName = fileName; };
        void setDisplayName(std::string displayName) { _displayName = displayName; };

        void setTags(std::vector<std::string> tags) { _tags = tags; };
        void addTags(std::vector<std::string> newTags) { _tags.insert(_tags.end(), newTags.begin(), newTags.end()); };
        void addTag(std::string newTag) { _tags.push_back(newTag); };

        void removeTags(std::vector<std::string> tags);
        void removeTag(std::string tag);
        bool findAllTags(std::vector<std::string>);
        bool findAnyTags(std::vector<std::string>);
        bool findTag(std::string);

    protected:
        std::string _fileName;
        std::string _displayName;
        std::vector<std::string> _tags;
};
