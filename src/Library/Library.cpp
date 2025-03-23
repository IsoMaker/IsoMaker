#include "Library.hpp"

Library::Library() {
    _libRectangle = {SCREENWIDTH - ASSETWIDTH, 0, ASSETWIDTH, SCREENHEIGHT};
    isLibDisplayed = true;
    displayedLib = 0;
}

void Library::add3DAsset(std::string filename)
{
    Asset3D newAsset;
    newAsset.setFileName(filename);
    if (newAsset.loadFile()) {
        _assets3D.push_back(newAsset);
    }
    update();
}

void Library::remove3DAsset(std::string &filename)
{
    _assets3D.erase(std::remove_if(_assets3D.begin(), _assets3D.end(), [&filename](Asset3D& model) {
        return model.getFileName() == filename;
    }), _assets3D.end());
    update();
}

void Library::update()
{
    _currentlyDisplayed.clear();
    int assetsPerPage = SCREENHEIGHT / (ASSETHEIGHT + 5);
    int currentHeight = 0;
    for (auto i = _assets3D.begin(); i != _assets3D.begin() + assetsPerPage && i != _assets3D.end(); i++) {
        std::pair<Utilities::Vector2D, std::vector<Asset3D>::iterator> temp = std::make_pair(Utilities::Vector2D(_libRectangle.x, currentHeight), i);
        _currentlyDisplayed.push_back(temp);
        currentHeight += ASSETHEIGHT + 5;
    }
}

void Library::draw()
{
    if (!isLibDisplayed)
        return;

    if (displayedLib == 0) {
        DrawRectangleRec(_libRectangle, WHITE);
        for (auto i = _currentlyDisplayed.begin(); i != _currentlyDisplayed.end(); i++) {
            i->second->displayLibraryAsset(i->first.x, i->first.y);
        }
    }
}

Asset3D Library::get3DAssetFromClick(input::MouseHandler &mouseHandler)
{
    if (mouseHandler.isReleased(input::Generic::SELECT1)) {
        std::cout << "SOMETHING CLICKED" << std::endl;
        //Utilities::Vector2D mousePos = mouseHandler.getMouseCoords();
        //for (auto i = _currentlyDisplayed.begin(); i != _currentlyDisplayed.end(); i++) {
        //    if (mousePos > i->first && mousePos < (i->first + Utilities::Vector2D(ASSETHEIGHT, ASSETWIDTH))) {
        //        std::cout << "ASSET CLICKED" << std::endl;
        //        return *i->second;
        //    }
        //}
    }
}