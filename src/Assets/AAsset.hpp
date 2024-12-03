#pragma once

#include "IAsset.hpp"

class AAsset : public IAsset
{
    public:
        AAsset();
        ~AAsset();

        std::string getFileName();
        std::string getDisplayName();

        std::vector<std::string> getTags();

        void setFileName(std::string);
        void setDisplayName(std::string);

        void setTags(std::vector<std::string>);
        void addTags(std::vector<std::string>);
        void addTag(std::string);
        void removeTags(std::vector<std::string>);
        void removeTag(std::string);
        bool findAllTags(std::vector<std::string>);
        bool findAnyTags(std::vector<std::string>);
        bool findTag(std::string);

    protected:
        std::string _fileName;
        std::string _displayName;
        std::vector<std::string> _tags;
};
