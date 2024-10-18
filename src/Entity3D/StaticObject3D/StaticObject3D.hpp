#ifndef BLOCK_HPP_
#define BLOCK_HPP_

#include "../IEntity3D.hpp"
#include "../../Assets/Asset2D.hpp"
#include "../../Assets/Asset3D.hpp"
#include "../../Utilities/ObjectBox.hpp"

using namespace Utilities;

namespace Entity3D {

class StaticObject3D : public IEntity3D
{
    public:
        StaticObject3D();
        StaticObject3D(Asset3D, Vector3D);
        ~StaticObject3D();

        void setAsset(Asset3D);
        void setAsset(Asset2D);
        void draw();
        void move(Vector3D);
        void moveTo(Vector3D);
        void resizePercent(float);
        ObjectBox3D getBox();

    private:
        ObjectBox3D _objectBox;
        Asset3D _asset;
};

}

#endif