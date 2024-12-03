#pragma once

#include <memory>
#include <string>
#include "raylib.h"

class IAsset
{
    public:
        virtual std::string getFileName() = 0;
        virtual std::string getDisplayName() = 0;

        virtual std::vector<std::string> getTags() = 0;

        virtual void setFileName(std::string) = 0;
        virtual void setDisplayName(std::string) = 0;

        virtual void setTags(std::vector<std::string>) = 0;
        virtual void addTags(std::vector<std::string>) = 0;
        virtual void addTag(std::string) = 0;
        virtual void removeTags(std::vector<std::string>) = 0;
        virtual void removeTag(std::string) = 0;
        virtual bool findAllTags(std::vector<std::string>) = 0;
        virtual bool findAnyTags(std::vector<std::string>) = 0;
        virtual bool findTag(std::string) = 0;

        virtual void loadFile() = 0;

        //virtual AssetType getType(); ?
        //set/get/load preview ?
};
