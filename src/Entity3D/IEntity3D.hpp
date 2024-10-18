#ifndef IENTITY_3D_HPP_
#define IENTITY_3D_HPP_

#include "../Utilities/Vector.hpp"

using namespace Utilities;

namespace Entity3D {

class IEntity3D
{
    public:
        virtual void draw() = 0;
        virtual void move(Vector3D) = 0;
        virtual void moveTo(Vector3D) = 0;
        virtual void resizePercent(float) = 0;
};

}

#endif