#include "Section.hpp"

using namespace Object2D;

void Section::draw() {
    DrawTextureEx(_asset.getTexture(), _objectBox.position.convert(), 0, _objectBox.scale, WHITE);
    for (std::vector<std::shared_ptr<BasicObject2D>>::iterator it = _subsections.begin(); it != _subsections.end(); ++it)
        (*it)->draw();
}

void Section::addSubsection(std::shared_ptr<BasicObject2D> newSubsection) {
    _subsections.push_back(newSubsection);
}