#pragma once

#include <vector>
#include <algorithm>

#include "IAsset.hpp"

class AAsset : public IAsset
{
    public:
        AAsset() {};
        AAsset(std::string fileName) { _fileName = fileName; };
        ~AAsset() {};

        std::string getFileName() { return _fileName; };
        std::string getDisplayName() { return _displayName; };
        std::vector<std::string> getTags() { return _tags; };

        void setFileName(std::string fileName) { _fileName = fileName; };
        void setDisplayName(std::string displayName) { _displayName = displayName; };

        void setTags(std::vector<std::string> tags) { _tags = tags; };
        void addTags(std::vector<std::string> newTags) { _tags.insert(_tags.end(), newTags.begin(), newTags.end()); };
        void addTag(std::string newTag) { _tags.push_back(newTag); };

        void removeTag(std::string tag)
        {
            std::vector<std::string>::iterator it = std::find(_tags.begin(), _tags.end(), tag);
            if (it != _tags.end()) {
                _tags.erase(it);
            }
        };
        void removeTags(std::vector<std::string> tags)
        {
            for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
                removeTag(*it);
            }
        };
        bool findTag(std::string tag)
        {
            std::vector<std::string>::iterator it = std::find(_tags.begin(), _tags.end(), tag);
            if (it == _tags.end())
                return false;
            return true;
        };
        bool findAllTags(std::vector<std::string> tags)
        {
            for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
                if (!findTag(*it))
                    return false;
            }
            return true;
        };
        bool findAnyTags(std::vector<std::string> tags)
        {
            for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); it++) {
                if (findTag(*it))
                    return true;
            }
            return false;
        };

    protected:
        std::string _fileName;
        std::string _displayName;
        std::vector<std::string> _tags;
};
