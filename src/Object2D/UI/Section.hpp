#pragma once

#include "../BasicObject2D.hpp"

using namespace Utilities;

namespace Object2D {

    class Section : public BasicObject2D {
        public:
            void draw();
            void addSubsection(std::shared_ptr<BasicObject2D>);

        private:
            ObjectBox2D _objectBox;
            Asset2D _asset;
            std::vector<std::shared_ptr<BasicObject2D>> _subsections;
    };

}