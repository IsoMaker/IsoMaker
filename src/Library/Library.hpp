#pragma once

#include <vector>
#include <limits>
#include <functional>
#include <iostream>
#include <utility>

#include "../Assets/Asset3D.hpp"
#include "../Assets/Asset2D.hpp"
#include "../Utilities/Vector.hpp"
#include "../Input/Mouse.hpp"
#include "../../includes/macros.hpp"

class Library {
    public:
        Library();
        ~Library() {};

        void add3DAsset(std::string filename);
        void remove3DAsset(std::string &filename);

        void update();
        void draw();
        Asset3D get3DAssetFromClick(input::MouseHandler &mouseHandler);

    protected:
        bool isLibDisplayed;
        int displayedLib; //0 = 3d objects

        Rectangle _libRectangle;
        std::vector<std::pair<Utilities::Vector2D, std::vector<Asset3D>::iterator>> _currentlyDisplayed;

        std::vector<Asset3D> _assets3D;
        std::vector<Asset2D> _assets2D;


};